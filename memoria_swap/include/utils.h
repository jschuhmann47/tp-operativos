#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <commons/log.h>

#include "memoria_swap_config.h"

#define MEMORIA_SWAP_CFG_PATH "cfg/memoria_swap.cfg"
#define MEMORIA_SWAP_LOG_DEST "bin/memoria_swap.log"
#define MEMORIA_SWAP_MODULE_NAME "Memoria_Swap"

extern t_log *memoria_swapLogger;
extern t_memoria_swap_config *memoria_swapCfg;
extern void *MEMORIA_PRINCIPAL;

void *inicializar_memoria(t_memoria_swap_config *memoria_swap_config);

#endif
