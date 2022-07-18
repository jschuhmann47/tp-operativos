#include "suspension.h"


void suspender_proceso(uint32_t indice, uint32_t pid){
    log_info(memoria_swapLogger,"Memoria: Pasando a SWAP proceso de id %i",pid);
    t_tablaPrimerNivel* tablaALiberar = list_get(tablasPrimerNivel,indice);
    t_entradaPrimerNivel* entradaPrimerNivel;
    for(int i=0; i<list_size(tablaALiberar->entradasPrimerNivel);i++){
        entradaPrimerNivel=list_get(tablaALiberar->entradasPrimerNivel,i);
        t_tablaSegundoNivel* tablaSegundoNivel = list_get(tablasSegundoNivel, entradaPrimerNivel->indiceTablaSegundoNivel);
        for(int j=0; j<list_size(tablaSegundoNivel->marcos); j++){
            int nroPagina = memoria_swapCfg->PAGINAS_POR_TABLA*i+j;
            t_marco* m = list_get(tablaSegundoNivel->marcos, j);
            if(m->presencia){
                if(m->modificado){
                    log_info(memoria_swapLogger, "Memoria: Escribiendo pagina %i en SWAP del proceso %i", nroPagina, pid);
                    escribir_en_archivo(pid, m->marco, nroPagina);
                    agregar_a_lista_paginas_suspendidas(pid, nroPagina);
                }
                liberar_marco(m);
            }
        }
    }
    log_info(memoria_swapLogger,"Marcos liberados para el proceso %i",pid);
}

bool pagina_fue_suspendida(uint32_t pid, uint32_t nroPagina){
    t_list* listaPaginasSuspendidas = buscar_lista_paginas_suspendidas(pid);
    if(listaPaginasSuspendidas == NULL){
        log_error(memoria_swapLogger,"No se encontro la lista de paginas suspendidas del proceso %i",pid);
        exit(-1);
    }
    for(int i=0; i<list_size(listaPaginasSuspendidas); i++){
        uint32_t* p = list_get(listaPaginasSuspendidas, i);
        if(*p == nroPagina){
            return true;
        }
    }
    return false;
}

// bool pagina_fue_suspendida(uint32_t pid, uint32_t nroPagina){
//     for (int i = 0; i < list_size(procesosSuspendidos); i++) {
//         t_procesoSuspendido* procesoSusp = list_get(procesosSuspendidos, i);
//         if(procesoSusp->pid == pid){
//             for (int j = 0; j < list_size(procesoSusp->paginasSuspendidas); j++) {
//                 uint32_t* paginaSusp = list_get(procesoSusp->paginasSuspendidas, j);
//                 if(*paginaSusp == nroPagina){
//                     remover_de_lista_paginas_suspendidas(pid,nroPagina);
//                     return true;
//                 }
//             }
//             return false;
//         }
//     }
//     return false;
// }


t_list* buscar_lista_paginas_suspendidas(uint32_t pid){
    for (int i = 0; i < list_size(procesosSuspendidos); i++) {
        t_procesoSuspendido* procesoSusp = list_get(procesosSuspendidos, i);
        if(procesoSusp->pid == pid){
            return procesoSusp->paginasSuspendidas;
        }
    }
    return NULL;
}

void crear_lista_paginas_suspendidas(uint32_t pid){
    t_procesoSuspendido* p = malloc(sizeof(t_procesoSuspendido));
    p->pid = pid;
    p->paginasSuspendidas = list_create();
    list_add(procesosSuspendidos, p);
}


void agregar_a_lista_paginas_suspendidas(uint32_t pid, uint32_t nroPagina){
    t_list* listaPaginasSuspendidas = buscar_lista_paginas_suspendidas(pid);
    t_paginaSuspendida* nroPaginaAgregar = malloc(sizeof(t_paginaSuspendida));
    nroPaginaAgregar->nroPagina = nroPagina;
    
    list_add(listaPaginasSuspendidas, nroPaginaAgregar);
    log_debug(memoria_swapLogger,"Se agrega pagina susp pid: %i, pagina: %i",pid,nroPagina);
}

void remover_de_lista_paginas_suspendidas(uint32_t pid, uint32_t nroPagina){
    t_list* listaPaginasSuspendidas = buscar_lista_paginas_suspendidas(pid);
    uint32_t* nroPaginaAQuitar;
    for(int i=0; i<list_size(listaPaginasSuspendidas); i++){
        nroPaginaAQuitar = list_get(listaPaginasSuspendidas, i);
        if(*nroPaginaAQuitar == nroPagina){
            nroPaginaAQuitar = list_remove(listaPaginasSuspendidas, i);
            log_debug(memoria_swapLogger,"Se quita pagina susp pid: %i, pagina: %i",pid,nroPagina);
            break;
        }
    }
    free(nroPaginaAQuitar);
}