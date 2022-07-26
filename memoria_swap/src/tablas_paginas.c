#include "tablas_paginas.h"

int cantTablas;

void inicializar_tabla_paginas(){
    tablasPrimerNivel = list_create();
    tablasSegundoNivel = list_create();
    marcosAsignadosPorProceso = list_create();
    
    cantTablas = memoria_swapCfg->PAGINAS_POR_TABLA;
    log_info(memoria_swapLogger, "Inicializadas las tablas correctamente");
}

t_tablaPrimerNivel* crear_tabla_primer_nivel(uint32_t pid){

    t_tablaPrimerNivel* tablaPrimerNv = malloc(sizeof(t_tablaPrimerNivel));
    tablaPrimerNv->entradasPrimerNivel = list_create();
    tablaPrimerNv->nroTabla = get_siguiente_indice_primer_nivel(); 
    for (int i = 0; i < cantTablas; i++) {
        t_entradaPrimerNivel* entradaPrimerNivel = malloc(sizeof(t_entradaPrimerNivel));
        entradaPrimerNivel->indice = i;
        t_tablaSegundoNivel* tablaSegNv = crear_tabla_segundo_nivel(pid);
        entradaPrimerNivel->indiceTablaSegundoNivel = tablaSegNv->indice;
        list_add(tablaPrimerNv->entradasPrimerNivel, entradaPrimerNivel);
    }
    t_marcosAsignadoPorProceso* marcosAsignadosAlProceso = malloc(sizeof(t_marcosAsignadoPorProceso));
    marcosAsignadosAlProceso->pid = pid;
    marcosAsignadosAlProceso->marcosAsignados = list_create();
    marcosAsignadosAlProceso->puntero = 0;
    list_add(marcosAsignadosPorProceso, marcosAsignadosAlProceso);
    list_add(tablasPrimerNivel, tablaPrimerNv);
    tablaPrimerNv->pid = pid;
    return tablaPrimerNv;
}


uint32_t agregar_a_tabla_primer_nivel(t_tablaPrimerNivel* tablaPrimerNv, t_tablaSegundoNivel* tablaSegNv)
{
    t_entradaPrimerNivel* primerNivel;
    primerNivel = list_find(tablaPrimerNv->entradasPrimerNivel, lugar_libre);
    primerNivel->indiceTablaSegundoNivel = tablaSegNv->indice;
    log_info(memoria_swapLogger, "Memoria: asigno %i - %i",primerNivel->indice,primerNivel->indiceTablaSegundoNivel);
    return primerNivel->indice;     
}

bool lugar_libre(t_entradaPrimerNivel* filaPrimerNivel)
{
    return filaPrimerNivel->indiceTablaSegundoNivel == -1;
}

t_tablaSegundoNivel* crear_tabla_segundo_nivel(uint32_t pid){
    t_tablaSegundoNivel* entrada = malloc(sizeof(t_tablaSegundoNivel));
        entrada->indice=get_siguiente_indice_segundo_nivel();
        //entrada->puntero=0;
        entrada->marcos = list_create();
        for(int j = 0; j < cantTablas; j++){
            t_marco* nuevoMarco = crear_marco();
            list_add(entrada->marcos, nuevoMarco);
        }
    entrada->pid = pid;
    list_add(tablasSegundoNivel, entrada);
    return entrada;
}

t_marco* crear_marco(){
    t_marco* nuevoMarco = malloc(sizeof(t_marco));
    nuevoMarco->marco = -1;
    nuevoMarco->presencia = false;
    nuevoMarco->uso = false;
    nuevoMarco->modificado = false;
    return nuevoMarco;
}



uint32_t obtener_indice_tabla_primer_nivel(t_tablaPrimerNivel* tabla){
    for(int i = 0; i < list_size(tablasPrimerNivel); i++){
        if(tabla == list_get(tablasPrimerNivel, i)){
            return i;
        }
    }
    log_error(memoria_swapLogger, "Memoria: no se pudo obtener el indice de la tabla de primer nivel");
    exit(-1);
}



void liberar_marcos(uint32_t indice){
    t_tablaPrimerNivel* primerNivel;
    primerNivel = list_get(tablasPrimerNivel, indice);
    for(int i = 0; i < list_size(primerNivel->entradasPrimerNivel); i++){
        t_entradaPrimerNivel* entradaPrimerNv = list_get(primerNivel->entradasPrimerNivel,i);
        t_tablaSegundoNivel* tablaSegNvALiberar = list_get(tablasSegundoNivel,entradaPrimerNv->indiceTablaSegundoNivel);
        list_destroy_and_destroy_elements(tablaSegNvALiberar->marcos,free_lista_marco);
    }
}

void free_lista_marco(t_marco* m){
    
    liberar_marco(m);
    free(m);
}

void liberar_marco(t_marco* marco){
    if(marco->presencia){
        marco->presencia=false;
        marcar_marco_libre(marco->marco);
        marco->marco=-1;
    }
}
