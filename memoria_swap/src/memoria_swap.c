#include "memoria_swap.h"

pthread_mutex_t mutexIndice,mutexIndice2doNivel;

int main(int argc, char **argv){

    char* log_level = "debug";

    pthread_mutex_init(&mutexIndice, NULL);
    pthread_mutex_init(&mutexIndice2doNivel, NULL);

    memoria_swapLogger = log_create(MEMORIA_SWAP_LOG_DEST, MEMORIA_SWAP_MODULE_NAME, true, determinar_nivel_de_log(log_level));
    memoria_swapCfg = memoria_swap_cfg_create();
    cargar_configuracion(MEMORIA_SWAP_MODULE_NAME, memoria_swapCfg, MEMORIA_SWAP_CFG_PATH, memoria_swapLogger, memoria_swap_config_initialize);

    inicializar_tabla_paginas();
    inicializar_lista_marcos_libres();
    MEMORIA_PRINCIPAL = crear_espacio_de_memoria();

    nextIndicePrimerNv = 0;
    nextIndiceSegundoNv = 0; 
    marcosPorProceso = memoria_swapCfg->MARCOS_POR_PROCESO;
    procesosSuspendidos = list_create();
    marcosAsignadosPorProceso = list_create();

    int socket_servidor = iniciar_servidor(memoria_swapCfg->IP_MEMORIA, memoria_swapCfg->PUERTO_ESCUCHA);
    struct sockaddr cliente;
    socklen_t lenCliD = sizeof(cliente);

    conexion* conexionCpu = malloc(sizeof(conexion));
    conexion* conexionKernel = malloc(sizeof(conexion));

    conexionCpu->socket=socket_servidor;
    conexionCpu->sockAddr=cliente;
    conexionCpu->sockrAddrLen=lenCliD;

    conexionKernel->socket=socket_servidor;
    conexionKernel->sockAddr=cliente;
    conexionKernel->sockrAddrLen=lenCliD;

    pthread_t atenderCpu;
    pthread_t atenderKernel;
    
    int socketCpu = aceptar_conexion_memoria(conexionCpu); //supuestamente kernel no arranca hasta que cpu este levantado asi que no deberia entrar aca
    if(socketCpu > 0) {
        log_info(memoria_swapLogger, "Memoria: Acepto la conexión del CPU con socket: %d", socketCpu);
        recibir_handshake(socketCpu);
        pthread_create(&atenderCpu, NULL, recibir_instrucciones_cpu, socketCpu);
    } else {
        log_error(memoria_swapLogger, "Memoria: Error al aceptar conexión: %s", strerror(errno));
    }
    

    int socketKernel = aceptar_conexion_memoria(conexionKernel);
    if(socketKernel > 0) {
        log_info(memoria_swapLogger, "Memoria: Acepto la conexión del Kernel con socket: %d", socketKernel);
        pthread_create(&atenderKernel, NULL, atender_peticiones_kernel, socketKernel);
    } else {
        log_error(memoria_swapLogger, "Memoria: Error al aceptar conexión: %s", strerror(errno));
    }

    pthread_join(atenderCpu, NULL);
    pthread_join(atenderKernel, NULL);

    liberar_modulo_memoria_swap(memoria_swapLogger, memoria_swapCfg);

    return EXIT_SUCCESS;
}

int aceptar_conexion_memoria(conexion* con){
    int socket = accept(con->socket, &(con->sockAddr), &(con->sockrAddrLen));
    return socket;
}


