#include "instruccion.h"

pthread_mutex_t mutex_cpu;


void hacer_ciclo_de_instruccion(t_pcb* pcb){
    
    t_instruccion* instruccionAEjecutar = cpu_fetch(pcb);
    bool necesitaOperandos = cpu_decode(instruccionAEjecutar);
    // int operandoIO = 0;
    // operandoIO = 10;
    
    if(necesitaOperandos){ //va a buscarlos a memoria solo si es COPY
        uint32_t operando = cpu_fetch_operands(instruccionAEjecutar); 
        cpu_execute_con_operando(instruccionAEjecutar,operando);

    }else{
        cpu_execute(instruccionAEjecutar,pcb /*operandoDeIo*/);
    }
    pcb->programCounter++;
    cpu_check_interrupt(/*??*/);
    //devolver_pcb_por_io(operandoDeIo) hace de cuenta que esta hecha
    
    
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

// char** hola;

// char a= 'a';
// char* b="hola";

// char** c = {"hola","chau"};
// c[1]="hola";
// c[1][1]='h';

void cpu_execute(t_instruccion* instruccion,t_pcb* pcb /*int operando*/){
    switch (NO_OP /*instruccion->codigo.op*/) //no esta en el struct, ver de donde sale o si llega parseado
    {
    case NO_OP: //TODO
        /* code */ //funcion que les puede servir: usleep()
        break;
    case I_O: //TODO
        /* code */
        pcb->status=BLOCKED;
        break;
    case WRITE:
        /* code */
        break;
    case READ:
        /* code */
        break;
    case EXIT: //TODO
        /* code */
        break;
    default:
        break;
    }
}

void cpu_execute_con_operando(t_instruccion* instruccion,uint32_t operando){
    //el COPY y el WRITE son iguales solo que el valor que entra en WRITE lo mandan, y el de COPY se busca en memoria, pero despues
    //de conseguir el valor de ahi en adelante es igual
    
}

uint32_t cpu_fetch_operands(t_instruccion* instruccion){
    void* direccionMemoriaAObtener = list_get(instruccion->parametros,1); //COPY dirección_lógica_destino dirección_lógica_origen
    //TODO, buscarlo en la memoria
}

void cpu_check_interrupt(){
    
}

