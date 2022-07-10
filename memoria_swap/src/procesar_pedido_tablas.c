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
    
    t_marcoAsignado* marcosAsig = buscar_marcos_asignados_al_proceso(tablaSegundoNivel->pid); 
    
    int marcoLibre;
    if(list_size(marcosAsig->marcosAsignados) == memoria_swapCfg->MARCOS_POR_PROCESO){
        t_marco* nuevoMarco = malloc(sizeof(t_marco));
        nuevoMarco->presencia=true;
        nuevoMarco->uso=true;
        if(strcmp(memoria_swapCfg->ALGORITMO_REEMPLAZO,"CLOCK")==0){
            reemplazo_clock(nuevoMarco,tablaSegundoNivel); //falta testear
            marcoLibre = nuevoMarco->marco;
        }
        if(strcmp(memoria_swapCfg->ALGORITMO_REEMPLAZO,"CLOCK-M")==0){
            reemplazo_clock_modificado(nuevoMarco,tablaSegundoNivel); //falta testear
            marcoLibre = nuevoMarco->marco;
        }
        return marcoLibre;
    }else{
        marcoLibre = encontrar_marco_libre();
        t_marco* m = list_get(tablaSegundoNivel->marcos,indicePagina);
        m->marco=marcoLibre;
        m->presencia=true;
        m->uso=true;
        int *nroMarco = malloc(sizeof(int));
        list_add(marcosAsig->marcosAsignados,nroMarco);
        return marcoLibre;
    }

    
}

t_marcoAsignado* buscar_marcos_asignados_al_proceso(uint32_t pid){
    //return list_get(marcosAsignadosPorProceso, pid-1); //el pid empezaba en 1 no? si esto funca se puede sacar pid del struct
    for (int i = 0; i < list_size(marcosAsignadosPorProceso); i++) {
        t_marcoAsignado* marcosAsig = list_get(marcosAsignadosPorProceso, i);
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
