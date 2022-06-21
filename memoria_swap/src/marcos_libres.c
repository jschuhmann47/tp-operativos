#include "marcos_libres.h"

void inicializar_lista_marcos_libres(){
    marcosLibres = list_create();
    int cantidadMarcosMemoria = memoria_swapCfg->TAM_MEMORIA/memoria_swapCfg->TAM_PAGINA;
    for (int i = 0; i < cantidadMarcosMemoria; i++){
        crear_marco_libre(i);
    }
}

void crear_marco_libre(uint32_t nroMarco){
    t_marco_libre* marco=malloc(sizeof(t_marco_libre));
    marco->nroMarco=nroMarco;
    marco->estaLibre=true;
    list_add(marcosLibres, marco);
}

bool marco_esta_libre(uint32_t marco){
    
    t_marco_libre* encontrado = encontrar_marco_en_lista_libre(marco);
    if(encontrado != NULL){
        return encontrado->estaLibre;
    }
    return false;

}

void marcar_marco_ocupado(uint32_t marco){
    t_marco_libre* encontrado = encontrar_marco_en_lista_libre(marco);
    if(encontrado != NULL){
        encontrado->estaLibre=false;
    }
}

void marcar_marco_libre(uint32_t marco){
    t_marco_libre* encontrado = encontrar_marco_en_lista_libre(marco);
    if(encontrado != NULL){
        encontrado->estaLibre=true;
    }
}

t_marco_libre* encontrar_marco_en_lista_libre(uint32_t marcoABuscar){
    int cantMarcos = list_size(marcosLibres);
    for(int i = 0; i < cantMarcos; i++){
        t_marco_libre* marco = list_get(marcosLibres, i);
        if(marcoABuscar == marco->nroMarco){
            return marco;
        }
    }
    return NULL;
}

int encontrar_marco_libre(){
    int cantMarcos = list_size(marcosLibres);
    for(int i = 0; i < cantMarcos; i++){
        t_marco_libre* marco = list_get(marcosLibres, i);
        if(marco->estaLibre){
            return marco->nroMarco;
        }
    }
    return -1;
}