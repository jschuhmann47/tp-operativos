#ifndef STATIC_COMMON_UTILS_H_INCLUDED
#define STATIC_COMMON_UTILS_H_INCLUDED

#include <stdlib.h>

#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <string.h>
#include "structs.h"

///////////////////////////// CONFIGS /////////////////////////////
int cargar_configuracion(const char *nombreModulo, void *moduleCfg, char *configPath, t_log *logger,
		void (*cargar_miembros)(void *cfg, t_config *localCfg));
char* agregarInstruccion(char **intruccion, char *mensaje);
t_instruccion *crear_instruccion(void);
int sizeArray(void **array);
code_instruccion getCodeIntruccion(char *code);
#endif
