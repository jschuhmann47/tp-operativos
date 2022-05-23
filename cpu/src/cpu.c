#include "cpu.h"

int main(int argc, char* argv[]) {
    cpuCfg = cpu_cfg_create();
    cpuLogger = log_create(CPU_LOG_DEST, CPU_MODULE_NAME, true, LOG_LEVEL_INFO);
    cargar_configuracion(CPU_MODULE_NAME, cpuCfg, CPU_CFG_PATH, cpuLogger, cpu_config_initialize);

    log_info(cpuLogger, "Valor de PUERTO_ESCUCHA_DISPATCH %s", cpuCfg->PUERTO_ESCUCHA_DISPATCH);

    int socketEscuchaDispatch = iniciar_servidor(cpuCfg->IP_MEMORIA, cpuCfg->PUERTO_ESCUCHA_DISPATCH);

    int socketEscuchaInterrupt = iniciar_servidor(cpuCfg->IP_MEMORIA, cpuCfg->PUERTO_ESCUCHA_INTERRUPT);

    struct sockaddr clienteDispatch;
    socklen_t lenCliD = sizeof(clienteDispatch);

    aceptar_conexiones_cpu(socketEscuchaDispatch, clienteDispatch, lenCliD);
    log_info(cpuLogger, "CPU: Acepto la conexión de Dispatch");

    struct sockaddr clienteInterrupt;
    socklen_t lenCliInt = sizeof(clienteInterrupt);

    aceptar_conexiones_cpu(socketEscuchaInterrupt, clienteInterrupt, lenCliInt);
    log_info(cpuLogger, "CPU: Acepto la conexión de Interrupcion");

    liberar_modulo_cpu(cpuLogger, cpuCfg);

    return EXIT_SUCCESS;
}

void aceptar_conexiones_cpu(int socketEscucha, struct sockaddr cliente, socklen_t len) {
    log_info(cpuLogger, "CPU: A la escucha de nuevas conexiones en puerto %d", socketEscucha);
    for(;;) {
        cpuCfg->KERNEL_SOCKET = accept(socketEscucha, &cliente, &len);
        if(cpuCfg->KERNEL_SOCKET > 0) {
            log_info(cpuLogger, "CPU: Acepto la conexión del socket: %d", cpuCfg->KERNEL_SOCKET);
        } else {
            log_error(cpuLogger, "CPU: Error al aceptar conexión: %s", strerror(errno));
        }
    }
}