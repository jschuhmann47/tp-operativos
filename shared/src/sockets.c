#include "../include/sockets.h"


// INICIA SERVER ESCUCHANDO EN IP:PUERTO
int iniciar_servidor(t_log* logger, const char* name, char* ip, char* puerto) {
		int socket_servidor;
	    struct addrinfo hints, *servinfo;
	    int esta_conectado=false;

	    memset(&hints, 0, sizeof(hints)); //seteos basicos
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_STREAM;
	    hints.ai_flags = AI_PASSIVE;

	    getaddrinfo(ip, puerto, &hints, &servinfo); //trae la info del servidor

	    // Itera por cada addrinfo devuelto
	       for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next) {
	           socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
	           if (socket_servidor == -1) // si socket devuelve -1 es que fallo
	               continue;

	           if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
				   close(socket_servidor);// si bind devuelve -1 es que fallo
	               continue;
	           }
	           esta_conectado = true;
	           break;
	       }

	       if(!esta_conectado) {
	           free(servinfo);
	           return 0;
	       }

	       listen(socket_servidor, SOMAXCONN); // Escuchando (hasta SOMAXCONN conexiones simultaneas)


	       log_info(logger, "Escuchando en %s:%s (%s)\n", ip, puerto, name);

	       freeaddrinfo(servinfo);

	       return socket_servidor;

}


// ESPERAR CONEXION DE CLIENTE EN UN SERVER ABIERTO
int esperar_cliente(t_log* logger, const char* name, int socket_servidor) {
	struct sockaddr_in direccion_cliente; //their_addr
	socklen_t tamanio_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &direccion_cliente, &tamanio_direccion);

	log_info(logger, "Cliente conectado (a %s)\n", name);

	return socket_cliente;
}

// CLIENTE SE INTENTA CONECTAR A SERVER ESCUCHANDO EN IP:PUERTO
int crear_conexion_a_server(t_log* logger, const char* server_name, char* ip, char* puerto) {
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;


	getaddrinfo(ip, puerto, &hints, &servinfo);

	int socket_cliente = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	if(socket_cliente == -1) {
		log_error(logger, "ERROR: No se pudo crear el socket para %s:%s", ip, puerto);
		return 0;
	}

	if(connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		log_error(logger, "ERROR: No se pudo conectar (a %s)\n", server_name);
		freeaddrinfo(servinfo);
		return 0;
	} else
		log_info(logger, "Cliente conectado en %s:%s (a %s)\n", ip, puerto, server_name);

	freeaddrinfo(servinfo);

	return socket_cliente;
}

// CERRAR CONEXION
void liberar_conexion(int* socket_cliente) {
	close(*socket_cliente);
	*socket_cliente = -1; //por las dudas
}
