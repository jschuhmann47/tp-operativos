#include "utils.h"

void *inicializar_memoria(t_memoria_swap_config *memoria_swap_config)
{


    MEMORIA_PRINCIPAL = malloc(memoria_swap_config->TAM_MEMORIA);

    log_info(memoria_swapLogger, "Se inicializo la memoria principal con un tamaño de %d bytes \n", memoria_swap_config->TAM_MEMORIA);

}