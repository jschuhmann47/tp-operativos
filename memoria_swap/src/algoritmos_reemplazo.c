#include "algoritmos_reemplazo.h"



void reemplazo_clock(t_marco* paginaAAgregar,t_tablaSegundoNivel* tablaSegundoNivel){
    int limite = list_size(tablaSegundoNivel->marcos);
    
    while (1){
        t_marco* victima = NULL;
        t_marco* pagina = list_get(tablaSegundoNivel->marcos, tablaSegundoNivel->puntero);
        
        if(!pagina->uso){
           victima = list_remove(tablaSegundoNivel->marcos, tablaSegundoNivel->puntero);
           paginaAAgregar->marco=victima->marco;
           list_add_in_index(tablaSegundoNivel, tablaSegundoNivel->puntero, paginaAAgregar);
           tablaSegundoNivel->puntero++;
           break;
        }else{
            pagina->uso = false;
            tablaSegundoNivel->puntero++;
        }

        if(tablaSegundoNivel->puntero == limite){
            tablaSegundoNivel->puntero = 0;
        }
    }
}

void reemplazo_clock_modificado(t_marco* paginaAAgregar,t_tablaSegundoNivel* tablaSegundoNivel){
    int contadorPasadas = 0;
    int limite = list_size(tablaSegundoNivel->marcos);
    int nroVuelta = 0;
    while (1){
        t_marco* victima = NULL;
        t_marco* pagina = list_get(tablaSegundoNivel->marcos, tablaSegundoNivel->puntero);

        if(nroVuelta==1){
            if(!pagina->uso && pagina->modificado){
                victima = list_remove(tablaSegundoNivel, tablaSegundoNivel->puntero);
                paginaAAgregar->marco=victima->marco;
                list_add_in_index(tablaSegundoNivel, tablaSegundoNivel->puntero, paginaAAgregar);
                break;
            }
            else{
                pagina->uso = false;
                contadorPasadas++;
                tablaSegundoNivel->puntero++;
            }
            if(contadorPasadas == limite){
                contadorPasadas = 0;
                nroVuelta = 0;
            }
        }
        else{
            if(!pagina->uso && !pagina->modificado){
                victima = list_remove(tablaSegundoNivel, tablaSegundoNivel->puntero);
                paginaAAgregar->marco=victima->marco;
                list_add_in_index(tablaSegundoNivel, tablaSegundoNivel->puntero, paginaAAgregar);
                tablaSegundoNivel->puntero++;
                break;
            } else{
                contadorPasadas++;
                tablaSegundoNivel->puntero++;
            }
            
            if(contadorPasadas==limite){
                nroVuelta = 1;
                contadorPasadas = 0;
            }
        }
        
        if(tablaSegundoNivel->puntero == limite){
            tablaSegundoNivel->puntero = 0;
        }

        
    }
    
}

/* clock modificado:
Busca U=0 M=0
Busca U=0 M=1 y reemplaza U=1 a U=0
repetir hasta encontrar
*/