#include "../include/parser.h"


char barraN='\n';


char** parser(FILE* archivo){
	char* buffer=string_new();
	fgets(buffer,32,&archivo); //deja de leer en EOF o \n. este 32 es max caracteres,

	return string_n_split(buffer,1," "); //tengo entendido que en caso de no haber parametros devuelve NULL
	//["WRITE","4", "30"]
}

//
//int instruccion_un_parametro(char* leido){
//	return (strcmp(leido,"NO_OP")==0 || strcmp(leido,"I/O")==0 || strcmp(leido,"READ")==0);
//
//}
//
//int instruccion_dos_parametros(char* leido){
//	return (strcmp(leido,"COPY")==0 || strcmp(leido,"WRITE")==0 || strcmp(leido,"READ")==0);
//
//}
//
//int instruccion_cero_parametros(char* leido){
//	return strcmp(leido,"EXIT")==0;
//
//}

