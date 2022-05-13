#ifndef MODCONSOLA_H
#define MODCONSOLA_H

#include <stdio.h>
#include <stdint.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
//#include <sys/socket.h>
//#include <commons/log.h>

#include "parser.h"
#include "../../shared/include/sockets.h"
#include "../../shared/include/protocolo.h"

typedef struct Nodo{
	char *Codigo;
	int  Parametro1;
	int  Parametro2;
	struct Nodo *sig;
} Node;

typedef struct ListaIdentificar {
	Node *inicio;
	Node *fin;
	int tamanio;
}Lista;

// int Inicializar(Lista*);
// int Agregar(Lista*, Node*, struct Instruccion);
// void Imprimir(Lista*);

void *preparar_paquete(uint32_t cantInstrucciones,t_list* listaInstrucciones,int sizeAMandar);
uint32_t string_to_uint(char* string);
op_code devolverCodigoOperacion(char* palabra);
void* serializar_instruccion(op_code opCode,char** leida);


t_log* consola_logger;


#endif
