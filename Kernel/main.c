
#include "include/main.h"


//p_thread

t_log* kernel_logger;


int main(int argc, char** argv){

	kernel_logger = log_create("kernel.log","KERNEL",1,LOG_LEVEL_DEBUG);

	t_config* kernel_config=config_create("kernel.config");
	char* ip_kernel=config_get_string_value(kernel_config, "IP_KERNEL");
	char* puerto_kernel=config_get_string_value(kernel_config, "PUERTO_ESCUCHA");

	int server_fd = iniciar_servidor(kernel_logger,"Kernel",ip_kernel,puerto_kernel);
	log_info(kernel_logger, "Servidor Kernel inicializado");

	while(server_escuchar(kernel_logger,"Kernel",server_fd)); //esto es infinito, no tiene que cortarse
	
	//crear un hilo para atender la coenxion
	//hilo main siempre escucha y el resto atiende


	return 0;
	//cosas


}




