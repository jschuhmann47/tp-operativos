#include <stdio.h>
#include <stdint.h>
#include <commons/string.h>

typedef enum {
	NO_OP, IO,COPY,READ,WRITE,EXIT
}OPCODE;

struct Instruccion {
	char* codigoInstruccion;
	uint32_t parametros[2];
};
char barraN='\n';


struct Instruccion parser2(FILE* archivo){
	struct Instruccion ins;
	char* buffer=string_new();
	string_append(buffer,fgets(archivo)); //deja de leer en EOF o \n
	if(buffer!=EOF){
	char** separar_instruccion = string_n_split(buffer,2," "); //tengo entendido que en caso de no haber parametros devuelve NULL
		ins.codigoInstruccion=separar_instruccion[0];
		switch(separar_instruccion[0]){
			case("NO_OP"):
				ins.parametros[0]=separar_instruccion[1];
				ins.parametros[1]=NULL;
				break;
			case("I/O"):
				ins.parametros[0]=separar_instruccion[1];
				ins.parametros[1]=NULL;
				break;
			case("COPY"):
				ins.parametros[0]=separar_instruccion[1];
				ins.parametros[1]=separar_instruccion[2];
				break;
			case("READ"):
				ins.parametros[0]=separar_instruccion[1];
				ins.parametros[1]=NULL;
				break;
			case("WRITE"):
				ins.parametros[0]=separar_instruccion[1];
				ins.parametros[1]=separar_instruccion[2];
				break;
			case("EXIT"):
				ins.parametros[0]=NULL;
				ins.parametros[1]=NULL;
				break;
			default:
				printf("Una instruccion es incorrecta\n");
				return -1;

		}
		fclose(archivo);
		free(buffer);
		free(separar_instruccion);
		return ins;
	}
	return NULL;

}
