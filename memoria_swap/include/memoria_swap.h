#ifndef MEMORIA_SWAP_H_INCLUDED
#define MEMORIA_SWAP_H_INCLUDED

#include <commons/log.h>

#include "memoria_swap_config.h"
#include "common_utils.h"


#define MEMORIA_SWAP_CFG_PATH "cfg/memoria_swap.cfg"
#define MEMORIA_SWAP_LOG_DEST "bin/memoria_swap.log"
#define MEMORIA_SWAP_MODULE_NAME "Memoria_Swap"

t_log* memoria_swapLogger;
t_memoria_swap_config* memoria_swapCfg;
void* MEMORIA_PRINCIPAL; //Tamaño de memoria, especificado en config.
static uint32_t nextIndicePrimerNv; //Auto incremental, para identificar tabla 1er nivel
static uint32_t nextIndiceSegundoNv; //Auto incremental, para identificar tabla 2do nivel
static int marcosPorProceso; //Especificado en config para la asignacion fija de marcos por proceso.

int aceptar_conexion_memoria(conexion* con);
void atender_peticiones_kernel(int socket_kernel);
void recibir_handshake(int socketCPu);
uint32_t get_siguiente_indice_primer_nivel();
uint32_t get_siguiente_indice_segundo_nivel();
void liberar_marcos(uint32_t indice);

#endif
