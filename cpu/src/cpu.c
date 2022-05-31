#include "cpu.h" //"../include/cpu.h"

int main(int argc, char* argv[]) {
    cpuCfg = cpu_cfg_create();
    cpuLogger = log_create(CPU_LOG_DEST, CPU_MODULE_NAME, true, LOG_LEVEL_INFO);
    cargar_configuracion(CPU_MODULE_NAME, cpuCfg, CPU_CFG_PATH, cpuLogger, cpu_config_initialize);

    log_info(cpuLogger, "Valor de PUERTO_ESCUCHA_DISPATCH %s", cpuCfg->PUERTO_ESCUCHA_DISPATCH);
    log_info(cpuLogger, "Valor de PUERTO_ESCUCHA_INTERRUPT %s", cpuCfg->PUERTO_ESCUCHA_INTERRUPT);


    int socketEscuchaDispatch = iniciar_servidor(cpuCfg->IP_MEMORIA, cpuCfg->PUERTO_ESCUCHA_DISPATCH);

    int socketEscuchaInterrupt = iniciar_servidor(cpuCfg->IP_MEMORIA, cpuCfg->PUERTO_ESCUCHA_INTERRUPT);

    struct sockaddr clienteDispatch;
    socklen_t lenCliD = sizeof(clienteDispatch);

    pthread_t atenderInterrupciones;
    pthread_create(&atenderInterrupciones, NULL, check_interrupt, NULL); 

    aceptar_conexiones_cpu(socketEscuchaDispatch, clienteDispatch, lenCliD);
    log_info(cpuLogger, "CPU: Acepto la conexión de Dispatch");

    

    /*struct sockaddr clienteInterrupt;
    socklen_t lenCliInt = sizeof(clienteInterrupt);

    aceptar_conexiones_cpu(socketEscuchaInterrupt, clienteInterrupt, lenCliInt);
    log_info(cpuLogger, "CPU: Acepto la conexión de Interrupcion");*/

    liberar_modulo_cpu(cpuLogger, cpuCfg);

    return EXIT_SUCCESS;
}

void aceptar_conexiones_cpu(int socketEscucha, struct sockaddr cliente, socklen_t len) {
    log_info(cpuLogger, "CPU: A la escucha de nuevas conexiones en puerto %d", socketEscucha);
    for(;;) {
        cpuCfg->KERNEL_SOCKET = accept(socketEscucha, &cliente, &len);
        if(cpuCfg->KERNEL_SOCKET > 0) {
            log_info(cpuLogger, "CPU: Acepto la conexión del socket: %d", cpuCfg->KERNEL_SOCKET);
            recibir_pcb_de_kernel(cpuCfg->KERNEL_SOCKET);
        } else {
            log_error(cpuLogger, "CPU: Error al aceptar conexión: %s", strerror(errno));
        }
    }
}

void recibir_pcb_de_kernel(int socketKernelDispatch){
    void* buffer;
    log_info(cpuLogger, "CPU: Recibiendo PCB de Kernel");
    t_mensaje_tamanio *tamanio_mensaje = malloc(sizeof(t_mensaje_tamanio));
    if (recibir_tamanio_mensaje(tamanio_mensaje, socketKernelDispatch)){
        buffer = malloc(tamanio_mensaje->tamanio);
        log_info(cpuLogger, "CPU: Recibi el tamanio: %i", tamanio_mensaje->tamanio);
        if (recv(socketKernelDispatch, buffer, tamanio_mensaje->tamanio, MSG_WAITALL)) {
            t_pcb *pcb = recibir_pcb(buffer);
            log_info(cpuLogger, "CPU: Recibi el PCB con ID: %i", pcb->id);
            log_info(cpuLogger, "CPU: Recibi el PCB con Tamanio: %i", pcb->tamanio);
            if(pcb->status == EXEC){
                log_info(cpuLogger, "CPU: Recibi el PCB con Status: %s", "EXEC");
            }
            log_info(cpuLogger, "CPU: Recibi el PCB con PC: %i", pcb->programCounter);
            log_info(cpuLogger, "CPU: Recibi el PCB con Rafaga: %f", pcb->est_rafaga_actual);
            log_info(cpuLogger, "Cantidad de Instrucciones: %i", pcb->instrucciones->elements_count);
            t_instruccion* instr = list_get(pcb->instrucciones,0);
            if(instr->indicador == NO_OP){
                log_info(cpuLogger, "Instr: %i", instr->indicador);
            }
            hacer_ciclo_de_instruccion(pcb, tamanio_mensaje, socketKernelDispatch);
            free(pcb);
        }
    }
}

