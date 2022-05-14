#include "include/main.h" //faltan meter los include en main.h de memoria


//p_thread

t_log* memoria_logger;


int main(int argc, char** argv){

	memoria_logger = log_create("memoria.log","MEMORIA",1,LOG_LEVEL_DEBUG);

	t_config* memoria_config=config_create("memoria.config");
	char* ip_memoria=config_get_string_value(memoria_config, "IP_MEMORIA");
	char* puerto_memoria=config_get_string_value(memoria_config, "PUERTO_ESCUCHA");

	int server_fd = iniciar_servidor(memoria_logger,"Memoria",ip_memoria,puerto_memoria);
	log_info(memoria_logger, "Servidor Memoria inicializado");


    //handshake
    recibir_handshake(cpu_fd,memoria_logger); //como agarrar el cpu_fd VER
	
	

	while(server_escuchar(memoria_logger,"Memoria",server_fd)); //escucha lo que el cpu venga a hacer
	

	return 0;
}
