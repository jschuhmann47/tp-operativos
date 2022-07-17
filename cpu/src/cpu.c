#include "cpu.h"

int SOCKET_MEMORIA;

int main(int argc, char* argv[]) {
    cpuCfg = cpu_cfg_create();
    cpuLogger = log_create(CPU_LOG_DEST, CPU_MODULE_NAME, true, LOG_LEVEL_INFO);
    cargar_configuracion(CPU_MODULE_NAME, cpuCfg, CPU_CFG_PATH, cpuLogger, cpu_config_initialize);
    generar_tlb(cpuCfg->ENTRADAS_TLB, cpuCfg->REEMPLAZO_TLB);
    log_info(cpuLogger, "CPU: TLB inicializada");

    int socketEscuchaDispatch = iniciar_servidor(cpuCfg->IP_CPU, cpuCfg->PUERTO_ESCUCHA_DISPATCH);

    int socketEscuchaInterrupt = iniciar_servidor(cpuCfg->IP_CPU, cpuCfg->PUERTO_ESCUCHA_INTERRUPT);
    
    SOCKET_MEMORIA = conectar_a_servidor(cpuCfg->IP_MEMORIA, cpuCfg->PUERTO_MEMORIA);
    log_info(cpuLogger, "CPU: Conectando a Memoria");

    if (SOCKET_MEMORIA == -1)
    {
        log_error(cpuLogger, "CPU: No se pudo establecer conexión con Memoria. Valor conexión %d", SOCKET_MEMORIA);
        exit(-1);
    }

    log_info(cpuLogger, "CPU: Realizando Handshake a Memoria");
    handshake_a_memoria(SOCKET_MEMORIA, &tamanioPagina, &paginasPorTabla);
    log_debug(cpuLogger, "CPU: Recibi tamanio de pagina: %i", tamanioPagina);
    log_debug(cpuLogger, "CPU: Recibi paginas por tabla: %i", paginasPorTabla);

    struct sockaddr clienteDispatch;
    socklen_t lenCliD = sizeof(clienteDispatch);

    conexion* conexionDispatch = malloc(sizeof(conexion));
    conexion* conexionInterrupt= malloc(sizeof(conexion));

    conexionDispatch->socket=socketEscuchaDispatch;
    conexionDispatch->sockAddr=clienteDispatch;
    conexionDispatch->sockrAddrLen=lenCliD;

    conexionInterrupt->socket=socketEscuchaInterrupt;
    conexionInterrupt->sockAddr=clienteDispatch;
    conexionInterrupt->sockrAddrLen=lenCliD;

    pthread_t atenderDispatch;
    pthread_create(&atenderDispatch, NULL, aceptar_conexiones_cpu, conexionDispatch);
    
    pthread_t atenderInterrupcion;
    pthread_create(&atenderInterrupcion, NULL, aceptar_conexiones_cpu_interrupcion, conexionInterrupt);

    pthread_join(atenderDispatch, NULL);
    pthread_join(atenderInterrupcion, NULL);

    liberar_modulo_cpu(cpuLogger, cpuCfg);

    return EXIT_SUCCESS;
}

void aceptar_conexiones_cpu(conexion* conexion) {
    log_info(cpuLogger, "CPU: A la escucha de nuevas conexiones en puerto %d", conexion->socket);
    for(;;) {
        cpuCfg->KERNEL_SOCKET = accept(conexion->socket, &(conexion->sockAddr), &(conexion->sockrAddrLen));
        if(cpuCfg->KERNEL_SOCKET > 0) {
            log_info(cpuLogger, "CPU: Acepto la conexión del socket: %d", cpuCfg->KERNEL_SOCKET);
            recibir_pcb_de_kernel(cpuCfg->KERNEL_SOCKET);
        } else {
            log_error(cpuLogger, "CPU: Error al aceptar conexión: %s", strerror(errno));
        }
    }
}


void aceptar_conexiones_cpu_interrupcion(conexion* conexion) {
    log_info(cpuLogger, "CPU: A la escucha de nuevas conexiones en puerto interrupt %d", conexion->socket);
    for(;;) {
        cpuCfg->KERNEL_INTERRUPT = accept(conexion->socket, &(conexion->sockAddr), &(conexion->sockrAddrLen));
        if(cpuCfg->KERNEL_INTERRUPT > 0) {
            log_info(cpuLogger, "CPU: Acepto la conexión del socket: %d", cpuCfg->KERNEL_INTERRUPT);
            check_interrupt();
        } else {
            log_error(cpuLogger, "CPU: Error al aceptar conexión interrupt: %s", strerror(errno));
        }
    }
}

