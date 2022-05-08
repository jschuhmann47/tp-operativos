#include "../include/ejecucion.h"


int server_escuchar(t_log* logger, char* server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    log_info(logger, "Un cliente se conecto");

    if (cliente_socket != -1) {
        log_info(cpu_logger, "Socket asignado");
        return 1;
    }
    return 0;
}