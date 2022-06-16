#include "manejo_memoria.h"

t_list* marcosLibres;

void* crear_espacio_de_memoria()
{
    return malloc(memoria_swapCfg->TAM_MEMORIA);
}

void escribir_en_memoria(void* memoria, void* contenido, uint32_t marco, uint32_t desplazamiento,int size){ //valido si el marco esta libre antes de llamar a esto
    uint32_t offset = marco * memoria_swapCfg->TAM_PAGINA + desplazamiento;
    memcpy(memoria + offset, contenido, size);
    marcar_marco_ocupado(marco);

}

void* leer_de_memoria(void* memoria, uint32_t marco, uint32_t desplazamiento, int size)
{
    void* contenido = malloc(size);
    memcpy(contenido, memoria + marco * memoria_swapCfg->TAM_PAGINA + desplazamiento, size);
    return contenido;
}


void inicializar_marcos(){
    marcosLibres = list_create();
    
    int cantMarcos=10; //Ver de donde sale este valor   
    for(int i = 0; i < cantMarcos; i++){
        t_marco* marco=malloc(sizeof(t_marco));
        marco->nroMarco=i;
        marco->estaLibre=true;
        list_add(marcosLibres, i);
    }

}

bool marco_libre(uint32_t marco){
    
    t_marco* encontrado = list_find(marcosLibres, marco);
    if(encontrado != NULL){
        return encontrado->estaLibre;
    }
    return false;

}

void marcar_marco_ocupado(uint32_t marco){
    t_marco* encontrado = list_find(marcosLibres, marco);
    if(encontrado != NULL){
        encontrado->estaLibre=false;
    }
}

void marcar_marco_libre(uint32_t marco){
    t_marco* encontrado = list_find(marcosLibres, marco);
    if(encontrado != NULL){
        encontrado->estaLibre=true;
    }
}