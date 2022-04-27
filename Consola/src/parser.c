#include "../include/parser.h"

#define END_OF_FILE = 100

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
	fgets(buffer,32,&archivo); //deja de leer en EOF o \n. este 32 es max caracteres,
	if(strcmp(buffer,EOF)!=0){
	char** separar_instruccion = string_n_split(buffer,2," "); //tengo entendido que en caso de no haber parametros devuelve NULL
		if(instruccion_un_parametro(separar_instruccion[0])){
			strcpy(ins.codigoInstruccion,separar_instruccion[0]);
			ins.parametros[0]=separar_instruccion[1];
			ins.parametros[1]=NULL;
		}else{

			if(instruccion_dos_parametros(separar_instruccion[0])){
				strcpy(ins.codigoInstruccion,separar_instruccion[0]);
				ins.parametros[0]=separar_instruccion[1];
				ins.parametros[1]=separar_instruccion[2];
		}else{
				if(instruccion_cero_parametros(separar_instruccion[0])){
					strcpy(ins.codigoInstruccion,separar_instruccion[0]);
					ins.parametros[0]=NULL;
					ins.parametros[1]=NULL;
				}
				else{
					ins.codigoInstruccion="ERROR";
					ins.parametros[0]=NULL;
					ins.parametros[1]=NULL;
				}
			}
		}
		fclose(archivo);
		free(buffer);
		free(separar_instruccion);
		return ins;
	}else{
		ins.codigoInstruccion="EOF";
		ins.parametros[0]=NULL;
		ins.parametros[1]=NULL;
		fclose(archivo);
		free(buffer);
		return ins;
	}
}


int instruccion_un_parametro(char* leido){
	return (strcmp(leido,"NO_OP")==0 || strcmp(leido,"I/O")==0 || strcmp(leido,"READ")==0);

}

int instruccion_dos_parametros(char* leido){
	return (strcmp(leido,"COPY")==0 || strcmp(leido,"WRITE")==0 || strcmp(leido,"READ")==0);

}

int instruccion_cero_parametros(char* leido){
	return strcmp(leido,"EXIT")==0;

}



//		switch(separar_instruccion[0]){
//			case(NO_OP):
//				ins.parametros[0]=separar_instruccion[1];
//				ins.parametros[1]=NULL;
//				break;
//			case(IO):
//				ins.parametros[0]=separar_instruccion[1];
//				ins.parametros[1]=NULL;
//				break;
//			case(COPY):
//				ins.parametros[0]=separar_instruccion[1];
//				ins.parametros[1]=separar_instruccion[2];
//				break;
//			case(READ):
//				ins.parametros[0]=separar_instruccion[1];
//				ins.parametros[1]=NULL;
//				break;
//			case(WRITE):
//				ins.parametros[0]=separar_instruccion[1];
//				ins.parametros[1]=separar_instruccion[2];
//				break;
//			case(EXIT):
//				ins.parametros[0]=NULL;
//				ins.parametros[1]=NULL;
//				break;
//			default:
//				printf("Una instruccion es incorrecta\n");
//				return -1;
