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
void* MEMORIA_PRINCIPAL;

void* crear_espacio_de_memoria();
void escribir_en_memoria(void* memoria, void* contenido, uint32_t marco, uint32_t desplazamiento,int size);
void* leer_de_memoria(void* memoria, int offset, int size);


int aceptar_conexion_memoria(conexion* con);
void recibir_pcbs_kernel(int socket_kernel);
void recibir_instrucciones_cpu(int socket_cpu);
void procesar_instruccion(void* buffer, int socket_cpu);
void procesar_read(uint32_t param, int socket_cpu);
void procesar_write(uint32_t param1, uint32_t param2, int socket_cpu);
void recibir_handshake(int socketCPu);


#endif
