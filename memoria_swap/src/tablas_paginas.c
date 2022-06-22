#include "tablas_paginas.h"

int cantTablas;

void inicializar_tabla_paginas(){
    tablaPrimerNivel = list_create();
    tablasSegundoNivel = list_create();
    
    cantTablas = memoria_swapCfg->PAGINAS_POR_TABLA; //ambas tienen el mismo tamaño
    log_info(memoria_swapLogger, "Cantidad de tablas: %i",cantTablas);
    for (int i = 0; i < cantTablas; i++) {
        t_primerNivel* entradaPrimerNivel = malloc(sizeof(t_primerNivel));
        entradaPrimerNivel->indice = i;
        entradaPrimerNivel->indiceTablaSegundoNivel = -1;
        list_add(tablaPrimerNivel, entradaPrimerNivel);
    }
    log_info(memoria_swapLogger, "Inicializadas las tablas correctamente");
}


// void obtener_nro_pagina_segunda_tabla(uint32_t indice, uint32_t* nroPagina){
//     *nroPagina = indice % tamanioSegundaTabla;
// }


void reemplazar_pagina(t_marco* paginaAAgregar,t_tablaSegundoNivel* tablaSegundoNivel){
    if(strcmp(memoria_swapCfg->ALGORITMO_REEMPLAZO, "CLOCK") == 0){
        reemplazo_clock(paginaAAgregar, tablaSegundoNivel);
    }
    if(strcmp(memoria_swapCfg->ALGORITMO_REEMPLAZO, "CLOCK-M") == 0){
        reemplazo_clock_modificado(paginaAAgregar, tablaSegundoNivel);
    }
}

uint32_t agregar_a_tabla_primer_nivel(t_tablaSegundoNivel* tablaSegNv)
{
    t_primerNivel* primerNivel;
    primerNivel = list_find(tablaPrimerNivel, lugar_libre);
    //if(primerNivel != NULL){
    //t_tablaSegundoNivel* tsn = malloc(size_tabla_segundo_nivel(tablaSegundoNivell));
    //tsn = tablaSegundoNivell;
    primerNivel->indiceTablaSegundoNivel = tablaSegNv->indice;
    return primerNivel->indice; 
    //}
    
}

t_tablaSegundoNivel* crear_tabla_segundo_nivel(){
    t_tablaSegundoNivel* entrada = malloc(sizeof(t_tablaSegundoNivel));
        entrada->indice=get_siguiente_indice();
        entrada->puntero=0;
        entrada->marcos = list_create();
        for(int j = 0; j < cantTablas; j++){
            t_marco* nuevoMarco = crear_marco();
            list_add(entrada->marcos, nuevoMarco);
        }
    list_add(tablasSegundoNivel, entrada);
    return entrada; //devuelve el puntero a la tabla
}

t_marco* crear_marco(){
    t_marco* nuevoMarco = malloc(sizeof(t_marco));
    nuevoMarco->marco = -1;
    nuevoMarco->presencia = false;
    nuevoMarco->uso = false;
    nuevoMarco->modificado = false;
    return nuevoMarco;
}


uint32_t size_tabla_segundo_nivel(t_tablaSegundoNivel* tablaSegundoNivel){
    return sizeof(int)+sizeof(uint32_t)+list_size(tablaSegundoNivel->marcos)*sizeof(t_marco);
}

bool lugar_libre(t_primerNivel* filaPrimerNivel)
{
    return filaPrimerNivel->indiceTablaSegundoNivel == -1;
}

void remover_tabla_primer_nivel(uint32_t indice){
    t_primerNivel* primerNivel;
    primerNivel = list_get(tablaPrimerNivel, indice);
    t_tablaSegundoNivel* tablaSegNvALiberar = list_get(tablasSegundoNivel,primerNivel->indiceTablaSegundoNivel);
    
    list_map(tablaSegNvALiberar->marcos,liberar_marco); //revisar si esto esta bien
    //list_destroy_and_destroy_elements(tablaSegNvALiberar->marcos,free);
    primerNivel->indiceTablaSegundoNivel = -1;
    //list_remove(tablaPrimerNivel, indice);
    //free(primerNivel);
}

