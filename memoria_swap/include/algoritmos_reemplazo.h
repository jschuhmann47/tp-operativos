#ifndef ALGORITMOS_REEMPLAZO_H
#define ALGORITMOS_REEMPLAZO_H

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <commons/string.h>
#include "memoria_swap_config.h"
#include "tablas_paginas.h"

void reemplazo_clock(t_segundoNivel* paginaAAgregar,t_tablaSegundoNivel* tablaSegundoNivel);
void reemplazo_clock_modificado(t_segundoNivel* paginaAAgregar,t_tablaSegundoNivel* tablaSegundoNivel);

#endif