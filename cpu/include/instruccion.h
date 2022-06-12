#ifndef CPU_INSTRUCCION_H_INCLUDED
#define CPU_INSTRUCCION_H_INCLUDED

#include "cpu_config.h"
#include "common_utils.h"
#include "conexiones.h"
#include <unistd.h>
#include <time.h>
#include "cpu_config.h"
#include "cpu.h"
#include "mmu.h"

void hacer_ciclo_de_instruccion(t_pcb* pcb, t_mensaje_tamanio* bytes, int socketKernelDispatch, int socket_memoria);
t_instruccion* cpu_fetch (t_pcb* pcb);
bool cpu_decode(t_instruccion* instruccion);
void cpu_execute(t_instruccion* instruccion,t_pcb* pcb, int socket_memoria);
void cpu_execute_con_operando(t_instruccion* instruccion,uint32_t operando, int socket_memoria);
uint32_t cpu_fetch_operands(t_instruccion* instruccion, int socket_memoria);
bool cpu_check_interrupcion();
void calcularTiempoEnMs(t_pcb* pcb,struct timespec start,struct timespec end);

#endif