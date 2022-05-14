
#include "include/main.h"


//p_thread

t_log* kernel_logger;


int main(int argc, char** argv){ //falta handshake con consola

	kernel_logger = log_create("kernel.log","KERNEL",1,LOG_LEVEL_DEBUG);

	//Abrir server
	t_config* kernel_config=config_create("kernel.config");
	char* ip_kernel=config_get_string_value(kernel_config, "IP_KERNEL");
	char* puerto_kernel=config_get_string_value(kernel_config, "PUERTO_ESCUCHA");

	int server_fd = iniciar_servidor(kernel_logger,"Kernel",ip_kernel,puerto_kernel);
	log_info(kernel_logger, "Servidor Kernel inicializado");


	char* ip_cpu=config_get_string_value(kernel_config, "IP_CPU");
	char* puerto_cpu=config_get_string_value(kernel_config, "PUERTO_CPU");

	int cpu_fd=crear_conexion_a_server(kernel_logger,"Kernel",ip_cpu,puerto_cpu); //Conexion a CPU

	char* ip_memoria=config_get_string_value(kernel_config, "IP_MEMORIA");
	char* puerto_memoria=config_get_string_value(kernel_config, "PUERTO_MEMORIA");

	int memoria_fd=crear_conexion_a_server(kernel_logger,"Kernel",ip_memoria,puerto_memoria); //Conexion a Memoria

	//handshake?

	while(server_escuchar(kernel_logger,"Kernel",server_fd)); //escuchar peticiones de la consola
	
	//crear un hilo para atender la coenxion
	//hilo main siempre escucha y el resto atiende


	return 0;
	//cosas


}




