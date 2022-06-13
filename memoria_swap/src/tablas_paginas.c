#include "tablas_paginas.h"


t_list* tablaPaginasPrimerNivel;

int cantTablas;

void inicializar_tabla_paginas(){
    tablaPaginasPrimerNivel = list_create();
    cantTablas = memoria_swapCfg->PAGINAS_POR_TABLA; //ambas tienen el mismo tamaño
   
    for (int i = 0; i < cantTablas; i++) {
        t_primerNivel* entradaPrimerNivel = malloc(sizeof(t_primerNivel));
        entradaPrimerNivel->indice = i;
        
        t_tablaSegundoNivel* entrada = malloc(sizeof(t_tablaSegundoNivel));
        entrada->puntero=0;
        entrada->marcos = list_create();
        for(int j = 0; j < cantTablas; j++){
            t_segundoNivel* nuevoMarco = malloc(sizeof(t_segundoNivel));
            nuevoMarco->indice = j;
            nuevoMarco->presencia = false;
            nuevoMarco->uso = false;
            nuevoMarco->modificado = false;
            list_add(entrada->marcos, nuevoMarco);
        }
        list_add(tablaPaginasPrimerNivel, entradaPrimerNivel);
    }
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