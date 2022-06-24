#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <commons/string.h>
#include "memoria_swap_config.h"


void generar_archivo(uint32_t pid);
void eliminar_archivo(uint32_t pid);
void escribir_en_archivo(uint32_t pid, int nroMarco);
void* leer_de_archivo(uint32_t pid,int nroMarco);
char* obtener_path_archivo(uint32_t pid);

#endif