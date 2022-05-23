#include "cpu.h"


// typedef struct t_pcb t_pcb; // Se declara este para poder usar el del "algortimo_update"
// struct t_pcb
// {
//     uint32_t id;        //como hacemos incremental el id? buscar.
//     t_status status;
//     uint32_t tamanio;
//     t_list *instrucciones;
//     void *programCounter;
//     // TODO: Tabla de paginas
//     // Estos dos ultimos solo se usan cuando es SRT
//     double est_rafaga_actual; // Esta en Milisegundos
//     void (*algoritmo_siguiente_estim)(t_pcb *self, time_t tiempoFinal, time_t tiempoInicial);
// };



void ejecutar_instruccion(t_pcb* pcb){
    
    t_instruccion* instruccionAEjecutar = cpu_fetch(pcb); //utilizaremos el Program Counter (también llamado Instruction Pointer),  que representa el número de instrucción a buscar, para buscar la próxima instrucción. Al finalizar el ciclo, este último deberá ser actualizado (sumarle 1).
    bool necesitaOperandos = cpu_decode(instruccionAEjecutar);
    
    if(necesitaOperandos){
        uint32_t operando = cpu_fetch_operands(instruccionAEjecutar); //va a buscarlos a memoria solo si es COPY
        cpu_execute_con_operando(instruccionAEjecutar,operando);
    }else{
        cpu_execute(instruccionAEjecutar);
    }
    cpu_check_interrupt(??);
    
}




t_instruccion* cpu_fetch (t_pcb* pcb){

    return list_get(pcb->instrucciones,pcb->programCounter); //uint32_t o int deberia ser el program counter? 
}

bool cpu_decode(t_instruccion* instruccion){
    //parsear aca?? o puede leer asi de una
    if(instruccion->indicador == COPY){
        return true;
    }
    return false;
}

uint32_t cpu_fetch_operands(instruccionAEjecutar){
    //todo: buscar en memoria el operando
}

void cpu_execute(t_instruccion* instruccion){
    switch (instruccion->codigo.operacion) //no esta en el struct, ver de donde sale o si llega parseado
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

