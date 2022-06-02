#ifndef STATIC_CONEXIONES_H_INCLUDED
#define STATIC_CONEXIONES_H_INCLUDED

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


extern const char* INT_KEYWORD; //podria ser un 1 mas facil xq no tenes q sacar el tamaño


int iniciar_servidor(char* ip, char* port);
int conectar_a_servidor(char* ip, char* port);

#endif