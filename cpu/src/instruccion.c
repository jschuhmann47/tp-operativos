#include "instruccion.h"

pthread_mutex_t mutex_cpu;


void hacer_ciclo_de_instruccion(t_pcb* pcb,t_mensaje_tamanio* bytes,int socketKernelDispatch, int socket_memoria){
   
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);

    while(1){ 
        t_instruccion* instruccionAEjecutar = cpu_fetch(pcb);
        bool necesitaOperandos = cpu_decode(instruccionAEjecutar);
        
        if(necesitaOperandos){ 
            uint32_t operando = cpu_fetch_operands(instruccionAEjecutar, pcb,socket_memoria); 
            cpu_execute_con_operando(instruccionAEjecutar,pcb,operando,socket_memoria);
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

void calcularTiempoEnMs(t_pcb* pcb,struct timespec start,struct timespec end){
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    uint32_t tiempoEnNs = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    log_info(cpuLogger, "El tiempo en ejecucion fue de %i ms", tiempoEnNs/1000);
    pcb->dur_ultima_rafaga=tiempoEnNs/1000;
    pcb->acum_rafaga+=pcb->dur_ultima_rafaga;
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
        uint32_t* valor = list_get(instruccion->parametros, 1);
        log_debug(cpuLogger, "CPU: Ejecute WRITE: direccion logica: %i , valor %i", *direccionLogicaW,*valor);
        if(mandar_instruccion(WRITE,traducir_direccion_logica(*direccionLogicaW, socket_memoria,cpuLogger,pcb->tablaDePaginas),*valor,socket_memoria,cpuLogger)>0){
            log_info(cpuLogger, "CPU: Se mando instruccion WRITE a Memoria.");
            if(send(socket_memoria,&pcb->tablaDePaginas,sizeof(uint32_t),0)<0){
                log_error(cpuLogger, "CPU: Error al enviar nro de tabla de primer nivel a Memoria.");
            }
            uint32_t entradaTablaPrimerNivel = floor(floor(*direccionLogicaW/tamanioPagina) / paginasPorTabla);
            if(send(socket_memoria,&entradaTablaPrimerNivel,sizeof(uint32_t),0)<0){
                log_error(cpuLogger, "CPU: Error al enviar entrada de tabla de primer nivel a Memoria.");
            }
        }else{
            log_info(cpuLogger, "CPU: No se pudo mandar instruccion WRITE a Memoria.");
        }        
        break;
    case READ:
        ;
        uint32_t* direccionLogicaR = list_get(instruccion->parametros, 0);
        log_debug(cpuLogger, "CPU: Ejecute READ: %i ", *direccionLogicaR);
        if(mandar_instruccion(READ,traducir_direccion_logica(*direccionLogicaR, socket_memoria,cpuLogger,pcb->tablaDePaginas),0,socket_memoria,cpuLogger)>0){
            log_info(cpuLogger, "CPU: Se mando instruccion READ a Memoria.");
            uint32_t leido;
            if(recv(socket_memoria,&leido,sizeof(uint32_t),MSG_WAITALL)<0){
                log_error(cpuLogger,"No se pudo recibir el valor leido por READ");
                exit(-1);
            }
            log_info(cpuLogger,"CPU: Instruccion READ finalizada, valor leido %i",leido);
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

void cpu_execute_con_operando(t_instruccion* instruccion,t_pcb* pcb,uint32_t operando, int socket_memoria){
    uint32_t* direccionLogicaW = list_get(instruccion->parametros, 0);
    uint32_t* valor = list_get(instruccion->parametros, 1);
    log_debug(cpuLogger, "CPU: Ejecute COPY (ejecucion), direccion logica:%i, operando: %i ", *direccionLogicaW,operando);

    if(mandar_instruccion(WRITE,traducir_direccion_logica(*direccionLogicaW, socket_memoria,cpuLogger,pcb->tablaDePaginas),operando,socket_memoria,cpuLogger)>0){
        if(send(socket_memoria,&pcb->tablaDePaginas,sizeof(uint32_t),0)<0){
            log_error(cpuLogger, "CPU: Error al enviar nro de tabla de primer nivel a Memoria.");
        }
        uint32_t entradaTablaPrimerNivel = floor(floor(*direccionLogicaW/tamanioPagina) / paginasPorTabla);
        if(send(socket_memoria,&entradaTablaPrimerNivel,sizeof(uint32_t),0)<0){
            log_error(cpuLogger, "CPU: Error al enviar entrada de tabla de primer nivel a Memoria.");
        }
        log_info(cpuLogger, "CPU: Se mando instruccion COPY (ejecucion) a Memoria.");
    }else{
        log_info(cpuLogger, "CPU: No se pudo mandar instruccion COPY/WRITE a Memoria.");
    }
}

uint32_t cpu_fetch_operands(t_instruccion* instruccion, t_pcb* pcb,int socket_memoria){
    uint32_t* direccionMemoriaAObtener = list_get(instruccion->parametros,1); //COPY dirección_lógica_destino dirección_lógica_origen
    uint32_t leido;
    log_debug(cpuLogger,"CPU: Ejecute COPY (fetch operands): %i ", *direccionMemoriaAObtener);
    if(mandar_instruccion(READ,traducir_direccion_logica(*direccionMemoriaAObtener, socket_memoria,cpuLogger,pcb->tablaDePaginas),0,socket_memoria,cpuLogger)){
        if(recv(socket_memoria,&leido,sizeof(uint32_t),MSG_WAITALL)<0){
            log_error(cpuLogger,"No se pudo recibir el valor leido por READ");
            exit(-1);
        }
        log_info(cpuLogger, "CPU: Se recibio operando de COPY de Memoria.");
    }
    else{
        log_info(cpuLogger, "CPU: No se pudo mandar instruccion COPY/READ a Memoria.");
    }
    return leido;
}

bool cpu_check_interrupcion(){
    pthread_mutex_lock(&mutex_interrupciones);
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

