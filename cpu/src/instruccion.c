#include "cpu.h"

pthread_mutex_t mutex_cpu;

void ejecutar_instruccion(t_pcb* pcb){
    
    t_instruccion* instruccionAEjecutar = cpu_fetch(pcb); //utilizaremos el Program Counter (también llamado Instruction Pointer),  que representa el número de instrucción a buscar, para buscar la próxima instrucción. Al finalizar el ciclo, este último deberá ser actualizado (sumarle 1).
    bool necesitaOperandos = cpu_decode(instruccionAEjecutar);
    
    if(necesitaOperandos){
        uint32_t operando = cpu_fetch_operands(instruccionAEjecutar); //va a buscarlos a memoria solo si es COPY
        cpu_execute_con_operando(instruccionAEjecutar,operando);
    }else{
        cpu_execute(instruccionAEjecutar);
    }
    cpu_check_interrupt(/*??*/);
    
}



t_instruccion* cpu_fetch (t_pcb* pcb){
    //devuelve la instruccion de indice programCounter
    return list_get(pcb->instrucciones,pcb->programCounter); //uint32_t o int deberia ser el program counter? 
}

bool cpu_decode(t_instruccion* instruccion){
    
    if(strcmp(instruccion->indicador, "COPY") == 0){
        return true;
    }
    return false;
}


void cpu_execute(t_instruccion* instruccion){
    switch (instruccion->codigo.op) //no esta en el struct, ver de donde sale o si llega parseado
    {
    case NO_OP:
        /* code */
        break;
    case I_O:
        /* code */
        break;
    case WRITE:
        /* code */
        break;
    case READ:
        /* code */
        break;
    case EXIT:
        /* code */
        break;
    default:
        break;
    }
}

void cpu_execute_con_operandos(t_instruccion* instruccion,uint32_t operando){
    //el COPY y el WRITE son iguales solo que el valor que entra en WRITE lo mandan, y el de COPY se busca en memoria, pero despues
    //de conseguir el valor de ahi en adelante es igual
    
}

uint32_t cpu_fetch_operands(t_instruccion* instruccion){
    void* direccionMemoriaAObtener = list_get(instruccion->parametros,1); //COPY dirección_lógica_destino dirección_lógica_origen
    //TODO, buscarlo en la memoria
}

