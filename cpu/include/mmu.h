#ifndef CPU_MMU_H_INCLUDED
#define CPU_MMU_H_INCLUDED

#include "conexiones.h"
#include "common_utils.h"
#include <unistd.h>
#include <time.h>
#include "cpu_config.h"
#include "cpu.h"
#include "instruccion.h"
#include "structs.h"
#include <math.h>

uint32_t obtener_marco_de_memoria(uint32_t direccionLogica, 
                          uint32_t tamanioPagina, 
                          uint32_t paginasPorTabla,
                          int socket_memoria);

uint32_t obtener_direccion_fisica(uint32_t marco, uint32_t desplazamiento);
uint32_t obtener_desplazamiento(uint32_t direccionLogica, uint32_t numeroDePagina);
uint32_t traducir_direccion_logica(uint32_t direccionLogica, int socket_memoria,t_log* cpuLogger);


#endif