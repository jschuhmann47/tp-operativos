#include "memoria_swap.h"

int main(int argc, char *argv[])
{

    memoria_swapLogger = log_create(MEMORIA_SWAP_LOG_DEST, MEMORIA_SWAP_MODULE_NAME, true, LOG_LEVEL_INFO);
    memoria_swapCfg = memoria_swap_cfg_create();
    cargar_configuracion(MEMORIA_SWAP_MODULE_NAME, memoria_swapCfg, MEMORIA_SWAP_CFG_PATH, memoria_swapLogger, memoria_swap_config_initialize);

    inicializar_memoria(memoria_swapCfg);

    //iniciar_servidor

    int socket_servidor = crearSocketEscucha(memoria_swapCfg->PUERTO_ESCUCHA, memoria_swapLogger);
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
