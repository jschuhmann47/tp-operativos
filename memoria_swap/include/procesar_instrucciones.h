#ifndef PROCESAR_INSTRUCCIONES_H 
#define PROCESAR_INSTRUCCIONES_H

#include "memoria_swap_config.h"
#include "manejo_memoria.h"
#include "common_utils.h"
#include "archivos.h"

void recibir_instrucciones_cpu(int socket_cpu);
void procesar_instruccion(void* buffer, int socket_cpu);
uint32_t procesar_read(uint32_t direccionFisica);
void procesar_write(uint32_t direccionFisica, uint32_t valor);
void actualizar_bit_de_marco(int socket_cpu, uint32_t direccionFisica, op_code codOp);
void procesar_entrada_tabla_segundo_nv(int socket_cpu);
void procesar_entrada_tabla_primer_nv(int socket_cpu);


#endif