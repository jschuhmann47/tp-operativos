#include "instruccion.h"

pthread_mutex_t mutex_cpu;


void hacer_ciclo_de_instruccion(t_pcb* pcb){
    log_info(cpuLogger, "CPU: Ejecutando instruccion");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);


    //while(){ CHEQUEAR QUE SEA EXIT, INTERRUPCION Y I/O
        t_instruccion* instruccionAEjecutar = cpu_fetch(pcb);
        log_info(cpuLogger, "CPU: Ejecuté fetch");
        bool necesitaOperandos = cpu_decode(instruccionAEjecutar);
        log_info(cpuLogger, "CPU: Ejecuté decode");
        // int operandoIO = 0;
        // operandoIO = 10;
        
        if(necesitaOperandos){ 
            uint32_t operando = cpu_fetch_operands(instruccionAEjecutar); 
            cpu_execute_con_operando(instruccionAEjecutar,operando);

        }else{
            cpu_execute(instruccionAEjecutar,pcb);
        }
        pcb->programCounter++;
    
    if(cpu_check_interrupt()){
        log_info(cpuLogger, "CPU: Hay una interrupción, devolviendo PCB a Kernel");
        //mandar_pcb_a_kernel(pcb); ver si es justo io??
    }

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    
    u_int32_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    
    log_info(cpuLogger, "Prueba de timer %i", delta_us);

    //La rafaga la seteamos antes de que vuelva a kernel, xq sino en cada instruccion estas pisando el valor anterior
    //Ver bien donde va esto del tiempo, es x aca pero depende de si hay interrupcion o no, dps lo veo bien
    //}
    //devolver_pcb_por_io(operandoDeIo) hace de cuenta que esta hecha
    
    
}




t_instruccion* cpu_fetch (t_pcb* pcb){
    log_info(cpuLogger, "CPU: Ejecutando fetch");
    //devuelve la instruccion de indice programCounter
    return list_get(pcb->instrucciones,pcb->programCounter); 
}

bool cpu_decode(t_instruccion* instruccion){
    
    log_info(cpuLogger, "CPU: Ejecutando decode");
    if(strcmp(instruccion->indicador, "COPY") == 0){
        return true;
    }
    return false;
}

void cpu_execute(t_instruccion* instruccion,t_pcb* pcb /*int operando*/){
    code_instruccion codOp = getCodeIntruccion(instruccion->indicador);
    switch (codOp)
    {
    case NO_OP: //TODO
        usleep(/*cpuCfg->RETARDO_NOOP*/1);
        break;
    case I_O: //TODO
        pcb->status=BLOCKED;
        
        break;
    case WRITE:
        /* code */
        break;
    case READ:
        /* code */
        break;
    case EXIT: //TODO
        pcb->status=EXIT;
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

bool cpu_check_interrupt(){
    pthread_mutex_lock(&mutex_interrupciones);//este mutex comparte con cpu.c
    if(hayInterrupcion){
        hayInterrupcion=0;
        pthread_mutex_unlock(&mutex_interrupciones);
        return true;
    }
    else{
        pthread_mutex_unlock(&mutex_interrupciones);
        return false;
    }
}

