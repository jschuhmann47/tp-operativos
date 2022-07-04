#ifndef TABLAS_PAGINAS_H
#define TABLAS_PAGINAS_H

#include <string.h>
#include <stdint.h>
#include <commons/log.h>

#include "memoria_swap_config.h"
#include <commons/collections/list.h>
#include "marcos_libres.h"

t_list* tablasPrimerNivel; //lista de t_tablaPrimerNivel
t_list* tablasSegundoNivel;

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
    uint32_t puntero;
};

typedef struct t_tablaPrimerNivel{
    int nroTabla;
    t_list* entradasPrimerNivel; //lista de t_entradaPrimerNivel
}t_tablaPrimerNivel;

typedef struct t_entradaPrimerNivel t_entradaPrimerNivel;
struct t_entradaPrimerNivel{
    uint32_t indice;
    int indiceTablaSegundoNivel;
};

#include "algoritmos_reemplazo.h"

void inicializar_tabla_paginas();
void reemplazar_pagina(t_marco* paginaAAgregar,t_tablaSegundoNivel* tablaSegundoNivel);
t_tablaPrimerNivel* crear_tabla_primer_nivel();
t_tablaSegundoNivel* crear_tabla_segundo_nivel();
uint32_t agregar_a_tabla_primer_nivel(t_tablaPrimerNivel* tablaPrimerNv, t_tablaSegundoNivel* tablaSegNv);
bool lugar_libre(t_entradaPrimerNivel* filaPrimerNivel);
void procesar_entrada_tabla_primer_nv(int socket_cpu);
void procesar_entrada_tabla_segundo_nv(int socket_cpu);
//void remover_tabla_primer_nivel(uint32_t indice);
uint32_t size_tabla_segundo_nivel(t_tablaSegundoNivel* tablaSegundoNivel);
t_marco* crear_marco();
void liberar_marco(t_marco* marco);
int conseguir_marco_libre(t_tablaSegundoNivel* tablaSegundoNivel, uint32_t indicePagina);


#endif