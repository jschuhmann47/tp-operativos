#include "memoria_swap.h"

int main(int argc, char *argv[]){

    memoria_swapLogger = log_create(MEMORIA_SWAP_LOG_DEST, MEMORIA_SWAP_MODULE_NAME, true, LOG_LEVEL_INFO);
    memoria_swapCfg = memoria_swap_cfg_create();
    cargar_configuracion(MEMORIA_SWAP_MODULE_NAME, memoria_swapCfg, MEMORIA_SWAP_CFG_PATH, memoria_swapLogger, memoria_swap_config_initialize);

    inicializar_tabla_paginas();
    inicializar_marcos();
    MEMORIA_PRINCIPAL = crear_espacio_de_memoria();
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
    
    //pthread_create(&atenderKernel, NULL, aceptar_conexiones_memoria, conexionCpu);

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
        if(recv(socket_kernel, &opCode, sizeof(op_code), MSG_WAITALL)){
            switch(opCode){
                case NEWTABLE:
                ;
                t_tablaSegundoNivel* tablaSegundoNivel = malloc(sizeof(t_tablaSegundoNivel));
                tablaSegundoNivel->puntero = 0;
                tablaSegundoNivel->marcos = list_create();
                uint32_t indice = agregar_a_tabla_primer_nivel(tablaSegundoNivel);
                if(send(socket_kernel, &indice, sizeof(uint32_t), 0)){
                    log_info(memoria_swapLogger, "Memoria: Envio de posicion de tabla correctamente");
                }
                break;
                case SUSPENSION:
                ;
                void* buffer;
                t_mensaje_tamanio *tamanio_mensaje = malloc(sizeof(t_mensaje_tamanio));
                if (recibir_tamanio_mensaje(tamanio_mensaje, socket_kernel)){
                    buffer = malloc(tamanio_mensaje->tamanio);
                    log_info(memoria_swapLogger, "MEMORIA: Recibi el tamanio: %i", tamanio_mensaje->tamanio);
                    if (recv(socket_kernel, buffer, tamanio_mensaje->tamanio, MSG_WAITALL)) {
                        t_pcb *pcb = recibir_pcb(buffer, tamanio_mensaje->tamanio);
                        log_info(memoria_swapLogger, "MEMORIA: Recibi el PCB con ID: %i", pcb->id);
                        //suspender_pcb();
                        free(pcb);
                    }
                }
                case FREEPCB:
                ;
                uint32_t indiceALiberar;
                if(recv(socket_kernel, &indiceALiberar, sizeof(uint32_t), MSG_WAITALL)){
                    log_info(memoria_swapLogger, "MEMORIA: Recibi el indice a liberar: %i", indiceALiberar);
                    remover_tabla_primer_nivel(indiceALiberar);
                }
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


