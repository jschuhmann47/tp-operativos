#include "procesar_instrucciones.h"

void recibir_instrucciones_cpu(int socket_cpu){
    while(1){
        
        op_code opCode;
        if(recv(socket_cpu, &opCode, sizeof(op_code), MSG_WAITALL) == -1){
            log_error(memoria_swapLogger, "Memoria: Error al recibir opCode de CPU: %s", strerror(errno));
            break;
        }
        
        switch(opCode){
            case INSTRUCCION:
                ;
                uint32_t size;
                if(recv(socket_cpu, &size, sizeof(uint32_t), MSG_WAITALL) == -1){
                    log_error(memoria_swapLogger, "Memoria: Error al recibir size de CPU: %s", strerror(errno));
                    break;
                }
                void* buffer=malloc(size);
                log_info(memoria_swapLogger, "Memoria: Esperando instruccion de CPU");
                if(recv(socket_cpu, buffer, size, MSG_WAITALL)){
                    procesar_instruccion(buffer,socket_cpu);
                }
                free(buffer);
                break;
            case TABLAUNO:
                procesar_entrada_tabla_primer_nv(socket_cpu);
                break;
            case TABLADOS:
                procesar_entrada_tabla_segundo_nv(socket_cpu);
                break;
            default:
                log_error(memoria_swapLogger, "Memoria: Error al leer el codigo de operacion");
                break;    
        }

        
    }
}

void procesar_instruccion(void* buffer, int socket_cpu){
    code_instruccion codOp;
    memcpy(&codOp, buffer, sizeof(code_instruccion));
    uint32_t param1;
    uint32_t param2;
    log_info(memoria_swapLogger, "Memoria: Recibi el codigo de operacion: %i", codOp);
    uint32_t leido;
    switch (codOp)
    {
    case READ:
        memcpy(&param1, buffer+sizeof(code_instruccion), sizeof(uint32_t));
        log_info(memoria_swapLogger, "Memoria: Recibi READ con parametro: %i", param1);
        leido = procesar_read(param1);
        send(socket_cpu, &leido, sizeof(uint32_t), 0);
        break;
    case WRITE:
        memcpy(&param1, buffer+sizeof(code_instruccion), sizeof(uint32_t));
        memcpy(&param2, buffer+sizeof(code_instruccion)+sizeof(uint32_t), sizeof(uint32_t));
        log_info(memoria_swapLogger, "Memoria: Recibi WRITE con parametros: %i, %i", param1, param2);
        procesar_write(param1, param2);
        actualizar_bit_de_marco(socket_cpu, param1);
        break;
    default:
        log_error(memoria_swapLogger, "Memoria: Error al leer el codigo de operacion");
        break;
    }
    //devolver a cpu un ok o ver que devuelve en cada caso
}

uint32_t procesar_read(uint32_t direccionFisica){ //READ devuelve el valor leido
    log_info(memoria_swapLogger, "Memoria: Procesando READ");
    uint32_t desplazamiento = direccionFisica % memoria_swapCfg->TAM_PAGINA;
    uint32_t marco = (direccionFisica - desplazamiento) / memoria_swapCfg->TAM_PAGINA;
    uint32_t* leido = leer_de_memoria(MEMORIA_PRINCIPAL, marco, desplazamiento, sizeof(uint32_t));
    marcar_marco_ocupado(marco);
    log_info(memoria_swapLogger, "Memoria: READ terminado %i", *leido);
    return *leido;
}

void procesar_write(uint32_t direccionFisica, uint32_t valor){ //write no dice, devolver ok o error?
    log_info(memoria_swapLogger, "Memoria: Procesando WRITE");
    uint32_t desplazamiento = direccionFisica % memoria_swapCfg->TAM_PAGINA;
    uint32_t marco = (direccionFisica - desplazamiento) / memoria_swapCfg->TAM_PAGINA;
    log_info(memoria_swapLogger, "Memoria: Marco %i", marco);
    escribir_en_memoria(MEMORIA_PRINCIPAL, &valor, marco, desplazamiento, sizeof(uint32_t));
    marcar_marco_ocupado(marco);
    log_info(memoria_swapLogger, "Memoria: WRITE terminado");
}

void actualizar_bit_de_marco(int socket_cpu, uint32_t direccionFisica){
    uint32_t desplazamiento = direccionFisica % memoria_swapCfg->TAM_PAGINA;
    uint32_t marco = (direccionFisica - desplazamiento) / memoria_swapCfg->TAM_PAGINA;
    uint32_t nroTablaPrimerNv, entradaTablaPrimerNv;
    if(recv(socket_cpu, &nroTablaPrimerNv, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir marco de CPU: %s", strerror(errno));
    }
    if(recv(socket_cpu, &entradaTablaPrimerNv, sizeof(uint32_t), MSG_WAITALL) == -1){
        log_error(memoria_swapLogger, "Memoria: Error al recibir marco de CPU: %s", strerror(errno));
    }
    t_tablaPrimerNivel* tUno = list_get(tablasPrimerNivel, nroTablaPrimerNv);
    t_entradaPrimerNivel* eUno = list_get(tUno->entradasPrimerNivel, entradaTablaPrimerNv);
    t_tablaSegundoNivel* tabla = list_get(tablasSegundoNivel, eUno->indiceTablaSegundoNivel);
    for(int i = 0; i < list_size(tabla->marcos); i++){
        t_marco* entradaMarco = list_get(tabla->marcos, i);
        if(entradaMarco->marco == marco && entradaMarco->presencia){
            log_info(memoria_swapLogger, "Memoria: Actualizando bit de modificado de marco %i", marco);
            entradaMarco->modificado = true;
            break;
        }
    }
    
}

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
                    log_info(memoria_swapLogger, "Memoria: Escribiendo pagina %i  en SWAP del proceso %i en el marco %i", nroPagina, pid, m->marco);
                    escribir_en_archivo(pid, m->marco, nroPagina);
                }
                liberar_marco(m);
            }
        }
    }
    log_info(memoria_swapLogger,"Marcos liberados para el proceso %i",pid);
}