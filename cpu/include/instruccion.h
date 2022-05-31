#ifndef CPU_INSTRUCCION_H_INCLUDED
#define CPU_INSTRUCCION_H_INCLUDED

#include "cpu_config.h"
#include "common_utils.h"
#include "conexiones.h"
#include <unistd.h>
#include <time.h>
#include "cpu_config.h"
#include "cpu.h"

void hacer_ciclo_de_instruccion(t_pcb* pcb,t_mensaje_tamanio* bytes,int socketKernelDispatch);
t_instruccion* cpu_fetch (t_pcb* pcb);
bool cpu_decode(t_instruccion* instruccion);
void cpu_execute(t_instruccion* instruccion,t_pcb* pcb);
void cpu_execute_con_operando(t_instruccion* instruccion,uint32_t operando);
uint32_t cpu_fetch_operands(t_instruccion* instruccion);
bool cpu_check_interrupt();
void calcularTiempoEnMs(t_pcb* pcb,struct timespec start,struct timespec end);

#endif