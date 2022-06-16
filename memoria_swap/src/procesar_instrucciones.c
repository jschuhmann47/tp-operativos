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
                continue;
            case TABLAUNO:
                procesar_entrada_tabla_primer_nv(socket_cpu);
                continue;
            case TABLADOS:
                procesar_entrada_tabla_segundo_nv(socket_cpu);
                continue;
        }

        
    }
}

void procesar_instruccion(void* buffer, int socket_cpu){
    code_instruccion codOp;
    memcpy(&codOp, buffer, sizeof(code_instruccion));
    uint32_t param1;
    uint32_t param2;
    log_info(memoria_swapLogger, "Memoria: Recibi el codigo de operacion: %i", codOp);
    switch (codOp)
    {
    case READ:
        memcpy(&param1, buffer+sizeof(code_instruccion), sizeof(uint32_t));
        log_info(memoria_swapLogger, "Memoria: Recibi READ con parametro: %i", param1);
        procesar_read(param1, socket_cpu); //TODO
        break;
    case WRITE:
        memcpy(&param1, buffer+sizeof(code_instruccion), sizeof(uint32_t));
        memcpy(&param2, buffer+sizeof(code_instruccion)+sizeof(uint32_t), sizeof(uint32_t));
        log_info(memoria_swapLogger, "Memoria: Recibi WRITE con parametros: %i, %i", param1, param2);
        procesar_write(param1, param2, socket_cpu);//TODO
        break;
    default:
        log_error(memoria_swapLogger, "Memoria: Error al leer el codigo de operacion");
        break;
    }
    //devolver a cpu un ok o ver que devuelve en cada caso
}

void procesar_read(uint32_t param, int socket_cpu){ //READ devuelve el valor leido
    log_info(memoria_swapLogger, "Memoria: Procesando READ");
    //...
    log_info(memoria_swapLogger, "Memoria: READ terminado");
}

void procesar_write(uint32_t param1, uint32_t param2, int socket_cpu){ //write no dice, devolver ok o error?
    log_info(memoria_swapLogger, "Memoria: Procesando WRITE");
    //...
    log_info(memoria_swapLogger, "Memoria: WRITE terminado");
}