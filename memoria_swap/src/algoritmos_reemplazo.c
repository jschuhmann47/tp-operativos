#include "algoritmos_reemplazo.h"



t_marco* reemplazo_clock(t_tablaSegundoNivel* tablaSegNv, t_marco* paginaAAgregar,t_marcoAsignado* marcosAsig){
    int limite = list_size(marcosAsig->marcosAsignados);
    
    while (1){
        t_marco* victima = NULL;
        t_marco* pagina = list_get(marcosAsig->marcosAsignados, marcosAsig->puntero);
        
        if(!pagina->uso){
            victima = list_remove(marcosAsig->marcosAsignados, marcosAsig->puntero);
            paginaAAgregar->marco = victima->marco;
            list_add_in_index(marcosAsig->marcosAsignados, marcosAsig->puntero, paginaAAgregar);
            list_remove(tablaSegNv->marcos, marcosAsig->puntero);
            list_add_in_index(tablaSegNv->marcos, marcosAsig->puntero, paginaAAgregar);
            marcosAsig->puntero++;
            log_info(memoria_swapLogger,"Se reemplazo una pagina (Clock), marco: %i",paginaAAgregar->marco);
            return victima;
            //break;
        }else{
            pagina->uso = false;
            marcosAsig->puntero++;
        }

        if(marcosAsig->puntero == limite){
            marcosAsig->puntero = 0;
        }
    }
}

t_marco* reemplazo_clock_modificado(t_tablaSegundoNivel* tablaSegNv, t_marco* paginaAAgregar,t_marcoAsignado* marcosAsig){
    int contadorPasadas = 0;
    int limite = list_size(marcosAsig->marcosAsignados);
    int nroVuelta = 0;
    while (1){
        t_marco* victima = NULL;
        t_marco* pagina = list_get(marcosAsig->marcosAsignados, marcosAsig->puntero);

        if(nroVuelta==1){
            if(!pagina->uso && pagina->modificado){
                victima = list_remove(marcosAsig->marcosAsignados, marcosAsig->puntero);
                paginaAAgregar->marco = victima->marco;
                list_add_in_index(marcosAsig->marcosAsignados, marcosAsig->puntero, paginaAAgregar);
                list_remove(tablaSegNv->marcos, marcosAsig->puntero);
                list_add_in_index(tablaSegNv->marcos, marcosAsig->puntero, paginaAAgregar);
                marcosAsig->puntero++;
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
                contadorPasadas = 0;
                nroVuelta = 0;
            }
        }
        else{
            if(!pagina->uso && !pagina->modificado){
                victima = list_remove(marcosAsig->marcosAsignados, marcosAsig->puntero);
                paginaAAgregar->marco = victima->marco;
                list_add_in_index(marcosAsig->marcosAsignados, marcosAsig->puntero, paginaAAgregar);
                list_remove(tablaSegNv->marcos, marcosAsig->puntero);
                list_add_in_index(tablaSegNv->marcos, marcosAsig->puntero, paginaAAgregar);
                log_info(memoria_swapLogger,"Se reemplazo una pagina (Clock-M), marco: %i",paginaAAgregar->marco);
                marcosAsig->puntero++;
                break;
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