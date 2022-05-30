#ifndef CPU_CONFIG_H_INCLUDED
#define CPU_CONFIG_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>

typedef struct {
    uint32_t KERNEL_SOCKET;
    uint32_t KERNEL_INTERRUPT;
    uint16_t ENTRADAS_TLB;
    char* REEMPLAZO_TLB;
    uint16_t RETARDO_NOOP;
    char* IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char* PUERTO_ESCUCHA_DISPATCH;
    char* PUERTO_ESCUCHA_INTERRUPT;
} t_cpu_config;

extern t_log* cpuLogger;
extern t_cpu_config* consolaCfg;

pthread_mutex_t mutex_interrupciones;
uint32_t hayInterrupcion;

t_cpu_config* cpu_cfg_create(void);
void cpu_config_initialize(void* cpuCfg, t_config* config);
void liberar_modulo_cpu(t_log* cpuLogger, t_cpu_config* cpuCfg);

#endif
