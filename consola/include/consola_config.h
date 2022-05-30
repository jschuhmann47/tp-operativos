#ifndef CONSOLA_CONFIG_H_INCLUDED
#define CONSOLA_CONFIG_H_INCLUDED

#include <sys/socket.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "structs.h"
#include <stdio.h>

#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>

typedef struct {
    char* IP_KERNEL;
    char* PUERTO_KERNEL;
} t_consola_config;

typedef struct{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

extern t_log* consolaLogger;
extern t_consola_config* consolaCfg;

t_consola_config* consola_cfg_create(void);
void consola_config_initialize(void* consolaCfg, t_config* config);
void enviar_mensaje(char* mensaje, int socket_cliente, op_code cod_op);
void enviar_instrucciones(t_list* instrucciones, int socket_cliente);
void* serializar_paquete(t_paquete* paquete, uint32_t bytes);
void liberar_modulo_consola(t_log* consolaLogger, t_consola_config* consolaCfg);
void eliminar_paquete(t_paquete* paquete);
t_instruccion* crear_instruccion(void);
void terminar_conexion(int socketKernel);
char* recibir_mensaje_finalizacion(int socketKernel);
void* recibir_buffer(int socketKernel);

#endif
