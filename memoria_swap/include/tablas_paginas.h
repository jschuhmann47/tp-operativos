#ifndef TABLAS_PAGINAS_H
#define TABLAS_PAGINAS_H

#include <string.h>
#include <stdint.h>
#include <commons/log.h>

#include "memoria_swap_config.h"
#include <commons/collections/list.h>
#include "marcos_libres.h"

t_list* tablaPrimerNivel;
t_list* tablasSegundoNivel;

typedef struct {
    int marco;
    bool presencia; //bit de presencia
    bool uso; //bit de uso
    bool modificado; //bit de modificado
} t_marco;

typedef struct t_tablaSegundoNivel t_tablaSegundoNivel;
struct t_tablaSegundoNivel{
    int indice;
    t_list* marcos; //lista de t_marco
    //pthread_mutex_t mutex;
    uint32_t puntero;
};

typedef struct t_primerNivel t_primerNivel;
struct t_primerNivel{
    uint32_t indice;
    int indiceTablaSegundoNivel; //creo que esto hay que cambiarlo a un indice en la lista de tablasSegundoNivel
};

#include "algoritmos_reemplazo.h"

void inicializar_tabla_paginas();
void reemplazar_pagina(t_marco* paginaAAgregar,t_tablaSegundoNivel* tablaSegundoNivel);
uint32_t agregar_a_tabla_primer_nivel(t_tablaSegundoNivel* tablaSegundoNivel);
bool lugar_libre(t_primerNivel* filaPrimerNivel);
void procesar_entrada_tabla_primer_nv(int socket_cpu);
void procesar_entrada_tabla_segundo_nv(int socket_cpu);
void remover_tabla_primer_nivel(uint32_t indice);
uint32_t size_tabla_segundo_nivel(t_tablaSegundoNivel* tablaSegundoNivel);
t_marco* crear_marco();
void liberar_marco(t_marco* marco);
int conseguir_marco_libre(t_tablaSegundoNivel* tablaSegundoNivel, uint32_t indicePagina);


#endif