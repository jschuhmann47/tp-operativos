#include "include/main.h" //faltan meter los include en main.h de memoria


//p_thread

t_log* memoria_logger;


int main(int argc, char** argv){

	memoria_logger = log_create("memoria.log","MEMORIA",1,LOG_LEVEL_DEBUG);

	t_config* memoria_config=config_create("memoria.config");
	char* ip_memoria=config_get_string_value(memoria_config, "IP_MEMORIA");
	char* puerto_memoria=config_get_string_value(memoria_config, "PUERTO_ESCUCHA");

	int server_fd = iniciar_servidor(memoria_logger,"Memoria",ip_memoria,puerto_memoria);
	log_info(memoria_logger, "Servidor Kernel inicializado");


    char* ip_cpu=config_get_string_value(memoria_config, "IP_KERNEL");
	char* puerto_cpu=config_get_string_value(memoria_config, "PUERTO_KERNEL");

	int cpu_fd=crear_conexion_a_server(memoria_logger,"Kernel",ip_cpu,puerto_cpu);



    //handshake
    uint32_t handshake = 1;
    uint32_t result;
    if(send(cpu_fd, &handshake, sizeof(uint32_t), 0)==-1){
    	log_info(memoria_logger, "Fallo envio de handshake");
    	return EXIT_FAILURE;
    }
    if(recv(cpu_fd, &result, sizeof(uint32_t), MSG_WAITALL)==-1){
    	log_info(memoria_logger, "Fallo recibo de handshake");
    	return EXIT_FAILURE;
    }else{
    	log_info(memoria_logger, "Handshake con exito!");
    }

	while(server_escuchar(memoria_logger,"Memoria",server_fd)); //esto es infinito, no tiene que cortarse
	


	return 0;
}
