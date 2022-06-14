#include "memoria_swap.h"

int main(int argc, char *argv[]){

    memoria_swapLogger = log_create(MEMORIA_SWAP_LOG_DEST, MEMORIA_SWAP_MODULE_NAME, true, LOG_LEVEL_INFO);
    memoria_swapCfg = memoria_swap_cfg_create();
    cargar_configuracion(MEMORIA_SWAP_MODULE_NAME, memoria_swapCfg, MEMORIA_SWAP_CFG_PATH, memoria_swapLogger, memoria_swap_config_initialize);

    inicializar_tabla_paginas();

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

//de aca para abajo deberian ir en otro archivo

void recibir_instrucciones_cpu(int socket_cpu){
    while(1){
        
        op_code opCode;
        if(recv(socket_cpu, &opCode, sizeof(op_code), MSG_WAITALL) == -1){
            log_error(memoria_swapLogger, "Memoria: Error al recibir opCode de CPU: %s", strerror(errno));
            break;
        }
        
        switch(opCode){
            case INSTRUCCION:
                ;
                uint32_t size;
                if(recv(socket_cpu, &size, sizeof(uint32_t), MSG_WAITALL) == -1){
                    log_error(memoria_swapLogger, "Memoria: Error al recibir size de CPU: %s", strerror(errno));
                    break;
                }
                void* buffer=malloc(size);
                log_info(memoria_swapLogger, "Memoria: Esperando instruccion de CPU");
                if(recv(socket_cpu, buffer, size, MSG_WAITALL)){
                    procesar_instruccion(buffer,socket_cpu);
                }
                free(buffer);
                continue;
            case TABLAUNO:
                procesar_entrada_tabla_primer_nv(socket_cpu);
                continue;
            case TABLADOS:
                procesar_entrada_tabla_segundo_nv(socket_cpu);
                continue;
        }

        
    }
}

void procesar_instruccion(void* buffer, int socket_cpu){
    code_instruccion codOp;
    memcpy(&codOp, buffer, sizeof(code_instruccion));
    uint32_t param1;
    uint32_t param2;
    log_info(memoria_swapLogger, "Memoria: Recibi el codigo de operacion: %i", codOp);
    switch (codOp)
    {
    case READ:
        memcpy(&param1, buffer+sizeof(code_instruccion), sizeof(uint32_t));
        log_info(memoria_swapLogger, "Memoria: Recibi READ con parametro: %i", param1);
        procesar_read(param1, socket_cpu); //TODO
        break;
    case WRITE:
        memcpy(&param1, buffer+sizeof(code_instruccion), sizeof(uint32_t));
        memcpy(&param2, buffer+sizeof(code_instruccion)+sizeof(uint32_t), sizeof(uint32_t));
        log_info(memoria_swapLogger, "Memoria: Recibi WRITE con parametros: %i, %i", param1, param2);
        procesar_write(param1, param2, socket_cpu);//TODO
        break;
    default:
        log_error(memoria_swapLogger, "Memoria: Error al leer el codigo de operacion");
        break;
    }
    //devolver a cpu un ok o ver que devuelve en cada caso
}

void procesar_read(uint32_t param, int socket_cpu){ //READ devuelve el valor leido
    log_info(memoria_swapLogger, "Memoria: Procesando READ");
    //...
    log_info(memoria_swapLogger, "Memoria: READ terminado");
}

void procesar_write(uint32_t param1, uint32_t param2, int socket_cpu){ //write no dice, devolver ok o error?
    log_info(memoria_swapLogger, "Memoria: Procesando WRITE");
    //...
    log_info(memoria_swapLogger, "Memoria: WRITE terminado");
}

void procesar_entrada_tabla_primer_nv(int socket_cpu){
    
    log_info(memoria_swapLogger, "Memoria: Procesando entrada de tabla de primer nivel");
    uint32_t requestPrimerTabla;
    if(recv(socket_cpu, &requestPrimerTabla, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir requestPrimerTabla de CPU: %s", strerror(errno));
        //return;
    }
    
    //va a la tabla y trae el numero de la segunda tabla

    uint32_t indiceSegundaTabla = 1;
    if(send(socket_cpu,&indiceSegundaTabla,sizeof(uint32_t),0) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al enviar indiceSegundaTabla a CPU: %s", strerror(errno));
        //return;
    }
}

void procesar_entrada_tabla_segundo_nv(int socket_cpu){
    log_info(memoria_swapLogger, "Memoria: Procesando entrada de tabla de segundo nivel");
    uint32_t requestSegundaTabla;
    if(recv(socket_cpu, &requestSegundaTabla, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir requestSegundaTabla de CPU: %s", strerror(errno));
        //return;
    }

    //busca el marco en la tabla de segunda nivel

    uint32_t marco = 1;
    if(send(socket_cpu,&marco,sizeof(uint32_t),0) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al enviar marco a CPU: %s", strerror(errno));
        //return;
    }

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
            }
        }
    }
}

void* crear_espacio_de_memoria()
{
    return malloc(memoria_swapCfg->TAM_MEMORIA);
}

void escribir_en_memoria(void* memoria, void* contenido, uint32_t marco, uint32_t desplazamiento,int size){
    uint32_t offset = marco * memoria_swapCfg->TAM_PAGINA + desplazamiento;
    memcpy(memoria + offset, contenido, size);
}

void* leer_de_memoria(void* memoria, int offset, int size)
{ //no valida nada
    void* contenido = malloc(size); //con marco
    memcpy(contenido, memoria + offset, size);
    return contenido;
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

uint32_t agregar_a_tabla_primer_nivel(t_tablaSegundoNivel* tablaSegundoNivel)
{
    t_primerNivel* primerNivel;
    primerNivel = list_find(tablaPaginasPrimerNivel, lugar_libre);
    primerNivel->tablaSegundoNivel = tablaSegundoNivel;
    return primerNivel->indice;
}

bool lugar_libre(t_primerNivel* filaPrimerNivel)
{
    return filaPrimerNivel->tablaSegundoNivel == NULL;
}