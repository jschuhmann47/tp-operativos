#ifndef TABLAS_PAGINAS_H
#define TABLAS_PAGINAS_H

#include "memoria_swap_config.h"
#include <commons/collections/list.h>

typedef struct{
    uint32_t indice;
    t_segundoNivel* paginaSegNv;
} t_primerNivel;

typedef struct{
    uint32_t indice;
    uint32_t marco;
    bool presencia; //bit de presencia
    bool uso; //bit de uso
    bool modificado; //bit de modificado
} t_segundoNivel;


void inicializar_tabla_paginas();


#endif