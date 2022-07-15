#ifndef ALGORITMOS_REEMPLAZO_H
#define ALGORITMOS_REEMPLAZO_H


#include "memoria_swap_config.h"
#include "tablas_paginas.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <commons/string.h>



t_marco* reemplazo_clock(t_tablaSegundoNivel* tablaSegNv, t_marco* paginaAAgregar,t_marcosAsignadoPorProceso* marcosAsig,int nroPagina,int* paginaVictima);
t_marco* reemplazo_clock_modificado(t_tablaSegundoNivel* tablaSegNv, t_marco* paginaAAgregar,t_marcosAsignadoPorProceso* marcosAsig, int nroPagina, int* paginaVictima);
void actualizar_paginas(t_marcoAsignado** victima,t_marcosAsignadoPorProceso* marcosAsig,t_tablaSegundoNivel* tablaSegNv,t_marco* paginaAAgregar,int nroPagina,int* paginaVictima);

#endif