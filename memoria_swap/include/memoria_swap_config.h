#ifndef MEMORIA_SWAP_CONFIG_H_INCLUDED
#define MEMORIA_SWAP_CONFIG_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include "tablas_paginas.h"

#include "procesar_instrucciones.h"
#include "manejo_memoria.h"

extern void* MEMORIA_PRINCIPAL;

typedef struct
{   
    int KERNEL_SOCKET;
    uint32_t CPU_SOCKET;
    char* IP_MEMORIA;
    char* PUERTO_ESCUCHA;
    uint32_t TAM_MEMORIA;
    uint32_t TAM_PAGINA;
    uint32_t PAGINAS_POR_TABLA;
    uint32_t RETARDO_MEMORIA;
    char *ALGORITMO_REEMPLAZO;
    uint32_t MARCOS_POR_PROCESO;
    uint32_t RETARDO_SWAP;
    char *PATH_SWAP;
} t_memoria_swap_config;

extern t_log *memoria_swapLogger;
extern t_memoria_swap_config *memoria_swapCfg;

t_memoria_swap_config *memoria_swap_cfg_create(void);
void memoria_swap_config_initialize(void *memoria_swapCfg, t_config *config);
void liberar_modulo_memoria_swap(t_log *memoria_swapLogger, t_memoria_swap_config *memoria_swapCfg);

#endif
