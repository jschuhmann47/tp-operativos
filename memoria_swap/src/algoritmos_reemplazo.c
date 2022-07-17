#include "algoritmos_reemplazo.h"



t_marco* reemplazo_clock(t_tablaSegundoNivel* tablaSegNv, t_marco* paginaAAgregar,t_marcosAsignadoPorProceso* marcosAsig,int nroPagina,int* paginaVictima){
    int limite = list_size(marcosAsig->marcosAsignados);
    
    while (1){
        t_marcoAsignado* victima = NULL;
        t_marco* pagina = list_get(marcosAsig->marcosAsignados, marcosAsig->puntero);
        log_debug(memoria_swapLogger,"Puntero actual: (Clock), marco: %i",marcosAsig->puntero);

        if(!pagina->uso){
            actualizar_paginas(&victima,marcosAsig,tablaSegNv,paginaAAgregar,nroPagina,paginaVictima);
            marcosAsig->puntero++;
            if(marcosAsig->puntero == limite){
                marcosAsig->puntero = 0;
            }
            log_info(memoria_swapLogger,"Se reemplazo una pagina (Clock), marco: %i",paginaAAgregar->marco);
            return victima->marco;
        }else{
            pagina->uso = false;
            marcosAsig->puntero++;
        }

        if(marcosAsig->puntero == limite){
            marcosAsig->puntero = 0;
        }
    }
}

t_marco* reemplazo_clock_modificado(t_tablaSegundoNivel* tablaSegNv, t_marco* paginaAAgregar,t_marcosAsignadoPorProceso* marcosAsig, int nroPagina, int* paginaVictima){
    int contadorPasadas = 0;
    int limite = list_size(marcosAsig->marcosAsignados);
    int nroVuelta = 0;
    while (1){
        t_marcoAsignado* victima = NULL;
        t_marco* pagina = list_get(marcosAsig->marcosAsignados, marcosAsig->puntero);
        log_info(memoria_swapLogger,"Puntero actual: (Clock-M), marco: %i",marcosAsig->puntero);

        if(nroVuelta==1){
            if(!pagina->uso && pagina->modificado){

                actualizar_paginas(&victima,marcosAsig,tablaSegNv,paginaAAgregar,nroPagina,paginaVictima);

                marcosAsig->puntero++;
                if(marcosAsig->puntero == limite){
                    marcosAsig->puntero = 0;
                }
                log_info(memoria_swapLogger,"Se reemplazo una pagina (Clock-M), marco: %i",paginaAAgregar->marco);
                return victima->marco;
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

                actualizar_paginas(&victima,marcosAsig,tablaSegNv,paginaAAgregar,nroPagina,paginaVictima);
                log_info(memoria_swapLogger,"Victima A(Clock-M), marco: %i",victima->marco->marco);
                log_info(memoria_swapLogger,"Se reemplazo una pagina (Clock-M), marco: %i",paginaAAgregar->marco);
                marcosAsig->puntero++;
                if(marcosAsig->puntero == limite){
                    marcosAsig->puntero = 0;
                }
                return victima->marco;
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

int buscar_nro_pagina(t_tablaSegundoNivel* tablaSegNv, t_marco* marco){
    for(int i = 0; i < list_size(tablaSegNv->marcos); i++){
        t_marco* pagina = list_get(tablaSegNv->marcos, i);
        if(pagina->marco == marco->marco && marco->presencia){
            return (tablaSegNv->indice % memoria_swapCfg->PAGINAS_POR_TABLA)*memoria_swapCfg->PAGINAS_POR_TABLA+i;
        }
    }
    log_error(memoria_swapLogger,"No se encontro la pagina en la tabla de paginas");
    exit(-1);
}

void actualizar_paginas(t_marcoAsignado** victima,t_marcosAsignadoPorProceso* marcosAsig,t_tablaSegundoNivel* tablaSegNv,t_marco* paginaAAgregar,int nroPagina,int* paginaVictima){
    t_marcoAsignado* vict = list_replace(marcosAsig->marcosAsignados, marcosAsig->puntero, paginaAAgregar); //reemplazo en la de asignados
    log_info(memoria_swapLogger,"Victima A(Clock-M), marco: %i",vict->marco->marco);

    paginaAAgregar->marco = vict->marco->marco;
    t_marco* paginaAActualizarNueva = list_get(tablaSegNv->marcos, nroPagina % memoria_swapCfg->PAGINAS_POR_TABLA); //pongo en presencia la nueva
    paginaAActualizarNueva->marco = vict->marco->marco;
    paginaAActualizarNueva->uso = paginaAAgregar->uso;
    paginaAActualizarNueva->presencia = paginaAAgregar->presencia;
    
    t_tablaSegundoNivel* tablaVictima = list_get(tablasSegundoNivel, vict->nroTablaSegundoNivel);
    int nroPaginaVictima = buscar_nro_pagina(tablaVictima,vict->marco);
    *paginaVictima = nroPaginaVictima;
    t_marco* paginaAActualizarVieja = list_get(tablaVictima->marcos, nroPaginaVictima % memoria_swapCfg->PAGINAS_POR_TABLA); //quito de presencia la victima
    paginaAActualizarVieja->presencia = false;
    paginaAActualizarVieja->uso = false;
    paginaAActualizarVieja->marco = -1; 
    
    *victima = vict;
}

/* clock modificado:
Busca U=0 M=0
Busca U=0 M=1 y reemplaza U=1 a U=0
repetir hasta encontrar
*/