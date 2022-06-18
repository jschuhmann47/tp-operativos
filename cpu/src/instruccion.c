#include "instruccion.h"

pthread_mutex_t mutex_cpu;


void hacer_ciclo_de_instruccion(t_pcb* pcb,t_mensaje_tamanio* bytes,int socketKernelDispatch, int socket_memoria){
    log_info(cpuLogger, "CPU: Ejecutando instruccion");
   
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    while(1){ 
        t_instruccion* instruccionAEjecutar = cpu_fetch(pcb);
        bool necesitaOperandos = cpu_decode(instruccionAEjecutar);
        
        if(necesitaOperandos){ 
            uint32_t operando = cpu_fetch_operands(instruccionAEjecutar, socket_memoria); 
            cpu_execute_con_operando(instruccionAEjecutar,operando,socket_memoria);
        }else{
            cpu_execute(instruccionAEjecutar,pcb,socket_memoria);
        }
        
        pcb->programCounter++;
        bool salirPorInterrupcion = cpu_check_interrupcion();

        if(instruccionAEjecutar->indicador == EXIT_I){
            log_info(cpuLogger, "CPU: Desalojo por instruccion EXIT");
            calcularTiempoEnMs(pcb,start,end);
            limpiar_tlb();
            mandar_pcb_a_kernel(pcb,bytes,socketKernelDispatch);
            break;
        }
        if (instruccionAEjecutar->indicador == I_O){
            log_info(cpuLogger, "CPU: Desalojo por instruccion I/O");
            calcularTiempoEnMs(pcb,start,end);
            limpiar_tlb();
            uint32_t* tiempoABloquearPorIO=list_get(instruccionAEjecutar->parametros,0);
            mandar_pcb_a_kernel_con_io(pcb,bytes,socketKernelDispatch,*tiempoABloquearPorIO);
            //free(tiempoABloquearPorIO);
            break;
        }
        if(salirPorInterrupcion){
            log_info(cpuLogger, "CPU: Hay una interrupción, devolviendo PCB a Kernel");
            calcularTiempoEnMs(pcb,start,end);
            mandar_pcb_a_kernel(pcb,bytes,socketKernelDispatch);
            limpiar_tlb();
            break;
        }
    }
}

