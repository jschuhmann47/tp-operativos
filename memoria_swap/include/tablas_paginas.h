#ifndef TABLAS_PAGINAS_H
#define TABLAS_PAGINAS_H

#include <string.h>
#include <stdint.h>
#include <commons/log.h>

#include "memoria_swap_config.h"
#include <commons/collections/list.h>
#include "marcos_libres.h"
#include "archivos.h"

t_list* tablasPrimerNivel; //lista de t_tablaPrimerNivel
t_list* tablasSegundoNivel; //lista de t_tablaSegundoNivel

t_list* marcosAsignadosPorProceso; //lista de t_marcosAsignadosPorProceso

typedef struct {
    int marco;
    bool presencia;
    bool uso;
    bool modificado;
} t_marco;

typedef struct t_tablaSegundoNivel t_tablaSegundoNivel;
struct t_tablaSegundoNivel{
    int indice;
    t_list* marcos; //lista de t_marco
    uint32_t pid;
};

typedef struct t_tablaPrimerNivel{
    int nroTabla;
    t_list* entradasPrimerNivel; //lista de t_entradaPrimerNivel
    uint32_t pid;
}t_tablaPrimerNivel;

typedef struct t_entradaPrimerNivel t_entradaPrimerNivel;
struct t_entradaPrimerNivel{
    uint32_t indice;
    int indiceTablaSegundoNivel;
};

typedef struct t_marcosAsignadoPorProceso{ //Especifica que marcos estan asignados al proceso en cuestion
    uint32_t pid;
    t_list* marcosAsignados; //lista de t_marcoAsignado
    int puntero; //puntero para realizar reemplazo por clock y clock-m
}t_marcosAsignadoPorProceso;

typedef struct t_marcoAsignado{
    t_marco* marco;
    int nroTablaSegundoNivel;
}t_marcoAsignado;

#include "algoritmos_reemplazo.h"

void inicializar_tabla_paginas();
t_tablaPrimerNivel* crear_tabla_primer_nivel(uint32_t pid);
t_tablaSegundoNivel* crear_tabla_segundo_nivel(uint32_t pid);
uint32_t agregar_a_tabla_primer_nivel(t_tablaPrimerNivel* tablaPrimerNv, t_tablaSegundoNivel* tablaSegNv);
bool lugar_libre(t_entradaPrimerNivel* filaPrimerNivel);

t_marco* crear_marco();
void liberar_marco(t_marco* marco);
void liberar_marcos(uint32_t indice);
void free_lista_marco(t_marco* m);

uint32_t get_siguiente_indice_segundo_nivel();
uint32_t get_siguiente_indice_primer_nivel();



#endif