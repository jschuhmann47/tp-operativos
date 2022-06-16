#include "tablas_paginas.h"

int cantTablas;

t_list* marcosLibres;

void inicializar_tabla_paginas(){
    tablaPaginasPrimerNivel = list_create();
    cantTablas = memoria_swapCfg->PAGINAS_POR_TABLA; //ambas tienen el mismo tamaño
    log_info(memoria_swapLogger, "Cantidad de tablas: %i",cantTablas);
    for (int i = 0; i < cantTablas; i++) {
        t_primerNivel* entradaPrimerNivel = malloc(sizeof(t_primerNivel));
        entradaPrimerNivel->indice = i;
        entradaPrimerNivel->tablaSegundoNivel = NULL;
        //t_tablaSegundoNivel* entrada = malloc(sizeof(t_tablaSegundoNivel));
        /*entrada->puntero=0;
        entrada->marcos = list_create();
        for(int j = 0; j < cantTablas; j++){
            t_segundoNivel* nuevoMarco = malloc(sizeof(t_segundoNivel));
            nuevoMarco->indice = j;
            nuevoMarco->presencia = false;
            nuevoMarco->uso = false;
            nuevoMarco->modificado = false;
            list_add(entrada->marcos, nuevoMarco);
        }*/
        list_add(tablaPaginasPrimerNivel, entradaPrimerNivel);
    }
    log_info(memoria_swapLogger, "Inicializadas las tablas correctamente");
}


// void obtener_nro_pagina_segunda_tabla(uint32_t indice, uint32_t* nroPagina){
//     *nroPagina = indice % tamanioSegundaTabla;
// }


void reemplazar_pagina(t_segundoNivel* paginaAAgregar,t_tablaSegundoNivel* tablaSegundoNivel){
    if(strcmp(memoria_swapCfg->ALGORITMO_REEMPLAZO, "CLOCK") == 0){
        reemplazo_clock(paginaAAgregar, tablaSegundoNivel);
    }
    if(strcmp(memoria_swapCfg->ALGORITMO_REEMPLAZO, "CLOCK-M") == 0){
        reemplazo_clock_modificado(paginaAAgregar, tablaSegundoNivel);
    }
}

uint32_t agregar_a_tabla_primer_nivel(t_tablaSegundoNivel* tablaSegundoNivel)
{
    t_primerNivel* primerNivel;
    primerNivel = list_find(tablaPaginasPrimerNivel, lugar_libre);
    primerNivel->tablaSegundoNivel = tablaSegundoNivel;
    return primerNivel->indice;
}

bool lugar_libre(t_primerNivel* filaPrimerNivel)
{
    return filaPrimerNivel->tablaSegundoNivel == NULL;
}


void procesar_entrada_tabla_primer_nv(int socket_cpu){
    
    log_info(memoria_swapLogger, "Memoria: Procesando entrada de tabla de primer nivel");
    uint32_t requestPrimerTabla;
    if(recv(socket_cpu, &requestPrimerTabla, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir requestPrimerTabla de CPU: %s", strerror(errno));
        //return;
    }
    
    //va a la tabla y trae el numero de la segunda tabla

    uint32_t indiceSegundaTabla = 1;
    if(send(socket_cpu,&indiceSegundaTabla,sizeof(uint32_t),0) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al enviar indiceSegundaTabla a CPU: %s", strerror(errno));
        //return;
    }
}

void procesar_entrada_tabla_segundo_nv(int socket_cpu){
    log_info(memoria_swapLogger, "Memoria: Procesando entrada de tabla de segundo nivel");
    uint32_t requestSegundaTabla;
    if(recv(socket_cpu, &requestSegundaTabla, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir requestSegundaTabla de CPU: %s", strerror(errno));
        //return;
    }

    //busca el marco en la tabla de segunda nivel

    uint32_t marco = 1;
    if(send(socket_cpu,&marco,sizeof(uint32_t),0) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al enviar marco a CPU: %s", strerror(errno));
        //return;
    }

}