void mandar_pcb_a_kernel(t_pcb* pcb, t_mensaje_tamanio* bytes, int socketKernelDispatch){
    log_info(cpuLogger, "CPU: Mando el PCB a Kernel");
    if(send(socketKernelDispatch, PCB_NORMAL_RETURN, sizeof(PCB_NORMAL_RETURN), 0)){  
        void* buffer = serializar_pcb(pcb,&(bytes->tamanio));
        if (enviar_tamanio_mensaje(bytes, socketKernelDispatch)){
            log_info(cpuLogger, "CPU: Envie tamaño a Kernel de proceso %i", pcb->id);
            if (send(socketKernelDispatch, buffer, bytes->tamanio, 0)) {
                log_info(cpuLogger, "CPU: Mande el PCB a Kernel");
                free(buffer);
                //free(pcb);-> creo que no alcanza, hay q liberar la lista de instrucciones tmb
            }
        }
    }
    else{
        log_error(cpuLogger, "CPU: Error al enviar PCB a Kernel");
    }
}

void mandar_pcb_a_kernel_con_io(t_pcb* pcb, t_mensaje_tamanio* bytes, int socketKernelDispatch,uint32_t tiempoABloquearse){
    log_info(cpuLogger, "CPU: Mando el PCB con tiempo de IO a Kernel");
    if(send(socketKernelDispatch, PCB_IO_RETURN, sizeof(PCB_IO_RETURN), 0)){ 
        void* buffer = serializar_pcb(pcb,&(bytes->tamanio));
        if (enviar_tamanio_mensaje(bytes, socketKernelDispatch)){
            log_info(cpuLogger, "CPU: Envie tamaño a Kernel de proceso %i", pcb->id);
            if (send(socketKernelDispatch, buffer, bytes->tamanio, 0)) {
                log_info(cpuLogger, "CPU: Mande el PCB a Kernel");
                free(buffer);
                if(send(socketKernelDispatch, &tiempoABloquearse, sizeof(uint32_t), 0)){ //falta hacer la recepcion apropiada de esto en kernel 
                    log_info(cpuLogger, "CPU: Mande el tiempo de IO a Kernel");
                    log_info(cpuLogger, "CPU: Devolucion de PCB completada!");
                }
                else{
                    log_error(cpuLogger, "CPU: Error al enviar tiempo de bloqueo a Kernel");
                }
                //free(pcb);-> creo que no alcanza, hay q liberar la lista de instrucciones tmb
            }
        }
    }
    else{
        log_error(cpuLogger, "CPU: Error al enviar PCB a Kernel");
    }
}

void* check_interrupt(){
    while(1){
        uint32_t mensaje;
        if(recv(cpuCfg->KERNEL_INTERRUPT, &mensaje, sizeof(uint32_t), MSG_WAITALL)){
            log_info(cpuLogger, "CPU: Recibi una interrupcion");
            pthread_mutex_lock(&mutex_interrupciones);
            hayInterrupcion=1; //y que se ponga en 0 en instruccion.c cuando se haga el ciclo
            pthread_mutex_unlock(&mutex_interrupciones);
        }
    else{
        log_error(cpuLogger, "CPU: Error al recibir una interrupcion");
        }  
    }
    
}