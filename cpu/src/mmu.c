#include "mmu.h"

uint32_t obtener_marco_de_memoria(uint32_t direccionLogica, //DEVUELVE EL MARCO
                          uint32_t tamanioPagina, 
                          uint32_t paginasPorTabla,
                          int socket_memoria){

    uint32_t numeroDePagina = floor(direccionLogica/tamanioPagina);

    uint32_t entradaTablaPrimerNivel = floor(numeroDePagina / paginasPorTabla);

    uint32_t entradaTablaSegundoNivel = numeroDePagina % paginasPorTabla;

    uint32_t rtaTablaPrimerNivel,marco;

    // log_info(cpuLogger, "CPU: Numero de Pagina: %i", numeroDePagina);
    // log_info(cpuLogger, "CPU: Tabla Primer Nivel: %i", entradaTablaPrimerNivel);
    // log_info(cpuLogger, "CPU: Tabla Segundo Nivel: %i", entradaTablaSegundoNivel);

    op_code opCode = TABLAUNO;
    if(send(socket_memoria,&opCode,sizeof(op_code),0)<0){
        log_error(cpuLogger, "CPU: No se pudo enviar opCode a Memoria.");
        
    }
    //log_info(cpuLogger, "CPU: Se mando opCode a Memoria.");

    if(send(socket_memoria,&entradaTablaPrimerNivel,sizeof(uint32_t),0)<0){
        log_error(cpuLogger, "CPU: No se pudo enviar entradaTablaPrimerNivel a Memoria.");
        
    }else{
        //log_info(cpuLogger, "CPU: Se mando entradaTablaPrimerNivel a Memoria.");
        if(recv(socket_memoria,&rtaTablaPrimerNivel,sizeof(uint32_t),MSG_WAITALL)<0){
            log_error(cpuLogger, "CPU: No se pudo recibir rtaTablaPrimerNivel de Memoria.");
        }
        else{
            //log_info(cpuLogger, "CPU: Se recibio rtaTablaPrimerNivel de Memoria.");

            op_code opCodeDos = TABLADOS;
            if(send(socket_memoria,&opCodeDos,sizeof(op_code),0)<0){
                log_error(cpuLogger, "CPU: No se pudo enviar opCodeDos a Memoria.");
            }
            //log_info(cpuLogger, "CPU: Se mando opCodeDos a Memoria.");
            if(send(socket_memoria,&rtaTablaPrimerNivel,sizeof(uint32_t),0)<0){
                log_error(cpuLogger, "CPU: No se pudo enviar rtaTablaPrimerNivel a Memoria.");
            }
            
            if(send(socket_memoria,&entradaTablaSegundoNivel,sizeof(uint32_t),0)<0){
                log_error(cpuLogger, "CPU: No se pudo enviar entradaTablaSegundoNivel a Memoria.");
            }
            else{
                
                if(recv(socket_memoria,&marco,sizeof(uint32_t),MSG_WAITALL)<0){
                    log_error(cpuLogger, "CPU: No se pudo recibir rtaTablaSegundoNivel de Memoria.");
                }
                else{
                    //log_info(cpuLogger, "CPU: Se recibio rtaTablaSegundoNivel de Memoria.");
                    return marco;
                }
            }
        }
    }

    

    return 0;
}

uint32_t obtener_direccion_fisica(uint32_t marco, uint32_t desplazamiento){
    return (marco * tamanioPagina) + desplazamiento;
}

uint32_t obtener_desplazamiento(uint32_t direccionLogica, uint32_t numeroDePagina){
    return direccionLogica - numeroDePagina * tamanioPagina;
}

uint32_t traducir_direccion_logica(uint32_t direccionLogica, int socket_memoria,t_log* cpuLogger){
    uint32_t nroPagina = floor(direccionLogica/tamanioPagina);
    uint32_t indice = obtener_indice_traduccion_tlb(nroPagina);
    uint32_t marcoR;
    uint32_t desplazamientoR;
    if(indice==-1){ //no esta en la tlb
        log_info(cpuLogger, "CPU: Fallo en la TLB, traduciendo direccion");
        marcoR = obtener_marco_de_memoria(direccionLogica, tamanioPagina, paginasPorTabla,socket_memoria);
        agregar_traduccion_a_tabla_tlb(nroPagina, marcoR);
    }else{
        marcoR = obtener_traduccion_tlb(indice);
    }
    return obtener_direccion_fisica(marcoR, obtener_desplazamiento(direccionLogica, nroPagina));
}
