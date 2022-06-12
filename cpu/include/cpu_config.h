#ifndef CPU_CONFIG_H_INCLUDED
#define CPU_CONFIG_H_INCLUDED

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "structs.h"

#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>

typedef struct {
    int KERNEL_SOCKET;
    int KERNEL_INTERRUPT;
    uint32_t ENTRADAS_TLB;
    char* REEMPLAZO_TLB;
    uint32_t RETARDO_NOOP;
    char* IP_MEMORIA;
    char* IP_CPU;
    char* PUERTO_MEMORIA;
    char* PUERTO_ESCUCHA_DISPATCH;
    char* PUERTO_ESCUCHA_INTERRUPT;
} t_cpu_config;

typedef struct{
    uint32_t entradasTlb;
    char* agloritmoReemplazo;
    uint32_t entradasDisponibles;
    t_list* direcciones;
} t_tlb;

typedef struct{
    uint32_t pagina;
    uint32_t marco;

} t_direccion;

extern t_log* cpuLogger;
extern t_cpu_config* consolaCfg;

pthread_mutex_t mutex_interrupciones;
uint32_t hayInterrupcion;

t_cpu_config* cpu_cfg_create(void);
void cpu_config_initialize(void* cpuCfg, t_config* config);
void liberar_modulo_cpu(t_log* cpuLogger, t_cpu_config* cpuCfg);
void generar_tlb(uint32_t entradasTlb, char* algoritmoReemplazo);
void handshake_a_memoria(int SOCKET_MEMORIA, uint32_t *tamanioPagina, uint32_t *paginasPorTabla);

#endif
