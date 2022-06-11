#include "memoria_swap.h"

int main(int argc, char *argv[]){

    memoria_swapLogger = log_create(MEMORIA_SWAP_LOG_DEST, MEMORIA_SWAP_MODULE_NAME, true, LOG_LEVEL_INFO);
    memoria_swapCfg = memoria_swap_cfg_create();
    cargar_configuracion(MEMORIA_SWAP_MODULE_NAME, memoria_swapCfg, MEMORIA_SWAP_CFG_PATH, memoria_swapLogger, memoria_swap_config_initialize);

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
        pthread_create(&atenderCpu, NULL, recibir_instrucciones_cpu, socketCpu);
    } else {
        log_error(memoria_swapLogger, "Memoria: Error al aceptar conexión: %s", strerror(errno));
    }
    

    int socketKernel = aceptar_conexion_memoria(conexionKernel);
    if(socketKernel > 0) {
        log_info(memoria_swapLogger, "Memoria: Acepto la conexión del Kernel con socket: %d", socketKernel);
        pthread_create(&atenderKernel, NULL, recibir_pcbs_kernel, socketKernel);
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
        uint32_t size;
        if(recv(socket_cpu, &size, sizeof(uint32_t), MSG_WAITALL) == -1){
            log_error(memoria_swapLogger, "Memoria: Error al recibir el tamaño de la instrucción: %s", strerror(errno));
            break;
        }
        log_info(memoria_swapLogger, "Memoria: Recibiendo instrucción de %i bytes", size);
        void* buffer=malloc(size);
        log_info(memoria_swapLogger, "Memoria: Esperando instruccion de CPU");
    
        if(recv(socket_cpu, buffer, size, MSG_WAITALL)){
            procesar_instruccion(buffer,socket_cpu);

            // log_info(memoria_swapLogger, "Memoria: Recibi parametro: %i", *parametroWrite);
        }
        free(buffer);
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

void recibir_pcbs_kernel(int socket_kernel){
    log_info(memoria_swapLogger, "Memoria: entre a recibir kernel");
    while(1){
        //recibir la pcb y tamaño

        //suspenderla o hacer lo que haya que hacer
    }
}

void* crear_espacio_de_memoria(){
    return malloc(memoria_swapCfg->TAM_MEMORIA);
}

void escribir_en_memoria(void* memoria, void* contenido, int offset, int size){ //esta funcion no valida si se pueda escribir
    memcpy(memoria + offset, contenido, size);
}

void* leer_de_memoria(void* memoria, int offset, int size){ //no valida nada
    void* contenido = malloc(size);
    memcpy(contenido, memoria + offset, size);
    return contenido;
}