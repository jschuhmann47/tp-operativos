#ifndef PROCESAR_INSTRUCCIONES_H 
#define PROCESAR_INSTRUCCIONES_H

#include "memoria_swap_config.h"
#include "manejo_memoria.h"
#include "common_utils.h"

void recibir_instrucciones_cpu(int socket_cpu);
void procesar_instruccion(void* buffer, int socket_cpu);

uint32_t procesar_read(uint32_t direccionFisica, int socket_cpu);
void procesar_write(uint32_t direccionFisica, uint32_t valor, int socket_cpu);

#endif