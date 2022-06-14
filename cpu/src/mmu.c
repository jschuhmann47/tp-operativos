#include "mmu.h"

uint32_t traducir_direccion(uint32_t direccionLogica, 
                          uint32_t tamanioPagina, 
                          uint32_t paginasPorTabla,
                          int socket_memoria){

    uint32_t numeroDePagina = floor(direccionLogica/tamanioPagina);

    uint32_t entradaTablaPrimerNivel = floor(numeroDePagina/paginasPorTabla);

    uint32_t entradaTablaSegundoNivel = numeroDePagina % paginasPorTabla;

    uint32_t rtaTablaPrimerNivel,marco;

    uint32_t desplazamiento = direccionLogica - numeroDePagina * tamanioPagina;

    op_code opCode = TABLAUNO;
    if(send(socket_memoria,&opCode,sizeof(opCode),0)<0){
        log_error(cpuLogger, "CPU: No se pudo enviar opCode a Memoria.");
        
    }
    log_info(cpuLogger, "CPU: Se mando opCode a Memoria.");

    if(send(socket_memoria,&entradaTablaPrimerNivel,sizeof(entradaTablaPrimerNivel),0)<0){
        log_error(cpuLogger, "CPU: No se pudo enviar entradaTablaPrimerNivel a Memoria.");
        
    }else{
        log_info(cpuLogger, "CPU: Se mando entradaTablaPrimerNivel a Memoria.");
        if(recv(socket_memoria,&rtaTablaPrimerNivel,sizeof(rtaTablaPrimerNivel),MSG_WAITALL)<0){
            log_error(cpuLogger, "CPU: No se pudo recibir rtaTablaPrimerNivel de Memoria.");
        }
        else{
            log_info(cpuLogger, "CPU: Se recibio rtaTablaPrimerNivel de Memoria.");

            op_code opCodeDos = TABLADOS;
            if(send(socket_memoria,&opCodeDos,sizeof(opCode),0)<0){
            log_error(cpuLogger, "CPU: No se pudo enviar opCodeDos a Memoria.");
            }
            log_info(cpuLogger, "CPU: Se mando opCodeDos a Memoria.");

            if(send(socket_memoria,&entradaTablaSegundoNivel,sizeof(entradaTablaSegundoNivel),0)<0){
                log_error(cpuLogger, "CPU: No se pudo enviar entradaTablaSegundoNivel a Memoria.");
            }
            else{
                log_info(cpuLogger, "CPU: Se mando entradaTablaSegundoNivel a Memoria.");
                if(recv(socket_memoria,&marco,sizeof(marco),MSG_WAITALL)<0){
                    log_error(cpuLogger, "CPU: No se pudo recibir rtaTablaSegundoNivel de Memoria.");
                }
                else{
                    log_info(cpuLogger, "CPU: Se recibio rtaTablaSegundoNivel de Memoria.");
                    return marco + desplazamiento;
                }
            }
        }
    }

    log_info(cpuLogger, "CPU: Numero de Pagina: %i", numeroDePagina);
    log_info(cpuLogger, "CPU: Tabla Primer Nivel: %i", entradaTablaPrimerNivel);
    log_info(cpuLogger, "CPU: Tabla Segundo Nivel: %i", entradaTablaSegundoNivel);
    log_info(cpuLogger, "CPU: Desplazamiento: %i", desplazamiento);

    return 0;
}