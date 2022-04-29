#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

int instruccion_un_parametro(char* leido);
int instruccion_dos_parametros(char* leido);
int instruccion_cero_parametros(char* leido);
struct Instruccion parser(FILE* archivo);

#endif
