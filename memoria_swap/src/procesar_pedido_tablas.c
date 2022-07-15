#include "procesar_pedidos_tablas.h"

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
    
    log_info(memoria_swapLogger, "Memoria: Llego requestPrimerTabla %i",requestPrimerTabla);

    t_entradaPrimerNivel* entrada = list_get(tablaPrimerNv->entradasPrimerNivel, requestPrimerTabla);
    uint32_t indiceSegundoNivel = entrada->indiceTablaSegundoNivel;

    log_info(memoria_swapLogger, "Memoria: INDICE %i",indiceSegundoNivel);
    int bytes;
    if((bytes=send(socket_cpu,&indiceSegundoNivel,sizeof(uint32_t),0)) == -1){ 
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
    
    t_tablaSegundoNivel* tablaSegundoNivel = list_get(tablasSegundoNivel, nroTabla);
    
    uint32_t nroPagina;
    if(recv(socket_cpu, &nroPagina, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir nroPagina de CPU: %s", strerror(errno));
        exit(-1);
    }

    t_marco* marcoEncontrado = list_get(tablaSegundoNivel->marcos, nroPagina);
    uint32_t nroMarco = marcoEncontrado->marco;
    if(!marcoEncontrado->presencia){
        nroMarco = conseguir_marco_libre(tablaSegundoNivel, nroPagina);
        marcar_marco_ocupado(nroMarco);
    }

    if(proceso_fue_suspendido(tablaSegundoNivel->pid)){
        cargar_pagina_en_memoria(tablaSegundoNivel, nroPagina, nroMarco);
    }

    if(send(socket_cpu,&nroMarco,sizeof(uint32_t),0) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al enviar marco a CPU: %s", strerror(errno));
        exit(-1);
    }

}

int conseguir_marco_libre(t_tablaSegundoNivel* tablaSegundoNivel, uint32_t indicePagina){
    
    t_marcosAsignadoPorProceso* marcosAsig = buscar_marcos_asignados_al_proceso(tablaSegundoNivel->pid); 
    int nroPagina = (tablaSegundoNivel->indice % memoria_swapCfg->PAGINAS_POR_TABLA)*memoria_swapCfg->PAGINAS_POR_TABLA+indicePagina;
    log_info(memoria_swapLogger, "Memoria :nroPagina %i - nroIndice %i",nroPagina,indicePagina);
    int marcoLibre;
    if(list_size(marcosAsig->marcosAsignados) == memoria_swapCfg->MARCOS_POR_PROCESO){
        marcoLibre = reemplazar_pagina(tablaSegundoNivel, marcosAsig, nroPagina);
        return marcoLibre;
    }else{
        marcoLibre = encontrar_marco_libre();
        if(marcoLibre != -1){
            t_marco* m = list_get(tablaSegundoNivel->marcos,indicePagina);
            m->marco=marcoLibre;
            m->presencia=true;
            m->uso=true;
            t_marcoAsignado* mA = malloc(sizeof(t_marcoAsignado));
            mA->marco = malloc(sizeof(t_marco));
            mA->marco->marco=marcoLibre;
            mA->marco->presencia=true;
            mA->marco->uso=true;
            mA->marco->modificado=false;
            mA->nroTablaSegundoNivel=tablaSegundoNivel->indice;
            list_add(marcosAsig->marcosAsignados,mA);
            return marcoLibre; 
        }else{
            marcoLibre = reemplazar_pagina(tablaSegundoNivel, marcosAsig, nroPagina);
            return marcoLibre;
        }
    }
}

int reemplazar_pagina(t_tablaSegundoNivel* tablaSegundoNivel, t_marcosAsignadoPorProceso* marcosAsig, int nroPagina){
    t_marco* victima = NULL;
    t_marco* nuevoMarco = malloc(sizeof(t_marco));
    nuevoMarco->presencia=true;
    nuevoMarco->uso=true;
    nuevoMarco->modificado=false;
    nuevoMarco->marco=-1;
    int paginaVictima = -1;
    if(strcmp(memoria_swapCfg->ALGORITMO_REEMPLAZO,"CLOCK")==0){
        victima = reemplazo_clock(tablaSegundoNivel,nuevoMarco,marcosAsig,nroPagina,&paginaVictima); //falta testear
    }
    if(strcmp(memoria_swapCfg->ALGORITMO_REEMPLAZO,"CLOCK-M")==0){
        victima = reemplazo_clock_modificado(tablaSegundoNivel,nuevoMarco,marcosAsig,nroPagina,&paginaVictima); //falta testear
    }
    if(victima->modificado){
       escribir_en_archivo(tablaSegundoNivel->pid, victima->marco, nroPagina); 
    }
    free(victima);
    return nuevoMarco->marco;
}

t_marcosAsignadoPorProceso* buscar_marcos_asignados_al_proceso(uint32_t pid){
    //return list_get(marcosAsignadosPorProceso, pid-1); //el pid empezaba en 1 no? si esto funca se puede sacar pid del struct
    for (int i = 0; i < list_size(marcosAsignadosPorProceso); i++) {
        t_marcosAsignadoPorProceso* marcosAsig = list_get(marcosAsignadosPorProceso, i);
        if(marcosAsig->pid == pid){
            return marcosAsig;
        }
    }
}

bool proceso_fue_suspendido(uint32_t pid){
    for (int i = 0; i < list_size(procesosSuspendidos); i++) {
        t_procesoSuspendido* procesoSusp = list_get(procesosSuspendidos, i);
        if(procesoSusp->pid == pid){
            if(procesoSusp->fueSuspendido){
                procesoSusp->fueSuspendido = false;
                return true;
            }else{
                return false;
            }
        }
    }
    return false;
}

void cargar_pagina_en_memoria(t_tablaSegundoNivel* tablaSegundoNivel, uint32_t nroPagina, uint32_t nroMarco){
    void* lectura = leer_de_archivo(tablaSegundoNivel->pid, nroPagina);
    escribir_en_memoria(MEMORIA_PRINCIPAL,lectura,nroMarco,nroMarco*memoria_swapCfg->TAM_PAGINA,memoria_swapCfg->TAM_PAGINA);
    free(lectura);
}
