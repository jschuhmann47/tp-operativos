#ifndef TLB_H_INCLUDED
#define TLB_H_INCLUDED

#include "cpu_config.h"

typedef struct{
    int indice;
    int contadorTiempo;
} t_entrada_lru;


void generar_tlb(uint32_t entradasTlb, char* algoritmoReemplazo);
void liberar_tlb();
int obtener_indice_traduccion_tlb(uint32_t pagina);
uint32_t obtener_traduccion_tlb(int indice);
void agregar_a_tlb(uint32_t pagina, uint32_t marco);
void agregar_traduccion_a_tabla_tlb(uint32_t pagina, uint32_t marco);
void reemplazar_tlb_lru(uint32_t pagina, uint32_t marco);
void reemplazar_tlb_fifo(uint32_t pagina, uint32_t marco);
void agregar_a_tlb_en_indice(uint32_t pagina, uint32_t marco, int indice);
int obtener_indice_victima_lru();
t_entrada_lru* comparar_lru(t_entrada_lru* a, t_entrada_lru* b);
void actualizar_tiempo_ultimo_uso_tlb(int indice);



int indiceReemplazoFifo;
t_list* reemplazoLru;
int instanteCargaLru;
#endif