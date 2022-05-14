#include "include/main.h"



t_log* cpu_logger;


int main(int argc, char** argv){

	cpu_logger = log_create("cpu.log","CPU",1,LOG_LEVEL_DEBUG);

	t_config* cpu_config=config_create("cpu.config");
	char* ip_cpu=config_get_string_value(cpu_config, "IP_CPU");
	char* puerto_cpu_dispatch=config_get_string_value(cpu_config, "PUERTO_ESCUCHA_DISPATCH"); //puerto donde llegan las instrucciones comunes
	char* puerto_cpu_interrupt=config_get_string_value(cpu_config, "PUERTO_ESCUCHA_INTERRUPT"); //puerto donde llegan las interrupciones y que se va a atender primero

	int dispatch_fd = iniciar_servidor(cpu_logger,"Cpu",ip_cpu,puerto_cpu_dispatch);
	log_info(cpu_logger, "Puerto CPU Dispatch inicializado");
	int interrupt_fd = iniciar_servidor(cpu_logger,"Cpu",ip_cpu,puerto_cpu_interrupt);
	log_info(cpu_logger, "Puerto CPU Interrupt inicializado");

	char* ip_memoria=config_get_string_value(cpu_config, "IP_MEMORIA");
	char* puerto_memoria=config_get_string_value(cpu_config, "PUERTO_MEMORIA");


	int memoria_fd=crear_conexion_a_server(cpu_logger,"Kernel",ip_memoria,puerto_memoria); //Conexion a CPU


	realizar_handshake(memoria_fd,cpu_logger);



	while(server_escuchar(cpu_logger,"Cpu",dispatch_fd)); //ver como poner a escuchar los 2 a la vez 
	
	return 0;
}

