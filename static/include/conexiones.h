#ifndef STATIC_CONEXIONES_H_INCLUDED
#define STATIC_CONEXIONES_H_INCLUDED

#define INT_KEYWORD "interrupcion" //podria ser un 1 mas facil xq no tenes q sacar el tamaño
const uint32_t PCB_IO_RETURN = 1 //estos nros los pueden cambiar
const uint32_t PCB_NORMAL_RETURN = 0


#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int iniciar_servidor(char* ip, char* port);
int conectar_a_servidor(char* ip, char* port);

#endif