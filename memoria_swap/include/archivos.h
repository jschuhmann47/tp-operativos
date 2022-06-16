#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <commons/string.h>
#include "memoria_swap.h"

void generar_archivo(uint32_t pid, char *contenido);
void eliminar_archivo(uint32_t pid);

#endif