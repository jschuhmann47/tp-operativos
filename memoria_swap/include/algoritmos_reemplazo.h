#ifndef ALGORITMOS_REEMPLAZO_H
#define ALGORITMOS_REEMPLAZO_H


#include "memoria_swap_config.h"
#include "tablas_paginas.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>



t_marco* reemplazo_clock(t_tablaSegundoNivel* tablaSegNv, t_marco* paginaAAgregar,t_marcoAsignado* marcosAsig);
t_marco* reemplazo_clock_modificado(t_tablaSegundoNivel* tablaSegNv, t_marco* paginaAAgregar,t_marcoAsignado* marcosAsig);

#endif