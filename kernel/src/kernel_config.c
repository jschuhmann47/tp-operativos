#include "kernel_config.h"

t_kernel_config* kernel_cfg_create(void) {
    t_kernel_config* newKernelCfg = malloc(sizeof(t_kernel_config));
    newKernelCfg->CONSOLA_SOCKET = 0;
    newKernelCfg->IP_MEMORIA = NULL;
    newKernelCfg->PUERTO_MEMORIA = NULL;
    newKernelCfg->IP_CPU = NULL;
    newKernelCfg->IP_KERNEL = NULL;
    newKernelCfg->PUERTO_CPU_DISPATCH = NULL;
    newKernelCfg->PUERTO_CPU_INTERRUPT = NULL;
    newKernelCfg->PUERTO_ESCUCHA = NULL;
    newKernelCfg->ALGORITMO_PLANIFICACION = NULL;
    newKernelCfg->ESTIMACION_INICIAL = 0;
    newKernelCfg->ALFA = 0;
    newKernelCfg->GRADO_MULTIPROGRAMACION = 0;
    newKernelCfg->TIEMPO_MAXIMO_BLOQUEADO = 0;
    return newKernelCfg;
}

void kernel_config_initialize(void* kernelCfg, t_config* config) {
    t_kernel_config* cfg = (t_kernel_config*) kernelCfg;
    cfg->IP_MEMORIA = strdup(config_get_string_value(config, "IP_MEMORIA"));
    cfg->PUERTO_MEMORIA = strdup(config_get_string_value(config, "PUERTO_MEMORIA"));
    cfg->IP_CPU = strdup(config_get_string_value(config, "IP_CPU"));
    cfg->IP_KERNEL = strdup(config_get_string_value(config, "IP_KERNEL"));
    cfg->PUERTO_CPU_DISPATCH = strdup(config_get_string_value(config, "PUERTO_CPU_DISPATCH"));
    cfg->PUERTO_CPU_INTERRUPT = strdup(config_get_string_value(config, "PUERTO_CPU_INTERRUPT"));
    cfg->PUERTO_ESCUCHA = strdup(config_get_string_value(config, "PUERTO_ESCUCHA"));
    cfg->ALGORITMO_PLANIFICACION = strdup(config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
    cfg->ESTIMACION_INICIAL = config_get_double_value(config, "ESTIMACION_INICIAL");
    cfg->ALFA = config_get_double_value(config, "ALFA");
    cfg->GRADO_MULTIPROGRAMACION = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
    cfg->TIEMPO_MAXIMO_BLOQUEADO = config_get_int_value(config, "TIEMPO_MAXIMO_BLOQUEADO");
}

void liberar_modulo_kernel(t_log* kernelLogger, t_kernel_config* kernelCfg) {
    log_destroy(kernelLogger);
    close(kernelCfg->CONSOLA_SOCKET);
    free(kernelCfg->IP_MEMORIA);
    free(kernelCfg->PUERTO_MEMORIA);
    free(kernelCfg->IP_CPU);
    free(kernelCfg->IP_KERNEL);
    free(kernelCfg->PUERTO_CPU_DISPATCH);
    free(kernelCfg->PUERTO_CPU_INTERRUPT);
    free(kernelCfg->PUERTO_ESCUCHA);
    free(kernelCfg->ALGORITMO_PLANIFICACION);
    free(kernelCfg);
}

op_code recibir_operacion(int socket_cliente)
{
	op_code cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(op_code), MSG_WAITALL) > 0)
		return cod_op;
	else{
		close(socket_cliente);
		return -1;
	}
}

char* recibir_mensaje(int socket_cliente)
{
	char* buffer = recibir_buffer(socket_cliente);

    return buffer;
}

t_list* recibir_instrucciones(int socket_cliente)
{
    char* buffer = recibir_buffer(socket_cliente);
    log_info(kernelLogger, "Me llego el mensaje %s", buffer);

    t_list* instrucciones = convertir_instruccion(buffer);

    free(buffer);

    return instrucciones;
}

