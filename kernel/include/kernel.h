#ifndef KERNEL_H_INCLUDED
#define KERNEL_H_INCLUDED

#include <commons/log.h>

#include "kernel_config.h"
#include "common_utils.h"
#include "conexiones.h"
#include "planificador.h"


#define KERNEL_CFG_PATH "cfg/kernel.cfg"
#define KERNEL_LOG_DEST "bin/kernel.log"
#define KERNEL_MODULE_NAME "Kernel"

t_log* kernelLogger;
t_kernel_config* kernelCfg;

int socketCpuDispatch;
int socketCpuInterrupt;
int socketMemoria;

void aceptar_conexiones_kernel(int socketEscucha, struct sockaddr cliente, socklen_t len);
void crear_hilo_handler_conexion_entrante(int socket);
void creacion_pcb();

#endif
