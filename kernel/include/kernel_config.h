#ifndef KERNEL_CONFIG_H_INCLUDED
#define KERNEL_CONFIG_H_INCLUDED

#include <sys/socket.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "structs.h"
#include <stdio.h>
#include "common_utils.h"

#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>

#include <commons/collections/list.h>

typedef struct {
    int CONSOLA_SOCKET;
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* IP_CPU;
    char* IP_KERNEL;
    char* PUERTO_CPU_DISPATCH;
    char* PUERTO_CPU_INTERRUPT;
    char* PUERTO_ESCUCHA;
    char* ALGORITMO_PLANIFICACION;
    double ESTIMACION_INICIAL;
    double ALFA;
    uint32_t GRADO_MULTIPROGRAMACION;
    uint32_t TIEMPO_MAXIMO_BLOQUEADO;
} t_kernel_config;

extern t_log* kernelLogger;
extern t_kernel_config* kernelCfg;
extern int socketCpuDispatch;
extern int socketCpuInterrupt;
extern int socketMemoria;

t_kernel_config* kernel_cfg_create(void);
void kernel_config_initialize(void* kernelCfg, t_config* config);
void liberar_modulo_kernel(t_log* kernelLogger, t_kernel_config* kernelCfg);
op_code recibir_operacion(int);
char* recibir_mensaje(int);
t_list* recibir_instrucciones(int socket_cliente);
void* recibir_buffer(int);
t_list* convertir_instruccion(char* buffer);
void enviar_finalizacion_consola(char *mensaje, int socket_cliente);
void *serializar_mensaje(t_buffer *mensaje, uint32_t bytes);

#endif
