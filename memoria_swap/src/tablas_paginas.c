#include "tablas_paginas.h"

int cantTablas;

void inicializar_tabla_paginas(){
    tablasPrimerNivel = list_create();
    tablasSegundoNivel = list_create();
    marcosAsignadosPorProceso = list_create();
    
    cantTablas = memoria_swapCfg->PAGINAS_POR_TABLA; //ambas tienen el mismo tamaño
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
    t_marcoAsignado* marcosAsignadosAlProceso = malloc(sizeof(t_marcoAsignado));
    marcosAsignadosAlProceso->pid = pid;
    marcosAsignadosAlProceso->marcosAsignados = list_create();
    list_add(marcosAsignadosPorProceso, marcosAsignadosAlProceso);
    list_add(tablasPrimerNivel, tablaPrimerNv);
    tablaPrimerNv->pid = pid;
    return tablaPrimerNv;
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
        entrada->puntero=0;
        entrada->marcos = list_create();
        for(int j = 0; j < cantTablas; j++){
            t_marco* nuevoMarco = crear_marco();
            list_add(entrada->marcos, nuevoMarco);
        }
    entrada->pid = pid; //chequear si esto esta bien
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


uint32_t size_tabla_segundo_nivel(t_tablaSegundoNivel* tablaSegundoNivel){
    return sizeof(int)+sizeof(uint32_t)+list_size(tablaSegundoNivel->marcos)*sizeof(t_marco);
}

uint32_t obtener_indice_tabla_primer_nivel(t_tablaPrimerNivel* tabla){
    for(int i = 0; i < list_size(tablasPrimerNivel); i++){
        if(tabla == list_get(tablasPrimerNivel, i)){
            return i;
        }
    }
    exit(-1);
}



void liberar_marcos(uint32_t indice){
    t_tablaPrimerNivel* primerNivel;
    primerNivel = list_get(tablasPrimerNivel, indice);
    for(int i = 0; i < list_size(primerNivel->entradasPrimerNivel); i++){
        t_entradaPrimerNivel* entradaPrimerNv = list_get(primerNivel->entradasPrimerNivel,i);
        t_tablaSegundoNivel* tablaSegNvALiberar = list_get(tablasSegundoNivel,entradaPrimerNv->indiceTablaSegundoNivel);
        list_map(tablaSegNvALiberar->marcos,liberar_marco);
    }
    //primerNivel->indiceTablaSegundoNivel = -1;
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
    uint32_t nroTablaPrimerNv;
    if(recv(socket_cpu, &nroTablaPrimerNv, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir requestPrimerTabla de CPU: %s", strerror(errno));
        exit(-1);
    }

    t_tablaPrimerNivel* tablaPrimerNv = list_get(tablasPrimerNivel, nroTablaPrimerNv);

    log_info(memoria_swapLogger, "Memoria: Recibi nroTablaPrimerNv %i",nroTablaPrimerNv);

    uint32_t requestPrimerTabla;
    if(recv(socket_cpu, &requestPrimerTabla, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir requestPrimerTabla de CPU: %s", strerror(errno));
        exit(-1);
    }
    //hay que recibir el indice de la tabla de primer nivel, y luego la entrada
    log_info(memoria_swapLogger, "Memoria: Llego requestPrimerTabla %i",requestPrimerTabla);

    t_entradaPrimerNivel* entrada = list_get(tablaPrimerNv->entradasPrimerNivel, requestPrimerTabla);
    uint32_t indiceSegundoNivel = entrada->indiceTablaSegundoNivel;

    log_info(memoria_swapLogger, "Memoria: INDICE %i",indiceSegundoNivel);
    int bytes;
    if((bytes=send(socket_cpu,&indiceSegundoNivel,sizeof(uint32_t),0)) == -1){ //aca se rompe
        log_error(memoria_swapLogger, "Memoria: Error al enviar indiceSegundoNivel a CPU: %s", strerror(errno));
        exit(-1);
    }
    log_info(memoria_swapLogger, "Memoria: bytes de indice %i",bytes);
}

void procesar_entrada_tabla_segundo_nv(int socket_cpu){
    log_info(memoria_swapLogger, "Memoria: Procesando entrada de tabla de segundo nivel");
    
    uint32_t nroTabla;
    if(recv(socket_cpu, &nroTabla, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir nroTabla de CPU: %s", strerror(errno));
        exit(-1);
    }
    log_info(memoria_swapLogger, "Memoria: Procesando entrada de tabla de segundo nivel RECIBI %i",nroTabla);
    
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
    
    t_marcoAsignado* marcosAsig = buscar_marcos_asignados_al_proceso(*(tablaSegundoNivel->pid)); // lo apuntado por el puntero
    
    int marcoLibre;
    if(list_size(marcosAsig->marcosAsignados) == memoria_swapCfg->MARCOS_POR_PROCESO){
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
        return marcoLibre;
    }else{
        marcoLibre = encontrar_marco_libre();
        t_marco* m = list_get(tablaSegundoNivel->marcos,indicePagina);
        m->marco=marcoLibre;
        m->presencia=true;
        m->uso=true;
        return marcoLibre;
    }

    
    
}

t_marcoAsignado* buscar_marcos_asignados_al_proceso(uint32_t pid){
    return list_get(marcosAsignadosPorProceso, pid-1); //el pid empezaba en 1 no? si esto funca se puede sacar pid del struct
}
