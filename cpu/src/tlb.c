#include "tlb.h"

void generar_tlb(uint32_t entradasTlb, char* algoritmoReemplazo)
{
    tlb = malloc(sizeof(t_tlb));
    tlb->entradasTlb = entradasTlb;
    tlb->agloritmoReemplazo = algoritmoReemplazo;
    tlb->entradasDisponibles = entradasTlb;
    tlb->direcciones = list_create();
    indiceReemplazoFifo=0;
    instanteCargaLru=0;
    reemplazoLru = list_create();
}

void liberar_tlb()
{
    list_destroy_and_destroy_elements(tlb->direcciones, free);
    list_destroy_and_destroy_elements(reemplazoLru, free);
    free(tlb);
    free(reemplazoLru);
}

void limpiar_tlb()
{
    list_clean_and_destroy_elements(tlb->direcciones,free);
    list_clean_and_destroy_elements(reemplazoLru,free);
}

int obtener_indice_traduccion_tlb(uint32_t pagina) //devuelve el indice, o -1 si no esta traducida
{
    t_direccion* d;
    for(int i = 0;i<list_size(tlb->direcciones);i++){
        d = list_get(tlb->direcciones, i);
        if(d==NULL){
            break;
        }
        if(d->pagina == pagina){
            log_info(cpuLogger, "TLB: La pagina %i esta traducida",pagina);
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
    if(strcmp(cpuCfg->REEMPLAZO_TLB, "LRU")==0){
        t_entrada_lru* lru = malloc(sizeof(t_entrada_lru));
        lru->indice = list_size(tlb->direcciones)-1;
        lru->contadorTiempo = instanteCargaLru;
        instanteCargaLru++;
        list_add(reemplazoLru, lru);
    }
}

void agregar_a_tlb_en_indice(uint32_t pagina, uint32_t marco, int indice){
    t_direccion* d;
    d = malloc(sizeof(t_direccion));
    d->pagina = pagina;
    d->marco = marco;
    list_add_in_index(tlb->direcciones, indice, d);
    if(strcmp(cpuCfg->REEMPLAZO_TLB, "LRU")==0){
        t_entrada_lru* lru = malloc(sizeof(t_entrada_lru));
        lru->indice = indice;
        lru->contadorTiempo = instanteCargaLru;
        instanteCargaLru++;
        list_add(reemplazoLru, lru);
    }
}

void agregar_traduccion_a_tabla_tlb(uint32_t pagina, uint32_t marco){
    if(list_size(tlb->direcciones)==cpuCfg->ENTRADAS_TLB){
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

void reemplazar_tlb_lru(uint32_t pagina, uint32_t marco){
    log_info(cpuLogger, "CPU: LRU");
    int indiceVictimaLru = obtener_indice_victima_lru();
    t_direccion* victima = list_remove(tlb->direcciones, indiceVictimaLru);
    log_debug(cpuLogger,"Se quita de la tlb indice %i",indiceVictimaLru);
    agregar_a_tlb_en_indice(pagina, marco, indiceVictimaLru);
    free(victima);
}

void reemplazar_tlb_fifo(uint32_t pagina, uint32_t marco){
    log_info(cpuLogger, "CPU: FIFO");
    t_direccion* victima = list_remove(tlb->direcciones, indiceReemplazoFifo);
    log_debug(cpuLogger,"Se quita de la tlb indice %i",indiceReemplazoFifo);
    agregar_a_tlb_en_indice(pagina, marco, indiceReemplazoFifo);
    indiceReemplazoFifo++;
    if(indiceReemplazoFifo==cpuCfg->ENTRADAS_TLB){
        indiceReemplazoFifo=0;
    }
    free(victima);
}

int obtener_indice_victima_lru(){
    t_entrada_lru* victima = list_get_minimum(reemplazoLru, comparar_lru);
    return victima->indice;
}

t_entrada_lru* comparar_lru(t_entrada_lru* a, t_entrada_lru* b){
    if(a->contadorTiempo < b->contadorTiempo){
        return a;
    }else{
        return b;
    }
}

void actualizar_tiempo_ultimo_uso_tlb(int indice){
    t_entrada_lru* entrada = list_get(reemplazoLru, indice);
    entrada->contadorTiempo = instanteCargaLru;
    instanteCargaLru++;
}
