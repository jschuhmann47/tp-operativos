#include "cpu_config.h"

t_cpu_config* cpu_cfg_create(void) {
    t_cpu_config* newcpuCfg = malloc(sizeof(t_cpu_config));
    newcpuCfg->KERNEL_SOCKET = 0;
    newcpuCfg->ENTRADAS_TLB = 0;
    newcpuCfg->REEMPLAZO_TLB = NULL;
    newcpuCfg->RETARDO_NOOP = 0;
    newcpuCfg->IP_MEMORIA = NULL;
    newcpuCfg->IP_CPU = NULL;
    newcpuCfg->PUERTO_MEMORIA = NULL;
    newcpuCfg->PUERTO_ESCUCHA_DISPATCH = NULL;
    newcpuCfg->PUERTO_ESCUCHA_INTERRUPT = NULL;
    return newcpuCfg;
}

void cpu_config_initialize(void* cpuCfg, t_config* config) {
    t_cpu_config* cfg = (t_cpu_config*) cpuCfg;
    cfg->ENTRADAS_TLB = config_get_int_value(config, "ENTRADAS_TLB");
    cfg->REEMPLAZO_TLB = strdup(config_get_string_value(config, "REEMPLAZO_TLB"));
    cfg->RETARDO_NOOP = config_get_int_value(config, "RETARDO_NOOP");
    cfg->IP_MEMORIA = strdup(config_get_string_value(config, "IP_MEMORIA"));
    cfg->IP_CPU = strdup(config_get_string_value(config, "IP_CPU"));
    cfg->PUERTO_MEMORIA = strdup(config_get_string_value(config, "PUERTO_MEMORIA"));
    cfg->PUERTO_ESCUCHA_DISPATCH = strdup(config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH"));
    cfg->PUERTO_ESCUCHA_INTERRUPT = strdup(config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT"));
}

void liberar_modulo_cpu(t_log* cpuLogger, t_cpu_config* cpuCfg) {
    log_destroy(cpuLogger);
    close(cpuCfg->KERNEL_SOCKET);
    free(cpuCfg->REEMPLAZO_TLB);
    free(cpuCfg->IP_MEMORIA);
    free(cpuCfg->IP_CPU);
    free(cpuCfg->PUERTO_MEMORIA);
    free(cpuCfg->PUERTO_ESCUCHA_DISPATCH);
    free(cpuCfg->PUERTO_ESCUCHA_INTERRUPT);
    free(cpuCfg);
}

void generar_tlb(uint32_t entradasTlb, char* algoritmoReemplazo)
{
    t_tlb* tlb = malloc(sizeof(t_tlb));
    tlb->entradasTlb = entradasTlb;
    tlb->agloritmoReemplazo = algoritmoReemplazo;
    tlb->entradasDisponibles = entradasTlb;
    tlb->direcciones = list_create();
}