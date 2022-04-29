#ifndef SOCKETS_H
#define SOCKETS_H

#include <commons/log.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>

// INICIA SERVER ESCUCHANDO EN IP:PUERTO
int iniciar_servidor(t_log* logger, const char* name, char* ip, char* puerto);


// ESPERAR CONEXION DE CLIENTE EN UN SERVER ABIERTO
int esperar_cliente(t_log* logger, const char* name, int socket_servidor);

// CLIENTE SE INTENTA CONECTAR A SERVER ESCUCHANDO EN IP:PUERTO
int crear_conexion_a_server(t_log* logger, const char* server_name, char* ip, char* puerto);

// CERRAR CONEXION
void liberar_conexion(int* socket_cliente);

#endif
