#include "cpu.h" //"../include/cpu.h"

int main(int argc, char* argv[]) {
    cpuCfg = cpu_cfg_create();
    cpuLogger = log_create(CPU_LOG_DEST, CPU_MODULE_NAME, true, LOG_LEVEL_INFO);
    cargar_configuracion(CPU_MODULE_NAME, cpuCfg, CPU_CFG_PATH, cpuLogger, cpu_config_initialize);

    log_info(cpuLogger, "Valor de PUERTO_ESCUCHA_DISPATCH %s", cpuCfg->PUERTO_ESCUCHA_DISPATCH);
    log_info(cpuLogger, "Valor de PUERTO_ESCUCHA_INTERRUPT %s", cpuCfg->PUERTO_ESCUCHA_INTERRUPT);


    int socketEscuchaDispatch = iniciar_servidor(cpuCfg->IP_MEMORIA, cpuCfg->PUERTO_ESCUCHA_DISPATCH);

    //int socketEscuchaInterrupt = iniciar_servidor(cpuCfg->IP_MEMORIA, cpuCfg->PUERTO_ESCUCHA_INTERRUPT);

    struct sockaddr clienteDispatch;
    socklen_t lenCliD = sizeof(clienteDispatch);

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
            //t_link_element* linkPrimerInstruccion = pcb->instrucciones->head;
            //t_link_element* linkSegundaInstruccion = linkPrimerInstruccion->next;
            
            //t_instruccion* primerInstruccion = linkPrimerInstruccion->data;
            //t_instruccion* segundaInstruccion = linkSegundaInstruccion->data;
            //string_append(&primerInstruccion->indicador, segundaInstruccion->indicador);
            log_info(cpuLogger, "Indicador: %d", pcb->instrucciones->head);
            hacer_ciclo_de_instruccion(pcb);
            free(pcb);
        }
    }
}

void mandar_pcb_a_kernel(t_pcb* pcb, t_mensaje_tamanio* bytes, int socketKernelDispatch){
    log_info(cpuLogger, "CPU: Mando el PCB a Kernel");
    if(send(socketKernelDispatch, PCB_NORMAL_RETURN, sizeof(PCB_NORMAL_RETURN), 0)){  
        char* buffer = serializar_pcb(pcb,bytes->tamanio);
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
        char* buffer = serializar_pcb(pcb,bytes->tamanio);
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

void* check_interrupt(){ //diria de hacer esto un hilo, y que edite un flag global: hayInterrupcion que cambie entre true y false
    while(1){
      if(recv(cpuCfg->KERNEL_INTERRUPT, &hayInterrupcion, sizeof(uint32_t), MSG_WAITALL)){
        log_info(cpuLogger, "CPU: Recibi una interrupcion");
        pthread_mutex_lock(&mutex_interrupciones);
        hayInterrupcion=1; //y que se ponga en 0 en instruccion.c cuando se haga el ciclo
        pthread_mutex_unlock(&mutex_interrupciones);
        return NULL;
    }
    else{
        log_error(cpuLogger, "CPU: Error al recibir una interrupcion");
        return NULL;
    }  
    }
    
}