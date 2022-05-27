#ifndef CPU_INSTRUCCION_H_INCLUDED
#define CPU_INSTRUCCION_H_INCLUDED

#include "cpu_config.h"
#include "common_utils.h"
#include "conexiones.h"

void hacer_ciclo_de_instruccion(t_pcb* pcb);
t_instruccion* cpu_fetch (t_pcb* pcb);
bool cpu_decode(t_instruccion* instruccion);
void cpu_execute(t_instruccion* instruccion,t_pcb* pcb);
void cpu_execute_con_operando(t_instruccion* instruccion,uint32_t operando);
uint32_t cpu_fetch_operands(t_instruccion* instruccion);
void cpu_check_interrupt();

#endif