#ifndef MANEJO_MEMORIA_H
#define MANEJO_MEMORIA_H

#include "memoria_swap_config.h"
#include "common_utils.h"

#include <commons/collections/list.h>

typedef struct
{
    uint32_t nroMarco;
    bool estaLibre;
} t_marco_libre;

void* crear_espacio_de_memoria();
void escribir_en_memoria(void* memoria, void* contenido, uint32_t marco, uint32_t desplazamiento,int size);
void* leer_de_memoria(void* memoria, uint32_t marco, uint32_t desplazamiento, int size);
void inicializar_marcos();
bool marco_libre(uint32_t marco);
void marcar_marco_ocupado(uint32_t marco);
t_marco_libre* encontrar_marco(uint32_t marcoABuscar);


#endif