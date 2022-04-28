#ifndef MODCONSOLA_H
#define MODCONSOLA_H

#include <stdio.h>
#include <stdint.h>
#include <commons/string.h>
#include <commons/config.h>
#include <sys/socket.h>
#include <commons/log.h>

#include "parser.h"


typedef enum {
	NO_OP, IO,COPY,READ,WRITE,EXIT
}OPCODE;

struct Instruccion {
	char* codigo_instruccion;
	uint32_t parametros[2];
};


#endif
