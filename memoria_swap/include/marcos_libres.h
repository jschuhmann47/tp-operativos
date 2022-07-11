#ifndef MARCOS_LIBRES_H_INCLUDED
#define MARCOS_LIBRES_H_INCLUDED

#include "memoria_swap_config.h"


typedef struct
{
    uint32_t nroMarco;
    bool estaLibre;
} t_marco_libre;


t_list* marcosLibres;

void inicializar_lista_marcos_libres();
void crear_marco_libre(uint32_t nroMarco);
bool marco_esta_libre(uint32_t marco);
void marcar_marco_ocupado(uint32_t marco);
void marcar_marco_libre(uint32_t marco);
t_marco_libre* encontrar_marco_en_lista_libre(uint32_t marcoABuscar);
int encontrar_marco_libre();
void crear_lista_marcos_asignados(uint32_t pid);


#endif