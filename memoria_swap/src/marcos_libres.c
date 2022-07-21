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
        log_debug(memoria_swapLogger, "Marcando marco %i ocupado",encontrado->nroMarco);
        encontrado->estaLibre=false;
    }
}

void marcar_marco_libre(uint32_t marco){
    t_marco_libre* encontrado = encontrar_marco_en_lista_libre(marco);
    if(encontrado != NULL){
        log_debug(memoria_swapLogger, "Marcando marco %i libre",encontrado->nroMarco);
        encontrado->estaLibre=true;
    }
}

t_marco_libre* encontrar_marco_en_lista_libre(uint32_t marcoABuscar){

    for(int i = 0; i < list_size(marcosLibres); i++){
        t_marco_libre* marco = list_get(marcosLibres, i);
        if(marcoABuscar == marco->nroMarco){
            return marco;
        }
    }
    log_error(memoria_swapLogger, "No se encontro el marco %i en la lista de libres",marcoABuscar);
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

void crear_lista_marcos_asignados(uint32_t pid){
    t_marcosAsignadoPorProceso* marcoAsignado = malloc(sizeof(t_marcosAsignadoPorProceso));
    marcoAsignado->pid = pid;
    marcoAsignado->marcosAsignados = list_create();
    marcoAsignado->puntero = 0;
    list_add(marcosAsignadosPorProceso, marcoAsignado);
}
