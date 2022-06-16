#ifndef PROCESAR_INSTRUCCIONES_H 
#define PROCESAR_INSTRUCCIONES_H

#include "memoria_swap_config.h"
#include "common_utils.h"

void recibir_instrucciones_cpu(int socket_cpu);
void procesar_instruccion(void* buffer, int socket_cpu);

void procesar_read(uint32_t param, int socket_cpu);
void procesar_write(uint32_t param1, uint32_t param2, int socket_cpu);

#endif