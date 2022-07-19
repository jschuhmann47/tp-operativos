#ifndef ARCHIVOS_H
#define ARCHIVOS_H

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <commons/string.h>
#include "memoria_swap_config.h"

void generar_archivo(uint32_t pid, uint32_t tamanioArchivo);
void eliminar_archivo(uint32_t pid);
void escribir_en_archivo(uint32_t pid, int nroMarco, int nroPagina);
void* leer_de_archivo(uint32_t pid,int nroMarco);
char* obtener_path_archivo(uint32_t pid);

#endif