#include "tlb.h"

void generar_tlb(uint32_t entradasTlb, char* algoritmoReemplazo)
{
    tlb = malloc(sizeof(t_tlb));
    tlb->entradasTlb = entradasTlb;
    tlb->agloritmoReemplazo = algoritmoReemplazo;
    tlb->entradasDisponibles = entradasTlb;
    tlb->direcciones = list_create();
    for(int i = 0; i<cpuCfg->ENTRADAS_TLB;i++){
        t_direccion* d = malloc(sizeof(t_direccion));
        d->pagina=0;
        d->marco=0;
        list_add(tlb->direcciones,d);
    }
}

void liberar_tlb()
{
    list_destroy_and_destroy_elements(tlb->direcciones, free);
    free(tlb);
}

void limpiar_tlb()
{
    list_destroy_and_destroy_elements(tlb->direcciones, free);
    tlb->direcciones = list_create();
    for(int i = 0; i<cpuCfg->ENTRADAS_TLB;i++){
        t_direccion* d = malloc(sizeof(t_direccion));
        d->pagina=0;
        d->marco=0;
        list_add(tlb->direcciones,d);
    }
}

int obtener_indice_traduccion_tlb(uint32_t pagina) //devuelve el indice, o -1 si no esta traducida
{
    t_direccion* d;
    for(int i = 0;i<cpuCfg->ENTRADAS_TLB;i++){
        d = list_get(tlb->direcciones, i);
        if(d==NULL){
            break;
        }
        if(d->pagina == pagina){
            //printf("La direccion esta traducida en el marco %d\n", d->marco);
            return i;
        }
    }
    return -1;
}

uint32_t obtener_traduccion_tlb(int indice){
    t_direccion* d;
    d = list_get(tlb->direcciones, indice);
    return d->marco;
}

void agregar_a_tlb(uint32_t pagina, uint32_t marco){
    t_direccion* d;
    d = malloc(sizeof(t_direccion));
    d->pagina = pagina;
    d->marco = marco;
    list_add(tlb->direcciones, d);
}

void agregar_traduccion_a_tabla_tlb(uint32_t pagina, uint32_t marco){ //El algoritmo de reemplazo, que puede ser FIFO o LRU.
    if(list_size(tlb)==cpuCfg->ENTRADAS_TLB){
        if(strcmp(cpuCfg->REEMPLAZO_TLB, "LRU")==0){
            reemplazar_tlb_lru(pagina, marco);
        }
        else if(strcmp(cpuCfg->REEMPLAZO_TLB, "FIFO")==0){
            reemplazar_tlb_fifo(pagina, marco);
        }
    }else{
        agregar_a_tlb(pagina, marco);
    }
}

void reemplazar_tlb_lru(uint32_t pagina, uint32_t marco){ //crear un timer y guardar el tiempo? mas facil para hacer los 2
    
}

void reemplazar_tlb_fifo(uint32_t pagina, uint32_t marco){
    
}