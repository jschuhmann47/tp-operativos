#ifndef CPU_H_INCLUDED
#define CPU_H_INCLUDED

#include <commons/log.h>

#include "cpu_config.h"
#include "common_utils.h"
#include "conexiones.h"
#include "instruccion.h"


#define CPU_CFG_PATH "cfg/cpu.cfg"
#define CPU_LOG_DEST "bin/cpu.log"
#define CPU_MODULE_NAME "Cpu"

struct conexion_cpu{
    int socket;
    struct sockaddr sockAddr;
    socklen_t sockrAddrLen;
}conexion_cpu;


t_log* cpuLogger;
t_cpu_config* cpuCfg;

void aceptar_conexiones_cpu(struct conexion_cpu* conexion);
void aceptar_conexiones_cpu_interrupcion(struct conexion_cpu* conexion);
void recibir_pcb_de_kernel(int socketKernelDispatch);
void mandar_pcb_a_kernel_con_io(t_pcb* pcb, t_mensaje_tamanio* bytes, int socketKernelDispatch,uint32_t tiempoABloquearse);
void mandar_pcb_a_kernel(t_pcb* pcb, t_mensaje_tamanio* bytes, int socketKernelDispatch);
void* check_interrupt();


#endif
