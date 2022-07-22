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
    uint32_t leido;
    switch (codOp)
    {
    case READ:
        memcpy(&param1, buffer+sizeof(code_instruccion), sizeof(uint32_t));
        log_debug(memoria_swapLogger, "Memoria: Recibi READ con direccion fisica: %i", param1);
        leido = procesar_read(param1);

        sleep(memoria_swapCfg->RETARDO_MEMORIA/1000);

        if(send(socket_cpu, &leido, sizeof(uint32_t), 0)==-1){
            log_error(memoria_swapLogger, "Memoria: No se pudo mandar el contenido leido");
            exit(-1);
        }
        actualizar_bit_de_marco(socket_cpu, param1,READ); //CPU
        break;
    case WRITE:
        memcpy(&param1, buffer+sizeof(code_instruccion), sizeof(uint32_t));
        memcpy(&param2, buffer+sizeof(code_instruccion)+sizeof(uint32_t), sizeof(uint32_t));
        log_debug(memoria_swapLogger, "Memoria: Recibi WRITE con direccion fisica: %i, valor a escribir:%i", param1, param2);
        
        sleep(memoria_swapCfg->RETARDO_MEMORIA/1000);
        
        procesar_write(param1, param2);
        actualizar_bit_de_marco(socket_cpu, param1,WRITE);
        break;
    default:
        log_error(memoria_swapLogger, "Memoria: Error al leer el codigo de operacion");
        break;
    }
}

uint32_t procesar_read(uint32_t direccionFisica){ 
    log_debug(memoria_swapLogger, "Memoria: Procesando READ");
    uint32_t desplazamiento = direccionFisica % memoria_swapCfg->TAM_PAGINA;
    uint32_t marco = (direccionFisica - desplazamiento) / memoria_swapCfg->TAM_PAGINA;
    uint32_t* leido = leer_de_memoria(MEMORIA_PRINCIPAL, marco, desplazamiento, sizeof(uint32_t));
    uint32_t lectura = *leido;
    free(leido);
    log_info(memoria_swapLogger, "Memoria: READ terminado, se leyo %i", lectura);
    return lectura;
}

void procesar_write(uint32_t direccionFisica, uint32_t valor){
    log_debug(memoria_swapLogger, "Memoria: Procesando WRITE");
    uint32_t desplazamiento = direccionFisica % memoria_swapCfg->TAM_PAGINA;
    uint32_t marco = (direccionFisica - desplazamiento) / memoria_swapCfg->TAM_PAGINA;
    log_debug(memoria_swapLogger, "Memoria: Marco %i", marco);
    escribir_en_memoria(MEMORIA_PRINCIPAL, &valor, marco, desplazamiento, sizeof(uint32_t));
    marcar_marco_ocupado(marco);
    log_info(memoria_swapLogger, "Memoria: WRITE terminado");
}

void actualizar_bit_de_marco(int socket_cpu, uint32_t direccionFisica, op_code codOp){
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
            int nroPagina = (eUno->indiceTablaSegundoNivel % memoria_swapCfg->PAGINAS_POR_TABLA)*memoria_swapCfg->PAGINAS_POR_TABLA+i;
            log_info(memoria_swapLogger, "Memoria: Actualizando bit de uso de pagina %i", nroPagina);
            entradaMarco->uso = true;
            if(codOp == WRITE){
                log_info(memoria_swapLogger, "Memoria: Actualizando bit de modificado de pagina %i", nroPagina);
                entradaMarco->modificado = true;
            }
            break;
        }
    }
    
}



