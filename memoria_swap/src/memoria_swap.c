#include "memoria_swap.h"

int main(int argc, char *argv[])
{

    memoria_swapLogger = log_create(MEMORIA_SWAP_LOG_DEST, MEMORIA_SWAP_MODULE_NAME, true, LOG_LEVEL_INFO);
    memoria_swapCfg = memoria_swap_cfg_create();
    cargar_configuracion(MEMORIA_SWAP_MODULE_NAME, memoria_swapCfg, MEMORIA_SWAP_CFG_PATH, memoria_swapLogger, memoria_swap_config_initialize);

    inicializar_memoria(memoria_swapCfg);

    //iniciar_servidor

    int socket_servidor = iniciar_servidor(memoria_swapCfg->IP_ESCUCHA, memoria_swapCfg->PUERTO_ESCUCHA);
    if (socket_servidor > 0)
    {
        int cliente = 0;
        while ((cliente = aceptarClienteMemoria(socket_servidor)) > 0)
        {
            pthread_t hilo;
            /*
            pthread_create(&hilo, NULL, (void *)suscribirCliente, (void *)cliente); // revisar est asignacion
            pthread_detach(hilo);
        */
        }
    }
    else
    {
        log_error(memoria_swapLogger, "Error al crear el socket de escucha");
        exit(1);
    }

    liberar_modulo_memoria_swap(memoria_swapLogger, memoria_swapCfg);

    return EXIT_SUCCESS;
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