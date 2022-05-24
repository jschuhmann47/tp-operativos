#ifndef CPU_INSTRUCCION_H_INCLUDED
#define CPU_INSTRUCCION_H_INCLUDED

#include "cpu_config.h"
#include "common_utils.h"
#include "conexiones.h"

void ejecutar_instruccion(t_pcb* pcb);
t_instruccion* cpu_fetch (t_pcb* pcb);
bool cpu_decode(t_instruccion* instruccion);
void cpu_execute(t_instruccion* instruccion);
void cpu_execute_con_operandos(t_instruccion* instruccion,uint32_t operando);
uint32_t cpu_fetch_operands(t_instruccion* instruccion);

#endif