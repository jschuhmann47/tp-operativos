#ifndef PARSER_H
#define PARSER_H



int instruccion_un_parametro(char* leido);
int instruccion_dos_parametros(char* leido);
int instruccion_cero_parametros(char* leido);
struct Instruccion parser2(FILE* archivo);

#endif