void recibir_pcb_de_kernel(int socketKernelDispatch){
    while(1){
        void* buffer;
        log_info(cpuLogger, "CPU: Esperando PCB de Kernel");
        t_mensaje_tamanio *tamanio_mensaje = malloc(sizeof(t_mensaje_tamanio));
        if (recibir_tamanio_mensaje(tamanio_mensaje, socketKernelDispatch)){
            buffer = malloc(tamanio_mensaje->tamanio);
            log_debug(cpuLogger, "CPU: Recibi el tamanio: %i", tamanio_mensaje->tamanio);
            if (recv(socketKernelDispatch, buffer, tamanio_mensaje->tamanio, MSG_WAITALL)) {
                t_pcb *pcb = recibir_pcb(buffer, tamanio_mensaje->tamanio);
                log_info(cpuLogger, "CPU: Recibi el PCB con ID: %i", pcb->id);
                hacer_ciclo_de_instruccion(pcb, tamanio_mensaje, socketKernelDispatch, SOCKET_MEMORIA);
            }
        }  
    }
   
}

void mandar_pcb_a_kernel(t_pcb* pcb, t_mensaje_tamanio* bytes, int socketKernelDispatch){
    log_info(cpuLogger, "CPU: Mandando el PCB a Kernel"); 
    uint32_t bytesPcb=0;
    void* buffer = serializar_pcb(pcb,&bytesPcb);
    bytes->tamanio=bytesPcb;
    if (enviar_tamanio_mensaje(bytes, socketKernelDispatch)){
        log_debug(cpuLogger, "CPU: Envie tamaño a Kernel de proceso %i", pcb->id);
        if (send(socketKernelDispatch, buffer, bytes->tamanio, 0)) {
            log_info(cpuLogger, "CPU: Devolucion de PCB completada!");
            free(buffer);
            pcb_destroy(pcb);
        }
    }
    else{
        log_error(cpuLogger, "CPU: Error al enviar PCB a Kernel");
        exit(-1);
    }
}

void mandar_pcb_a_kernel_con_io(t_pcb* pcb, t_mensaje_tamanio* bytes, int socketKernelDispatch,uint32_t tiempoABloquearse){
    log_info(cpuLogger, "CPU: Mandando el PCB con tiempo de IO a Kernel");
    uint32_t bytesPcb=0;
    void* buffer = serializar_pcb(pcb,&bytesPcb);
    bytes->tamanio=bytesPcb;
    if (enviar_tamanio_mensaje(bytes, socketKernelDispatch)){
        log_debug(cpuLogger, "CPU: Envie tamaño a Kernel de proceso %i", pcb->id);
        if (send(socketKernelDispatch, buffer, bytes->tamanio, 0)) {
            log_debug(cpuLogger, "CPU: Mande el PCB a Kernel");
            if(send(socketKernelDispatch, &tiempoABloquearse, sizeof(uint32_t), 0)){ 
                log_info(cpuLogger, "CPU: Mande el tiempo de IO a Kernel. Tiempo mandado IO %i",tiempoABloquearse);
                free(buffer);
                free(bytes);
                pcb_destroy(pcb);
            }
            else{
                log_error(cpuLogger, "CPU: Error al enviar tiempo de bloqueo a Kernel");
                exit(-1);
            }
        }
    }
    else{
        log_error(cpuLogger, "CPU: Error al enviar PCB a Kernel");
    }
}

void* check_interrupt(){
    log_info(cpuLogger,"CPU: Hilo de atenciones a interrupciones inicializado");
    while(1){
        uint32_t mensaje;
        recv(cpuCfg->KERNEL_INTERRUPT, &mensaje, sizeof(uint32_t), MSG_WAITALL);
        if(mensaje==1){
            log_warning(cpuLogger, "CPU: Recibi una interrupcion");
            pthread_mutex_lock(&mutex_interrupciones);
            hayInterrupcion=1;
            pthread_mutex_unlock(&mutex_interrupciones);
        }
    else{
        log_error(cpuLogger, "CPU: Error al recibir una interrupcion");
        exit(-1);
        }  
    }
    
}