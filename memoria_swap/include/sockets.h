#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h> //INADDR_ANY e INET_ADDR
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <commons/log.h>
#include <pthread.h>

#define ERROR -1 // Las system-calls de sockets retornan -1 en caso de error
#define DISCONNECTED 0




int crearSocketEscucha (int puerto, t_log* logger);
int crearSocketCliente(char *ipServidor, int puerto, t_log* logger);