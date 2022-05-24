#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <commons/log.h>

#include "cpu_config.h"
#include "common_utils.h"
#include "conexiones.h"

#define CPU_CFG_PATH "cfg/cpu.cfg"
#define CPU_LOG_DEST "bin/cpu.log"
#define CPU_MODULE_NAME "Cpu"

t_log* cpuLogger;
t_cpu_config* cpuCfg;

void aceptar_conexiones_cpu(int socketEscucha, struct sockaddr cliente, socklen_t len);
void recibir_pcb_de_kernel(int socketKernelDispatch);

#endif
