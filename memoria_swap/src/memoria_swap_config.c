#include "memoria_swap_config.h"

t_memoria_swap_config *memoria_swap_cfg_create(void)
{
    t_memoria_swap_config *newmemoria_swapCfg = malloc(sizeof(t_memoria_swap_config));
    newmemoria_swapCfg->CPU_SOCKET = 0;
    newmemoria_swapCfg->KERNEL_SOCKET = 0;
    newmemoria_swapCfg->PUERTO_ESCUCHA = NULL;
    newmemoria_swapCfg->IP_MEMORIA = NULL;
    newmemoria_swapCfg->TAM_MEMORIA = 0;
    newmemoria_swapCfg->TAM_PAGINA = 0;
    newmemoria_swapCfg->PAGINAS_POR_TABLA = 0;
    newmemoria_swapCfg->RETARDO_MEMORIA = 0;
    newmemoria_swapCfg->ALGORITMO_REEMPLAZO = NULL;
    newmemoria_swapCfg->MARCOS_POR_PROCESO = 0;
    newmemoria_swapCfg->RETARDO_SWAP = 0;
    newmemoria_swapCfg->PATH_SWAP = NULL;

    return newmemoria_swapCfg;
}

void memoria_swap_config_initialize(void *memoria_swapCfg, t_config *config)
{

    t_memoria_swap_config *cfg = (t_memoria_swap_config *)memoria_swapCfg;
    cfg->PUERTO_ESCUCHA = strdup(config_get_string_value(config, "PUERTO_ESCUCHA"));
    cfg->IP_MEMORIA = strdup(config_get_string_value(config, "IP_MEMORIA"));
    cfg->TAM_MEMORIA = config_get_int_value(config, "TAM_MEMORIA");
    cfg->TAM_PAGINA = config_get_int_value(config, "TAM_PAGINA");
    cfg->PAGINAS_POR_TABLA = config_get_int_value(config, "PAGINAS_POR_TABLA");
    cfg->RETARDO_MEMORIA = config_get_int_value(config, "RETARDO_MEMORIA");
    cfg->ALGORITMO_REEMPLAZO = strdup(config_get_string_value(config, "ALGORITMO_REEMPLAZO"));
    cfg->MARCOS_POR_PROCESO = config_get_int_value(config, "MARCOS_POR_PROCESO");
    cfg->RETARDO_SWAP = config_get_int_value(config, "RETARDO_SWAP");
    cfg->PATH_SWAP = strdup(config_get_string_value(config, "PATH_SWAP"));
}

void liberar_modulo_memoria_swap(t_log *memoria_swapLogger, t_memoria_swap_config *memoria_swapCfg)
{
    log_destroy(memoria_swapLogger);
    close(memoria_swapCfg->CPU_SOCKET);
    close(memoria_swapCfg->KERNEL_SOCKET);
    free(memoria_swapCfg->ALGORITMO_REEMPLAZO);
    free(memoria_swapCfg->PATH_SWAP);
    free(memoria_swapCfg);
}