void* recibir_buffer(int socket_cliente)
{
	void *buffer;
    uint32_t size = 0;

	recv(socket_cliente, &size, sizeof(uint32_t), MSG_WAITALL);
	buffer = malloc(size);
	recv(socket_cliente, buffer, size, MSG_WAITALL);

	return buffer;
}

t_list* convertir_instruccion(char* buffer)
{
    t_list* instrucciones = list_create();
    char **descript = string_get_string_as_array(buffer);
    uint32_t tamanio = sizeArray(descript);
    for(int i = 0; i < tamanio; i++)
    {
        if(string_length(buffer))
        {
            t_instruccion* instruccion = crear_instruccion();

            char *instruccionConPipe = string_substring(descript[i], 1, (string_length(descript[i]) - 2));
            char** pseudoInstruccion = string_split(instruccionConPipe, "|");
            char* instruccionSinPipe = pseudoInstruccion[0];
            char* primerParametroSinPipe;
            char* segundoParametroSinPipe;

            code_instruccion cod_op = getCodeIntruccion(instruccionSinPipe);
            uint32_t* param1=malloc(sizeof(uint32_t));
            uint32_t* param2=malloc(sizeof(uint32_t));
            switch(cod_op)
            {
                case NO_OP:
                    free(param1);
                    free(param2);
                    instruccion->indicador = NO_OP;
                    list_add(instrucciones, instruccion);
                    break;
                case I_O:
                    free(param2);   
                    instruccion->indicador = I_O;
                    primerParametroSinPipe = pseudoInstruccion[1];
                    *param1=string_to_uint(primerParametroSinPipe);
                    list_add(instruccion->parametros, param1);
                    list_add(instrucciones, instruccion);
                    break;
                case WRITE:
                    instruccion->indicador = WRITE;
                    primerParametroSinPipe = pseudoInstruccion[1];
                    segundoParametroSinPipe = pseudoInstruccion[2];
                    *param1=string_to_uint(primerParametroSinPipe);
                    list_add(instruccion->parametros, param1);
                    *param2=string_to_uint(segundoParametroSinPipe);
                    list_add(instruccion->parametros, param2);
                    list_add(instrucciones, instruccion);
                    break;
                case COPY:
                    instruccion->indicador = COPY;
                    primerParametroSinPipe = pseudoInstruccion[1];
                    segundoParametroSinPipe = pseudoInstruccion[2];
                    *param1=string_to_uint(primerParametroSinPipe);
                    list_add(instruccion->parametros, param1);
                    *param2=string_to_uint(segundoParametroSinPipe);
                    list_add(instruccion->parametros, param2);
                    list_add(instrucciones, instruccion);
                    break;
                case READ:
                    free(param2);
                    instruccion->indicador = READ;
                    primerParametroSinPipe = pseudoInstruccion[1];
                    *param1=string_to_uint(primerParametroSinPipe);
                    list_add(instruccion->parametros, param1);
                    list_add(instrucciones, instruccion);
                    break;
                case EXIT_I:
                    free(param1);
                    free(param2);
                    instruccion->indicador = EXIT_I;
                    list_add(instrucciones, instruccion);
                    break;
                default:
                    break;
            }
        }
    }
    return instrucciones;
}

void enviar_finalizacion_consola(char *mensaje, int socket_cliente)
{
	t_buffer *buffer = malloc(sizeof(t_buffer));
	char *datos = string_new();
	string_append(&datos, mensaje);

	buffer->size = strlen(datos) + 1;
	buffer->stream = datos;
	memcpy(buffer->stream, mensaje, buffer->size);

	uint32_t bytes = buffer->size + sizeof(uint32_t);

	void *a_enviar = serializar_mensaje(buffer, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void *serializar_mensaje(t_buffer *mensaje, uint32_t bytes)
{
	void *buffer = malloc(bytes);

	if (buffer == NULL)
	{
		printf("No hay memoria para buffer en serializar_mensaje");
	};

	uint32_t bytes_escritos = 0;

	memcpy(buffer + bytes_escritos, &(mensaje->size), sizeof(uint32_t));
	bytes_escritos += sizeof(uint32_t);
	memcpy(buffer + bytes_escritos, mensaje->stream, mensaje->size);
	bytes_escritos += mensaje->size;

	return buffer;
}