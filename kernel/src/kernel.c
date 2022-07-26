#include "kernel.h"

int main(int argc, char* argv[]) {
    if(argc<3){
        printf("Error. Faltan argumentos.\n");
        return EXIT_FAILURE;
    }
    
    kernelCfg = kernel_cfg_create();
    kernelLogger = log_create(KERNEL_LOG_DEST, KERNEL_MODULE_NAME, true, determinar_nivel_de_log(argv[1]));
    cargar_configuracion(KERNEL_MODULE_NAME, kernelCfg, argv[2], kernelLogger, kernel_config_initialize);

    int socketEscucha = iniciar_servidor(kernelCfg->IP_KERNEL, kernelCfg->PUERTO_ESCUCHA);
    
    struct sockaddr cliente;
    socklen_t len = sizeof(cliente);

    iniciar_planificacion(); //esta se conecta a cpu

    aceptar_conexiones_kernel(socketEscucha, cliente, len);

    liberar_modulo_kernel(kernelLogger, kernelCfg);

    return EXIT_SUCCESS;
}

void aceptar_conexiones_kernel(int socketEscucha, struct sockaddr cliente, socklen_t len) {
    log_info(kernelLogger, "Kernel: A la escucha de nuevas conexiones en puerto %d", socketEscucha);
    for(;;) {
        kernelCfg->CONSOLA_SOCKET = accept(socketEscucha, &cliente, &len); //esta mal
        if(kernelCfg->CONSOLA_SOCKET > 0) {
            log_info(kernelLogger, "Kernel: Acepto la conexión del socket: %d", kernelCfg->CONSOLA_SOCKET);
            crear_hilo_handler_conexion_entrante(kernelCfg->CONSOLA_SOCKET);
        } else {
            log_error(kernelLogger, "Kernel: Error al aceptar conexión: %s", strerror(errno));
        }
    }
}

void crear_hilo_handler_conexion_entrante(int socket) 
{
    pthread_t threadSuscripcion;
    pthread_create(&threadSuscripcion, NULL, agregar_pcb_en_cola_new, (void *) socket);
    pthread_detach(threadSuscripcion);
    return;
}