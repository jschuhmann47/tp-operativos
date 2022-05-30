#include "consola_config.h"

t_consola_config *consola_cfg_create(void)
{
	t_consola_config *newConsolaCfg = malloc(sizeof(t_consola_config));
	newConsolaCfg->IP_KERNEL = NULL;
	newConsolaCfg->PUERTO_KERNEL = NULL;
	return newConsolaCfg;
}

void consola_config_initialize(void *consolaCfg, t_config *config)
{
	t_consola_config *cfg = (t_consola_config *)consolaCfg;
	cfg->IP_KERNEL = strdup(config_get_string_value(config, "IP_KERNEL"));
	cfg->PUERTO_KERNEL = strdup(config_get_string_value(config, "PUERTO_KERNEL"));
}

void enviar_mensaje(char *mensaje, int socket_cliente, op_code cod_op)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	char *datos = string_new();
	string_append(&datos, mensaje);

	paquete->codigo_operacion = cod_op;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(datos) + 1;
	paquete->buffer->stream = datos;
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	uint32_t bytes = paquete->buffer->size + sizeof(op_code) + sizeof(uint32_t);

	void *a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void *serializar_paquete(t_paquete *paquete, uint32_t bytes)
{
	void *buffer = malloc(bytes);

	if (buffer == NULL)
	{
		printf("No hay memoria para buffer en  serializar_paquete");
	};

	uint32_t bytes_escritos = 0;

	memcpy(buffer + bytes_escritos, &(paquete->codigo_operacion), sizeof(op_code));
	bytes_escritos += sizeof(uint32_t);
	memcpy(buffer + bytes_escritos, &(paquete->buffer->size), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);
	memcpy(buffer + bytes_escritos, paquete->buffer->stream, paquete->buffer->size);
	bytes_escritos += paquete->buffer->size;

	return buffer;
}

void liberar_modulo_consola(t_log *consolaLogger, t_consola_config *consolaCfg)
{
	log_destroy(consolaLogger);
	free(consolaCfg->IP_KERNEL);
	free(consolaCfg->PUERTO_KERNEL);
	free(consolaCfg);
}

void eliminar_paquete(t_paquete *paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void terminar_conexion(int socketKernel)
{
    close(socketKernel);
	log_info(consolaLogger, "Conexion finalizada.");
}

char* recibir_mensaje_finalizacion(int socketKernel)
{
	char* buffer = recibir_buffer(socketKernel);

    return buffer;
}

void* recibir_buffer(int socketKernel)
{
	void *buffer;
    uint32_t size = 0;

	recv(socketKernel, &size, sizeof(uint32_t), MSG_WAITALL);
	buffer = malloc(size);
	recv(socketKernel, buffer, size, MSG_WAITALL);

	return buffer;
}