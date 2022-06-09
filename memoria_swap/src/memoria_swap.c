#include "memoria_swap.h"

int main(int argc, char *argv[])
{
    memoria_swapLogger = log_create(MEMORIA_SWAP_LOG_DEST, MEMORIA_SWAP_MODULE_NAME, true, LOG_LEVEL_INFO);
    memoria_swapCfg = memoria_swap_cfg_create();
    cargar_configuracion(MEMORIA_SWAP_MODULE_NAME, memoria_swapCfg, MEMORIA_SWAP_CFG_PATH, memoria_swapLogger, memoria_swap_config_initialize);

    int socket_servidor = iniciar_servidor(memoria_swapCfg->IP_MEMORIA, memoria_swapCfg->PUERTO_ESCUCHA);
    
    struct sockaddr cliente;
    socklen_t lenCliD = sizeof(cliente);

    conexion* conexionCpu = malloc(sizeof(conexion));

    conexionCpu->socket=socket_servidor;
    conexionCpu->sockAddr=cliente;
    conexionCpu->sockrAddrLen=lenCliD;
    
    pthread_t atenderDispatch;
    pthread_create(&atenderDispatch, NULL, aceptar_conexiones_memoria, conexionCpu);

    pthread_join(atenderDispatch, NULL);

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
    }
}

void recibir_instrucciones_cpu(int socket_memoria)
{
    while(1){
        uint32_t* parametroWrite = malloc(sizeof(uint32_t));
        if(recv(socket_memoria, parametroWrite, sizeof(uint32_t), MSG_WAITALL)){
            log_info(memoria_swapLogger, "Memoria: Recibi parametro: %i", *parametroWrite);
        }
    }
}

void* crear_espacio_de_memoria()
{
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