void atender_peticiones_kernel(int socket_kernel){
    log_info(memoria_swapLogger, "Memoria: entre a recibir kernel");
    while(1){
        op_code opCode;
        uint32_t PID;
        if(recv(socket_kernel, &opCode, sizeof(op_code), MSG_WAITALL)){
            switch(opCode){
                case NEWTABLE:
                    ;
                    if(recv(socket_kernel, &PID, sizeof(uint32_t), MSG_WAITALL)){
                        uint32_t tamanioArchivo;
                        if(recv(socket_kernel, &tamanioArchivo, sizeof(uint32_t), MSG_WAITALL)){
                            generar_archivo(PID,tamanioArchivo);
                            log_info(memoria_swapLogger, "Memoria: Generado archivo del PID: %i", PID);
                        }
                        
                    }
                    t_tablaPrimerNivel* nuevaTablaPrimerNv = crear_tabla_primer_nivel(PID);
                    uint32_t nroTablaPrimerNv = nuevaTablaPrimerNv->nroTabla;
                    crear_lista_marcos_asignados(PID);
                    crear_lista_paginas_suspendidas(PID);

                    if(send(socket_kernel, &nroTablaPrimerNv, sizeof(uint32_t), 0)){
                        log_info(memoria_swapLogger, "Memoria: Envio de numero de tabla correctamente");
                    }
                break;
                case SUSPENSION: //que reciba solo el indice de tabla de 2do nivel
                    ;
                    void* buffer;
                    t_mensaje_tamanio *tamanio_mensaje = malloc(sizeof(t_mensaje_tamanio));
                    if (recibir_tamanio_mensaje(tamanio_mensaje, socket_kernel)){
                        buffer = malloc(tamanio_mensaje->tamanio);
                        log_debug(memoria_swapLogger, "Memoria: Recibi el tamanio: %i", tamanio_mensaje->tamanio);
                        if (recv(socket_kernel, buffer, tamanio_mensaje->tamanio, MSG_WAITALL)) {
                            t_pcb *pcb = recibir_pcb(buffer, tamanio_mensaje->tamanio);
                            log_info(memoria_swapLogger, "Memoria: Recibi el PCB con ID: %i", pcb->id);
                            suspender_proceso(pcb->tablaDePaginas, pcb->id);
                            vaciar_lista_marcos_asignados(pcb->id);
                            free(pcb);
                            uint32_t ok = 1;
                            if(send(socket_kernel,&ok,sizeof(uint32_t),0)==-1){
                                log_error(memoria_swapLogger, "Memoria: No se pudo mandar el OK de suspension a Kernel");
                            }
                        }
                    }
                break;
                case FREEPCB: //sacar
                    ;
                    uint32_t pid;
                    if(recv(socket_kernel, &pid, sizeof(uint32_t), MSG_WAITALL)){
                        //log_info(memoria_swapLogger, "Memoria: Recibi el PID: %i", pid);
                        //leer_de_archivo(pid,);
                    }else{
                        log_info(memoria_swapLogger, "Memoria: Error al recibir indice a liberar");
                    }
                break;
                case FREEPROCESO:
                    ;
                    uint32_t indiceParaFinalizar;
                    if(recv(socket_kernel, &PID, sizeof(uint32_t), MSG_WAITALL)){
                        eliminar_archivo(PID);
                        //remover_tabla_suspendidas(PID);
                    }
                    if(recv(socket_kernel, &indiceParaFinalizar, sizeof(uint32_t), MSG_WAITALL)){
                        log_info(memoria_swapLogger, "Memoria: Recibi el indice de tabla de primer nivel a finalizar: %i", indiceParaFinalizar);
                        liberar_marcos(indiceParaFinalizar);
                    }else{
                        log_error(memoria_swapLogger, "Memoria: Error al recibir indice a finalizar");
                    }
                break;
                default:
                    log_error(memoria_swapLogger, "Memoria: Error al leer el codigo de operacion");
                break; 
            }
        }
    }
}


void recibir_handshake(int socketCPu)
{
    uint32_t handshake;
    void* bytes = malloc(sizeof(uint32_t)*2);
    if(recv(socketCPu, &handshake, sizeof(uint32_t), MSG_WAITALL)){
        if(handshake == 1){
            memcpy(bytes, &(memoria_swapCfg->TAM_PAGINA), sizeof(uint32_t));
            memcpy(bytes + sizeof(uint32_t), &(memoria_swapCfg->PAGINAS_POR_TABLA), sizeof(uint32_t));
            send(socketCPu, bytes, sizeof(uint32_t)*2, 0);
        }
    }
    free(bytes);
}

uint32_t get_siguiente_indice_primer_nivel() 
{
    pthread_mutex_lock(&mutexIndice);
    uint32_t id = nextIndicePrimerNv;
    nextIndicePrimerNv++;
    pthread_mutex_unlock(&mutexIndice);
    return id;
}

uint32_t get_siguiente_indice_segundo_nivel() 
{
    pthread_mutex_lock(&mutexIndice2doNivel);
    uint32_t id = nextIndiceSegundoNv;
    nextIndiceSegundoNv++;
    pthread_mutex_unlock(&mutexIndice2doNivel);
    return id;
}

