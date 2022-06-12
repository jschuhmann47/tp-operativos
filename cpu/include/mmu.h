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

uint32_t traducir_direccion(uint32_t direccionLogica, 
                          uint32_t tamanioPagina, 
                          uint32_t paginasPorTabla);

#endif