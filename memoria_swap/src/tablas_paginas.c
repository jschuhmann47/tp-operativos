#include "tablas_paginas.h"


t_list* tablaPaginasPrimerNivel;
t_list* tablaPaginasSegundoNivel;

int tamanioPrimerTabla;
int tamanioSegundaTabla;

void inicializar_tabla_paginas(){
    tablaPaginasPrimerNivel = list_create();
    tablaPaginasSegundoNivel = list_create();
    tamanioPrimerTabla = memoria_swapCfg->PAGINAS_POR_TABLA;
    tamanioSegundaTabla = memoria_swapCfg->PAGINAS_POR_TABLA;
}

// void obtener_nro_pagina_segunda_tabla(uint32_t indice, uint32_t* nroPagina){
//     *nroPagina = indice % tamanioSegundaTabla;
// }