void liberar_marco(t_marco* marco){
    if(marco->presencia){
        marco->presencia=0;
        marco->marco=-1;
        marcar_marco_libre(marco->marco);
    }
}


void procesar_entrada_tabla_primer_nv(int socket_cpu){
    
    log_info(memoria_swapLogger, "Memoria: Procesando entrada de tabla de primer nivel");
    uint32_t requestPrimerTabla;
    if(recv(socket_cpu, &requestPrimerTabla, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir requestPrimerTabla de CPU: %s", strerror(errno));
        exit(-1);
    }
    
    t_primerNivel* entradaTabla = list_get(tablaPrimerNivel, requestPrimerTabla);
    t_tablaSegundoNivel* tablaSegundoNivel = list_get(tablasSegundoNivel,entradaTabla->indiceTablaSegundoNivel);
    uint32_t indiceSegundoNivel = tablaSegundoNivel->indice;

    if(send(socket_cpu,&indiceSegundoNivel,sizeof(uint32_t),0) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al enviar indiceSegundoNivel a CPU: %s", strerror(errno));
        exit(-1);
    }
}

void procesar_entrada_tabla_segundo_nv(int socket_cpu){
    log_info(memoria_swapLogger, "Memoria: Procesando entrada de tabla de segundo nivel");
    
    uint32_t nroTabla;
    if(recv(socket_cpu, &nroTabla, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir nroTabla de CPU: %s", strerror(errno));
        exit(-1);
    }

    t_tablaSegundoNivel* tablaSegundoNivel = list_get(tablasSegundoNivel, nroTabla); //encuentra la tabla correspondiente
    
    uint32_t nroPagina;
    if(recv(socket_cpu, &nroPagina, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir nroPagina de CPU: %s", strerror(errno));
        exit(-1);
    }

    t_marco* marcoEncontrado = list_get(tablaSegundoNivel->marcos, nroPagina); //encuentra marco correspondiente
    uint32_t nroMarco = marcoEncontrado->marco;
    if(!marcoEncontrado->presencia){
        nroMarco = conseguir_marco_libre(tablaSegundoNivel, nroPagina); //esta por dentro va a reemplazar de ser necesario, y/o mandar a swap
        marcar_marco_ocupado(nroMarco);
    }
    

    if(send(socket_cpu,&nroMarco,sizeof(uint32_t),0) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al enviar marco a CPU: %s", strerror(errno));
        exit(-1);
    }

}

int conseguir_marco_libre(t_tablaSegundoNivel* tablaSegundoNivel, uint32_t indicePagina){ //busca un lugar libre y sino reemplaza segun algoritmo
    int marcoLibre = encontrar_marco_libre();
    if(marcoLibre == -1){
        t_marco* nuevoMarco = malloc(sizeof(t_marco)); //el nro de marco va a ser el de la victima
        nuevoMarco->presencia=true;
        nuevoMarco->uso=true;
        if(strcmp(memoria_swapCfg->ALGORITMO_REEMPLAZO,"CLOCK")==0){
            reemplazo_clock(nuevoMarco,tablaSegundoNivel);
            marcoLibre = nuevoMarco->marco;
        }
        if(strcmp(memoria_swapCfg->ALGORITMO_REEMPLAZO,"CLOCK-M")==0){
            reemplazo_clock_modificado(nuevoMarco,tablaSegundoNivel);
            marcoLibre = nuevoMarco->marco;
        }
        //ver cuándo tiene que swapear y qué TODO !!
        return marcoLibre;
    }
    else{
        t_marco* m = list_get(tablaSegundoNivel->marcos,indicePagina);
        m->marco=marcoLibre;
        m->presencia=true;
        m->uso=true;
        return marcoLibre;
    }
}

// void cargar_marco_en_memoria()
