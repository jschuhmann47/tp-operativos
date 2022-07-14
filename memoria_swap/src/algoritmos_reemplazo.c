#include "algoritmos_reemplazo.h"



t_marco* reemplazo_clock(t_tablaSegundoNivel* tablaSegNv, t_marco* paginaAAgregar,t_marcoAsignado* marcosAsig,int nroPagina){
    int limite = list_size(marcosAsig->marcosAsignados);
    log_info(memoria_swapLogger,"Limite (Clock), marco: %i",limite);
    
    while (1){
        t_marco* victima = NULL;
        t_marco* pagina = list_get(marcosAsig->marcosAsignados, marcosAsig->puntero);
        log_info(memoria_swapLogger,"Puntero actual: (Clock), marco: %i",marcosAsig->puntero);

        
        if(!pagina->uso){
            victima = list_remove(marcosAsig->marcosAsignados, marcosAsig->puntero);
            paginaAAgregar->marco = victima->marco;
            list_add_in_index(marcosAsig->marcosAsignados, marcosAsig->puntero, paginaAAgregar);
            list_remove(tablaSegNv->marcos, nroPagina % memoria_swapCfg->PAGINAS_POR_TABLA);
            list_add_in_index(tablaSegNv->marcos, nroPagina % memoria_swapCfg->PAGINAS_POR_TABLA, paginaAAgregar);
            marcosAsig->puntero++;
            if(marcosAsig->puntero == limite){
                marcosAsig->puntero = 0;
            }
            log_info(memoria_swapLogger,"Se reemplazo una pagina (Clock), marco: %i",paginaAAgregar->marco);
            return victima;
        }else{
            pagina->uso = false;
            marcosAsig->puntero++;
        }

        if(marcosAsig->puntero == limite){
            marcosAsig->puntero = 0;
        }
    }
}

t_marco* reemplazo_clock_modificado(t_tablaSegundoNivel* tablaSegNv, t_marco* paginaAAgregar,t_marcoAsignado* marcosAsig, int nroPagina){
    int contadorPasadas = 0;
    int limite = list_size(marcosAsig->marcosAsignados);
    int nroVuelta = 0;
    while (1){
        t_marco* victima = NULL;
        t_marco* pagina = list_get(marcosAsig->marcosAsignados, marcosAsig->puntero);
        log_info(memoria_swapLogger,"Puntero actual: (Clock-M), marco: %i",marcosAsig->puntero);

        if(nroVuelta==1){
            if(!pagina->uso && pagina->modificado){
                victima = list_remove(marcosAsig->marcosAsignados, marcosAsig->puntero);
                log_info(memoria_swapLogger,"Marco victima lista asig (Clock-M): %i",victima->marco);
                log_info(memoria_swapLogger,"Id tabla 2do nivel: %i, pagina nro %i",tablaSegNv->indice, nroPagina);
                paginaAAgregar->marco = victima->marco;
                list_add_in_index(marcosAsig->marcosAsignados, marcosAsig->puntero, paginaAAgregar);
                //(tablaSegundoNivel->indice % memoria_swapCfg->PAGINAS_POR_TABLA)*memoria_swapCfg->PAGINAS_POR_TABLA
                t_marco* victimaEnTablaSegNv = list_remove(tablaSegNv->marcos, nroPagina - (tablaSegNv->indice % memoria_swapCfg->PAGINAS_POR_TABLA)*memoria_swapCfg->PAGINAS_POR_TABLA);
                log_info(memoria_swapLogger,"Marco victima tabla (Clock-M): %i",victimaEnTablaSegNv->marco);
                list_add_in_index(tablaSegNv->marcos, nroPagina - (tablaSegNv->indice % memoria_swapCfg->PAGINAS_POR_TABLA)*memoria_swapCfg->PAGINAS_POR_TABLA, paginaAAgregar);
                marcosAsig->puntero++;
                if(marcosAsig->puntero == limite){
                    marcosAsig->puntero = 0;
                }
                log_info(memoria_swapLogger,"Se reemplazo una pagina (Clock-M), marco: %i",paginaAAgregar->marco);
                return victima;
                //break;
            }
            else{
                pagina->uso = false;
                contadorPasadas++;
                marcosAsig->puntero++;
            }
            if(contadorPasadas == limite){
                nroVuelta = 0;
                contadorPasadas = 0;
            }
        }
        else{
            if(!pagina->uso && !pagina->modificado){
                victima = list_remove(marcosAsig->marcosAsignados, marcosAsig->puntero);
                paginaAAgregar->marco = victima->marco;
                list_add_in_index(marcosAsig->marcosAsignados, marcosAsig->puntero, paginaAAgregar);
                list_remove(tablaSegNv->marcos, nroPagina - (tablaSegNv->indice % memoria_swapCfg->PAGINAS_POR_TABLA)*memoria_swapCfg->PAGINAS_POR_TABLA);
                list_add_in_index(tablaSegNv->marcos, nroPagina - (tablaSegNv->indice % memoria_swapCfg->PAGINAS_POR_TABLA)*memoria_swapCfg->PAGINAS_POR_TABLA, paginaAAgregar);
                log_info(memoria_swapLogger,"Se reemplazo una pagina (Clock-M), marco: %i",paginaAAgregar->marco);
                marcosAsig->puntero++;
                if(marcosAsig->puntero == limite){
                    marcosAsig->puntero = 0;
                }
                return victima;
            } else{
                contadorPasadas++;
                marcosAsig->puntero++;
            }
            
            if(contadorPasadas==limite){
                nroVuelta = 1;
                contadorPasadas = 0;
            }
        }
        
        if(marcosAsig->puntero == limite){
            marcosAsig->puntero = 0;
        }

        
    }
    
}

/* clock modificado:
Busca U=0 M=0
Busca U=0 M=1 y reemplaza U=1 a U=0
repetir hasta encontrar
*/