#ifndef PROCESAR_PEDIDOS_TABLAS_H
#define PROCESAR_PEDIDOS_TABLAS_H

#include "tablas_paginas.h"

void procesar_entrada_tabla_primer_nv(int socket_cpu);
void procesar_entrada_tabla_segundo_nv(int socket_cpu);
t_marcosAsignadoPorProceso* buscar_marcos_asignados_al_proceso(uint32_t pid);
int conseguir_marco_libre(t_tablaSegundoNivel* tablaSegundoNivel, uint32_t indicePagina);
void cargar_pagina_en_memoria(t_tablaSegundoNivel* tablaSegundoNivel, uint32_t nroPagina, uint32_t nroMarco);
int reemplazar_pagina(t_tablaSegundoNivel* tablaSegundoNivel, t_marcosAsignadoPorProceso* marcosAsig, int nroPagina);
void agregar_a_marcos_asignados(t_marcosAsignadoPorProceso* marcosAsig,int nroMarco, int indiceTablaSegNv);
void vaciar_lista_marcos_asignados(uint32_t pid);
void free_marco_asignado(t_marcoAsignado* mA);



#endif