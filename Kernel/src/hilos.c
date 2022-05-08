#include "../include/hilos.h"


int server_escuchar(t_log* logger, char* server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    log_info(logger, "Un cliente se conecto");

    // if (cliente_socket != -1) {
    //     pthread_t hilo;
    //     t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
    //     args->log = logger;
    //     args->fd = cliente_socket;
    //     args->server_name = server_name;
    //     pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
    //     pthread_detach(hilo);
    //     return 1;
    // }
    //return 0;


    return 1;
}

static void procesar_conexion(void* void_args) { //esto mateo es lo que hace el switch que te dije, al argumento que llega 
                                                //no le des mucha bola, es para lo de los threads
	//todo

}