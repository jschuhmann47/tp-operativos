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

void aceptar_conexiones_memoria(conexion* conexion) { 
    log_info(memoria_swapLogger, "Memoria: A la escucha de nuevas conexiones en puerto %d", conexion->socket);
    for(;;) {
        memoria_swapCfg->CPU_SOCKET = accept(conexion->socket, &(conexion->sockAddr), &(conexion->sockrAddrLen));
        if(memoria_swapCfg->CPU_SOCKET > 0) {
            log_info(memoria_swapLogger, "Memoria: Acepto la conexión del socket: %d", memoria_swapCfg->CPU_SOCKET);
            recibir_instrucciones_cpu(memoria_swapCfg->CPU_SOCKET);
        } else {
            log_error(memoria_swapLogger, "Memoria: Error al aceptar conexión: %s", strerror(errno));
        }
        memoria_swapCfg->KERNEL_SOCKET = accept(conexion->socket, &(conexion->sockAddr), &(conexion->sockrAddrLen));
        if(memoria_swapCfg->KERNEL_SOCKET > 0) {
            log_info(memoria_swapLogger, "Memoria: Acepto la conexión del socket: %d", memoria_swapCfg->KERNEL_SOCKET);
            recibir_pcbs_kernel(memoria_swapCfg->KERNEL_SOCKET);
        } else {
            log_error(memoria_swapLogger, "Memoria: Error al aceptar conexión: %s", strerror(errno));
        }
    }
}

int aceptar_conexion_memoria(conexion* con){
    int socket = accept(con->socket, &(con->sockAddr), &(con->sockrAddrLen));
    return socket;
}

void recibir_instrucciones_cpu(int socket_cpu){
    log_info(memoria_swapLogger, "Memoria: entre a recibir cpu");
    while(1){
        uint32_t* parametroWrite = malloc(sizeof(uint32_t));
        if(recv(socket_cpu, parametroWrite, sizeof(uint32_t), MSG_WAITALL)){
            log_info(memoria_swapLogger, "Memoria: Recibi parametro: %i", *parametroWrite);
        }
    }
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