
#include "include/main.h"


int main(){

	FILE* archivo=fopen("proceso1.txt","r");
	struct Instruccion hola = parser(archivo);
	printf(hola.codigo_instruccion); //esto es a modo de ejemplo
	return 0;

}
