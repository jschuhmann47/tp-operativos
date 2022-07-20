#ifndef SUSPENSION_H
#define SUSPENSION_H

#include "memoria_swap_config.h"
#include <semaphore.h>

typedef struct t_procesoSuspendido{
    uint32_t pid;
    t_list* paginasSuspendidas;
}t_procesoSuspendido;

typedef struct t_paginaSuspendida{
    uint32_t nroPagina;
    sem_t semaforo;
}t_paginaSuspendida;

t_list* procesosSuspendidos;

void suspender_proceso(uint32_t indice, uint32_t pid);
bool pagina_fue_suspendida(uint32_t pid, uint32_t nroPagina);
t_list* buscar_lista_paginas_suspendidas(uint32_t pid);
void crear_lista_paginas_suspendidas(uint32_t pid);
void agregar_a_lista_paginas_suspendidas(uint32_t pid, uint32_t nroPagina);
void remover_de_lista_paginas_suspendidas(uint32_t pid, uint32_t nroPagina);
void remover_tabla_suspendidas(uint32_t pid);

#endif