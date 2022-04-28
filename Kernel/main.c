#include "include/main.h"


t_log* kernel_logger;


int main(int argc, char** argv){

	kernel_logger = log_create("kernel.log","KERNEL",1,LOG_LEVEL_DEBUG);

	t_config* kernel_config=config_create("kernel.config");
	char* ip_kernel=config_get_string_value(kernel_config, IP_KERNEL);
	char* puerto_escucha=config_get_string_value(kernel_config, PUERTO_KERNEL);

	int server_fd = iniciar_servidor();
	log_info(kernel_logger, "Servidor inicializado");
	int cliente_fd = esperar_cliente(server_fd);



}

int iniciar_servidor(){

	int status, sockfd, socket_cliente;
	struct sockaddr_storage their_addr;
	socklen_t addr_size;
	struct addrinfo hints, *res;
	struct addrinfo *servinfo;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;


	getaddrinfo("127.0.0.1",PORT,&hints,&res);

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	bind(sockfd, res->ai_addr, res->ai_addrlen);

	listen(sockfd, 5);



}
