
#include "include/main.h"

t_log* consola_logger;


int main(){
	//log_create(char* file, char *process_name, bool is_active_console, t_log_level level)
	consola_logger = log_create("consola.log","CONSOLA",1,LOG_LEVEL_DEBUG);

	struct Instruccion lista_inst;
	FILE* archivo=fopen("proceso1.txt","r");
	struct Instruccion hola = parser(archivo);
	printf(hola.codigo_instruccion); //esto es a modo de ejemplo

	//Luego, leerá su archivo de configuración, se conectará al Kernel y le enviará toda la información del proceso (lista de instrucciones, tamaño).

	t_config* consola_config=config_create("consola.config");
	char* ip_kernel=config_get_string_value(consola_config, "IP_KERNEL");
	char* puerto_kernel=config_get_string_value(consola_config, "PUERTO_KERNEL");

	int consola_fd=crear_socket(ip_kernel,puerto_kernel);

	uint32_t handshake = 20;
	uint32_t result;

	send(consola_fd, &handshake, sizeof(uint32_t), 0);
	recv(consola_fd, &result, sizeof(uint32_t), MSG_WAITALL);
























	return 0;

}

int crear_socket(char* ip_kernel,char* puerto_kernel){
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip_kernel, puerto_kernel, &hints, &server_info);


	int consola_fd = socket(server_info->ai_family, server_info->ai_socktype,server_info->ai_protocol);
	if(connect(consola_fd, server_info->ai_addr, server_info->ai_addrlen)==-1){
		log_info(consola_logger,"Error al conectar el socket");
		return -1;
	}
	freeaddrinfo(server_info);
	return consola_fd;
}


