void calcularTiempoEnMs(t_pcb* pcb,struct timespec start,struct timespec end){ //la nueva rafaga se la asigna aca
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    uint32_t tiempoEnMs = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    log_info(cpuLogger, "El tiempo en ejecucion fue de %li ms", tiempoEnMs/1000);
    pcb->dur_ultima_rafaga=tiempoEnMs/1000;
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

void cpu_execute(t_instruccion* instruccion,t_pcb* pcb, int socket_memoria){
    code_instruccion codOp = instruccion->indicador;
    uint32_t indice;
    switch (codOp)
    {
    case NO_OP:
        sleep(cpuCfg->RETARDO_NOOP/1000);
        log_info(cpuLogger, "CPU: Ejecute NO_OP");
        break;
    case I_O: 
        pcb->status=BLOCKED;
        log_info(cpuLogger, "CPU: Ejecute I/O");
        break;
    case WRITE:
        ;
        uint32_t* direccionLogicaW = list_get(instruccion->parametros, 0);
        indice = obtener_indice_traduccion_tlb(*direccionLogicaW);
        uint32_t direccionFisicaW;
        if(indice==-1){ //no esta en la tlb
            direccionFisicaW = traducir_direccion(*direccionLogicaW, tamanioPagina, paginasPorTabla,socket_memoria);
            agregar_a_tlb(*direccionLogicaW, direccionFisicaW);
        }else{
            direccionFisicaW = obtener_indice_traduccion_tlb(indice);
        }
        uint32_t* valor = list_get(instruccion->parametros, 1);
        log_info(cpuLogger, "CPU: Ejecute WRITE: %i ", *direccionLogicaW);
        log_info(cpuLogger, "CPU: Ejecute WRITE: %i ", *valor);
        if(mandar_instruccion(WRITE,direccionFisicaW,*valor,socket_memoria)>0){ //cambiar op code
            log_info(cpuLogger, "CPU: Se mando instruccion WRITE a Memoria.");
        }else{
            log_info(cpuLogger, "CPU: No se pudo mandar instruccion WRITE a Memoria.");
        }
        
        break;
    case READ:
        ;
        uint32_t* direccionLogicaR = list_get(instruccion->parametros, 0);
        indice = obtener_indice_traduccion_tlb(*direccionLogicaR);
        uint32_t direccionFisicaR;
        if(indice==-1){ //no esta en la tlb
            direccionFisicaR = traducir_direccion(*direccionLogicaR, tamanioPagina, paginasPorTabla,socket_memoria);
            agregar_a_tlb(*direccionLogicaR, direccionFisicaR);
        }else{
            direccionFisicaR = obtener_indice_traduccion_tlb(indice);
        }
        log_info(cpuLogger, "CPU: Ejecute READ: %i ", *direccionLogicaR);
        if(mandar_instruccion(READ,direccionFisicaR,0,socket_memoria)>0){
            log_info(cpuLogger, "CPU: Se mando instruccion READ a Memoria.");
        }
        else{
            log_info(cpuLogger, "CPU: No se pudo mandar instruccion READ a Memoria.");
        }
        break;
    case EXIT_I:
        pcb->status=EXIT;
        log_info(cpuLogger, "CPU: Ejecute EXIT");
        break;
    default:
        break;
    }
}

void cpu_execute_con_operando(t_instruccion* instruccion,uint32_t operando, int socket_memoria){
    //el COPY y el WRITE son iguales solo que el valor que entra en WRITE lo mandan, y el de COPY se busca en memoria, pero despues
    //de conseguir el valor de ahi en adelante es igual
    uint32_t* direccionLogicaW = list_get(instruccion->parametros, 0);
    uint32_t indice = obtener_indice_traduccion_tlb(*direccionLogicaW);
    uint32_t direccionFisicaW;
    if(indice==-1){ //no esta en la tlb
        direccionFisicaW = traducir_direccion(*direccionLogicaW, tamanioPagina, paginasPorTabla,socket_memoria);
        agregar_a_tlb(*direccionLogicaW, direccionFisicaW);
    }else{
        direccionFisicaW = obtener_indice_traduccion_tlb(indice);
    }
    log_info(cpuLogger, "CPU: Ejecute COPY: %i ", direccionFisicaW);
    log_info(cpuLogger, "CPU: Ejecute COPY: %i ", operando);
    if(mandar_instruccion(WRITE,direccionFisicaW,operando,socket_memoria)){
        log_info(cpuLogger, "CPU: Se mando instruccion COPY/WRITE a Memoria.");
    }else{
        log_info(cpuLogger, "CPU: No se pudo mandar instruccion COPY/WRITE a Memoria.");
    }
    
}

uint32_t cpu_fetch_operands(t_instruccion* instruccion, int socket_memoria){
    uint32_t* direccionMemoriaAObtener = list_get(instruccion->parametros,1); //COPY dirección_lógica_destino dirección_lógica_origen
    uint32_t indice = obtener_indice_traduccion_tlb(*direccionMemoriaAObtener);
    uint32_t direccionFisicaR;
    if(indice==-1){ //no esta en la tlb
        direccionFisicaR = traducir_direccion(*direccionMemoriaAObtener, tamanioPagina, paginasPorTabla,socket_memoria);
        agregar_a_tlb(*direccionMemoriaAObtener, direccionFisicaR);
    }else{
        direccionFisicaR = obtener_indice_traduccion_tlb(indice);
    }

    log_info(cpuLogger, "CPU: Fetch operands: %i ", direccionFisicaR);
    if(mandar_instruccion(READ,direccionFisicaR,0,socket_memoria)){
        log_info(cpuLogger, "CPU: Se mando instruccion COPY/READ a Memoria.");
    }
    else{
        log_info(cpuLogger, "CPU: No se pudo mandar instruccion COPY/READ a Memoria.");
    }
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

