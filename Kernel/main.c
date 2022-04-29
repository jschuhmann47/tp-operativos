#include "../shared/include/sockets.h"
#include "include/main.h"
#include <commons/config.h>


t_log* kernel_logger;


int main(int argc, char** argv){

	kernel_logger = log_create("kernel.log","KERNEL",1,LOG_LEVEL_DEBUG);

	t_config* kernel_config=config_create("kernel.config");
	char* ip_kernel=config_get_string_value(kernel_config, "IP_KERNEL");
	char* puerto_kernel=config_get_string_value(kernel_config, "PUERTO_ESCUCHA");

	int server_fd = iniciar_servidor(kernel_logger,"Kernel",IP_KERNEL,PUERTO_KERNEL);
	log_info(kernel_logger, "Servidor Kernel inicializado");
	int cliente_fd = esperar_cliente(kernel_logger,"Kernel",server_fd);

	//cosas



}
