#ifndef TABLAS_PAGINAS_H
#define TABLAS_PAGINAS_H

#include <string.h>
#include <stdint.h>
#include <commons/log.h>

#include "memoria_swap_config.h"
#include <commons/collections/list.h>

t_list* tablaPaginasPrimerNivel;

typedef struct {
    uint32_t indice;
    uint32_t marco;
    bool presencia; //bit de presencia
    bool uso; //bit de uso
    bool modificado; //bit de modificado
} t_segundoNivel;

typedef struct t_tablaSegundoNivel t_tablaSegundoNivel;
struct t_tablaSegundoNivel{
    t_list* marcos; //lista de t_segundoNivel
    //pthread_mutex_t mutex;
    uint32_t puntero;
};

typedef struct t_primerNivel t_primerNivel;
struct t_primerNivel{
    uint32_t indice;
    t_tablaSegundoNivel* tablaSegundoNivel;
};



void inicializar_tabla_paginas();
void reemplazar_pagina(t_segundoNivel* paginaAAgregar,t_tablaSegundoNivel* tablaSegundoNivel);
uint32_t agregar_a_tabla_primer_nivel(t_tablaSegundoNivel* tablaSegundoNivel);
bool lugar_libre(t_primerNivel* filaPrimerNivel);
void procesar_entrada_tabla_primer_nv(int socket_cpu);
void procesar_entrada_tabla_segundo_nv(int socket_cpu);


#endif