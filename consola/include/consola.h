#ifndef CONSOLA_H_INCLUDED
#define CONSOLA_H_INCLUDED

#include <commons/log.h>

#include "consola_config.h"
#include "common_utils.h"
#include "conexiones.h"

#define CONSOLA_CFG_PATH "cfg/consola.cfg"
#define CONSOLA_LOG_DEST "bin/consola.log"
#define CONSOLA_MODULE_NAME "Consola"

t_log* consolaLogger;
t_consola_config* consolaCfg;
char* leer_archivo(char *nombre);
t_list* tomar_parametros(char** token);

#endif