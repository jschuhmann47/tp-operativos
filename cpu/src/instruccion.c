#include "instruccion.h"

pthread_mutex_t mutex_cpu;


void hacer_ciclo_de_instruccion(t_pcb* pcb,t_mensaje_tamanio* bytes,int socketKernelDispatch){
    log_info(cpuLogger, "CPU: Ejecutando instruccion");
    bool salirPorExit = false;
    bool salirPorIO = false;
    bool salirPorInterrupcion = false;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    while(1){ //CHEQUEAR QUE SEA EXIT, INTERRUPCION Y I/O
        t_instruccion* instruccionAEjecutar = cpu_fetch(pcb);
        bool necesitaOperandos = cpu_decode(instruccionAEjecutar);
        
        if(necesitaOperandos){ 
            uint32_t operando = cpu_fetch_operands(instruccionAEjecutar); 
            cpu_execute_con_operando(instruccionAEjecutar,operando);

        }else{
            cpu_execute(instruccionAEjecutar,pcb);
        }
        
        pcb->programCounter++;
        salirPorInterrupcion = cpu_check_interrupcion();

        if(instruccionAEjecutar->indicador == EXIT_I){
            log_info(cpuLogger, "CPU: Desalojo por instruccion EXIT");
            calcularTiempoEnMs(pcb,start,end);
            mandar_pcb_a_kernel(pcb,bytes,socketKernelDispatch);
            break;
        }
        if (instruccionAEjecutar->indicador == I_O){
            log_info(cpuLogger, "CPU: Desalojo por instruccion I/O");
            uint32_t* tiempoABloquearPorIO=malloc(sizeof(uint32_t));
            tiempoABloquearPorIO = list_get(instruccionAEjecutar->parametros,0);
            calcularTiempoEnMs(pcb,start,end);
            mandar_pcb_a_kernel_con_io(pcb,bytes,socketKernelDispatch,*tiempoABloquearPorIO);
            free(tiempoABloquearPorIO);
            break;
        }
        if(salirPorInterrupcion){
            log_info(cpuLogger, "CPU: Hay una interrupción, devolviendo PCB a Kernel");
            calcularTiempoEnMs(pcb,start,end);
            mandar_pcb_a_kernel(pcb,bytes,socketKernelDispatch);
            break;
        }
    }
}


void calcularTiempoEnMs(t_pcb* pcb,struct timespec start,struct timespec end){ //la nueva rafaga se la asigna aca
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    uint32_t tiempoEnMs = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    log_info(cpuLogger, "El tiempo en ejecucion fue de %i ms", tiempoEnMs);
    pcb->dur_ultima_rafaga=tiempoEnMs;
}

t_instruccion* cpu_fetch (t_pcb* pcb){
    return list_get(pcb->instrucciones,pcb->programCounter); 
}

bool cpu_decode(t_instruccion* instruccion){
    if(instruccion->indicador == COPY){
        return true;
    }
    return false;
}

void cpu_execute(t_instruccion* instruccion,t_pcb* pcb /*int operando*/){
    code_instruccion codOp = instruccion->indicador;
    switch (codOp)
    {
    case NO_OP:
        usleep(cpuCfg->RETARDO_NOOP);
        break;
    case I_O: 
        pcb->status=BLOCKED;
        break;
    case WRITE:
        /* code */
        break;
    case READ:
        /* code */
        break;
    case EXIT:
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
    return 1;
}

bool cpu_check_interrupcion(){
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

