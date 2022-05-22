#include "sockets.h" //Header de sockets
  
int crearSocket(){
   int fileDescriptor = socket(AF_INET, SOCK_STREAM, 0);//usa protocolo TCP/IP
	if (fileDescriptor == ERROR) {
		perror("No se pudo crear el file descriptor.\n");
	}

	//Hay que mejorarlo para el caso en el que escucha muchas conexiones

	return fileDescriptor;
}


//Funciones del SERVIDOR 
/* Creamos un Socket Servidor de escucha.
 * Retorna ERROR si el puerto ya estaba en uso, si el bind no funcionó
 *  o si el Listen no funcionó.
 */
int crearSocketServidor(int puerto, t_log* logger)	{
    struct sockaddr_in miDireccionServidor;
    int socketDeEscucha = crearSocket(logger);

    miDireccionServidor.sin_family = AF_INET;			//Protocolo de conexion
    //Contemplar que no levanta del archivo de configuraciones la ip del servidor
    miDireccionServidor.sin_addr.s_addr = INADDR_ANY;	//INADDR_ANY = 0 y significa que usa la IP actual de la maquina
    miDireccionServidor.sin_port = htons(puerto);		//Puerto en el que escucha
   memset(&(miDireccionServidor.sin_zero), '\0', 8);	//Pone 0 al campo de la estructura "miDireccionServidor"

    //Veamos si el puerto está en uso
    int puertoEnUso = 1;
    int puertoYaAsociado = setsockopt(socketDeEscucha, SOL_SOCKET, SO_REUSEADDR, (char*) &puertoEnUso, sizeof(puertoEnUso));

    if (puertoYaAsociado == ERROR) {
        log_error(logger, "El puerto asignado ya está siendo utilizado.");
    }
    //Turno del bind
    int activado = 1;
    //Para evitar que falle el bind, al querer usar un mismo puerto
    setsockopt(socketDeEscucha,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));

    int valorBind = bind(socketDeEscucha,(void*) &miDireccionServidor, sizeof(miDireccionServidor));

    if ( valorBind !=0) {
        log_error(logger, "El bind no funcionó, el socket no se pudo asociar al puerto");
        return 1;
    }

    //log_info(logger, "Servidor levantado en el puerto %i", puerto);

    return socketDeEscucha;
}

//Habilitar socket servidor de esucha
void escucharSocketsEn(int fd_socket ,t_log* logger){

	int valorListen;
	valorListen = listen(fd_socket, SOMAXCONN);/*Le podríamos poner al listen SOMAXCONN como segundo parámetro, y significaría el máximo tamaño de la cola*/
	if(valorListen == ERROR) {
		puts("El servidor no pudo recibir escuchar conexiones de clientes.\n");
	} else	{
		puts("¡Hola, estoy escuchando!");
	}
}

int crearSocketEscucha (int puerto, t_log* logger) {

	int socketDeEscucha = crearSocketServidor(puerto, logger);

	//Escuchar conexiones
	escucharSocketsEn(socketDeEscucha, logger);

	// hasta que no salga del listen, nunca va a retornar el socket del servidor ya que el listen es bloqueante

	return socketDeEscucha;
}


/**
 ***Crea socket para conexion con servidor y se conecta***
 *  pasados por parametro.
 * Retorna ERROR si no se pudo conectar con el servidor.
 **/
int crearSocketCliente(char *ipServidor, int puerto, t_log* logger) {

    int cliente;
    struct sockaddr_in direccionServidor;

    direccionServidor.sin_family = AF_INET;				// Ordenación de bytes de la máquina
    direccionServidor.sin_addr.s_addr = inet_addr(ipServidor);
    direccionServidor.sin_port = htons(puerto);			// short, Ordenación de bytes de la red
    memset(&(direccionServidor.sin_zero), '\0', 8); 	// Pone cero al resto de la estructura

    cliente = crearSocket(logger); //Creamos socket
    int valorConnect = connect(cliente, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor));

    if(valorConnect == ERROR)  {
        log_error(logger, "No se pudo establecer conexión entre el socket y el servidor.");
        return ERROR;
    }
    else {
//        //log_info(logger, "Se estableció correctamente la conexión con el servidor a través del socket %i.", cliente);
        return cliente;
    }
}
