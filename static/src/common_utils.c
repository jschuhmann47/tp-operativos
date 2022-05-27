#include "common_utils.h"

///////////////////////////// CONFIGS /////////////////////////////
int cargar_configuracion(const char *nombreModulo, void *moduleCfg, char *configPath, t_log *logger,
                         void (*cargar_miembros)(void *cfg, t_config *localCfg))
{
    t_config *localCfg = config_create(configPath);
    if (localCfg == NULL)
    {
        log_error(logger, "No se encontró %s", configPath);
        return EXIT_FAILURE;
    }
    cargar_miembros(moduleCfg, localCfg);
    log_info(logger, "%s: Inicialización de campos correcta", nombreModulo);
    config_destroy(localCfg);
    return EXIT_SUCCESS;
}

code_instruccion getCodeIntruccion(char *code)
{
    if (string_equals_ignore_case(code, "NO_OP"))
        return NO_OP;
    if (string_equals_ignore_case(code, "WRITE"))
        return WRITE;
    if (string_equals_ignore_case(code, "READ"))
        return READ;
    if (string_equals_ignore_case(code, "COPY"))
        return COPY;
    if (string_equals_ignore_case(code, "EXIT") || string_equals_ignore_case(code, "EXIT_I"))
        return EXIT_I;
    if (string_equals_ignore_case(code, "I/O") || string_equals_ignore_case(code, "I_O"))
        return I_O;
        
}

char* agregarInstruccion(char **intruccion, char *mensaje)
{

    char *dato = string_new();
    char *mensajeBuffer = string_new();

    string_append(&dato, "[");

    code_instruccion code = getCodeIntruccion(intruccion[0]);

    switch (code)
    {
    case NO_OP:
        string_append(&dato, "NO_OP");
        break;
    case EXIT_I:
        string_append(&dato, "EXIT_I");
        break;
    case I_O:
        string_append(&dato, "I_O");
        string_append(&dato, "|");
        string_append(&dato,intruccion[1]);
        break;
    case WRITE:
        string_append(&dato, "WRITE");
        string_append(&dato, "|");
        string_append(&dato, intruccion[1]);
        string_append(&dato, "|");
        string_append(&dato, intruccion[2]);
        break;
    case READ:
        string_append(&dato, "READ");
        string_append(&dato, "|");
        string_append(&dato, intruccion[1]);
        break;
    case COPY:
        string_append(&dato, "COPY");
        string_append(&dato, "|");
        string_append(&dato, intruccion[1]);
        string_append(&dato, "|");
        string_append(&dato, intruccion[2]);
        break;
    default:
        break;
    }
    string_trim_right(&dato);
    string_append(&dato, "]");

    if (string_length(mensaje))
    {

        uint32_t tam = string_length(mensaje) - 1;
        mensajeBuffer = string_substring(mensaje, 0, tam);
        puts(mensajeBuffer);
        string_append(&mensajeBuffer, ",");
        string_append(&mensajeBuffer, dato);
        string_append(&mensajeBuffer, "]");
        puts(mensajeBuffer);
    }
    else
    {

        string_append(&mensajeBuffer, "[");
        string_append(&mensajeBuffer, dato);
        string_append(&mensajeBuffer, "]");
    }

    free(dato);
    return mensajeBuffer;
}

t_instruccion *crear_instruccion(void)
{
	t_instruccion *instruccion = malloc(sizeof(t_instruccion));
	instruccion->indicador = NULL;
	instruccion->parametros = list_create();
	return instruccion;
}

int sizeArray(void **array)
{
    int cantidad = 0;

    for (cantidad; array[cantidad] != NULL; cantidad++)
        ;

    return cantidad;
}


uint32_t enviar_tamanio_mensaje(t_mensaje_tamanio *msjeTamanio, int socket) 
{
    uint32_t bytes = 0, rta;
    char *buf = serializar_tamanio(msjeTamanio, &bytes);
    rta = send(socket, buf, bytes, 0);
    free(buf);
    return rta;
}

char *serializar_tamanio(t_mensaje_tamanio *mensaje_tamanio, uint32_t *bytes) 
{
    char *empaquetado = malloc(sizeof(typeof(mensaje_tamanio->tamanio)));
    memset(empaquetado, 0, sizeof(typeof(mensaje_tamanio->tamanio)));
    uint32_t offset = 0, tmp_size = 0;
    memcpy(empaquetado + offset, &mensaje_tamanio->tamanio, tmp_size = sizeof(typeof(mensaje_tamanio->tamanio)));
    offset += tmp_size;
    *bytes = offset;
    return empaquetado;
}

char* serializar_pcb(t_pcb *pcb, uint32_t *bytes)
{
    char *empaquetado = malloc(sizeof(typeof(pcb->id)) +
                               sizeof(t_status) +
                               sizeof(typeof(pcb->tamanio)) +
                               sizeof(t_list) +
                               sizeof(typeof(pcb->programCounter)) +
                               sizeof(typeof(pcb->est_rafaga_actual)));

    uint32_t offset = 0, tmp_size = 0;
    tmp_size = sizeof(pcb->id);
    memcpy(empaquetado + offset, &(pcb->id), tmp_size);
    offset += tmp_size;
    tmp_size = sizeof(pcb->status);
    memcpy(empaquetado + offset, &(pcb->status), tmp_size);
    offset += tmp_size;
    tmp_size = sizeof(pcb->tamanio);
    memcpy(empaquetado + offset, &(pcb->tamanio), tmp_size);
    offset += tmp_size;
    tmp_size = sizeof(pcb->instrucciones);
    memcpy(empaquetado + offset, &(pcb->instrucciones), tmp_size);
    offset += tmp_size;
    tmp_size = sizeof(pcb->programCounter);
    memcpy(empaquetado + offset, &(pcb->programCounter), tmp_size);
    offset += tmp_size;
    tmp_size = sizeof(pcb->est_rafaga_actual);
    memcpy(empaquetado + offset, &(pcb->est_rafaga_actual), tmp_size);
    offset += tmp_size;

    *bytes = offset;
    return empaquetado;
}

uint32_t recibir_tamanio_mensaje(t_mensaje_tamanio *tamanio_mensaje, int socket)
{
    uint32_t response;
    t_mensaje_tamanio *message_aux;
    uint32_t buffer_size = sizeof(t_mensaje_tamanio);
    char *buffer = malloc(buffer_size);
    if (!recv(socket, buffer, buffer_size, MSG_WAITALL)) {
        response = 0;
    } else {
        message_aux = deserializar_tamanio(buffer);
        *tamanio_mensaje = *message_aux;
        free(message_aux);
        response = 1;
    }
    free(buffer);
    return response;
}

t_mensaje_tamanio* deserializar_tamanio(char *buffer) {
    t_mensaje_tamanio *header_message = malloc(sizeof(t_mensaje_tamanio));
    uint32_t offset = 0, tmp_len = 0;
    memcpy(&header_message->tamanio, buffer + offset, tmp_len = sizeof(typeof(header_message->tamanio)));
    return header_message;
}

t_pcb* recibir_pcb(char* buffer)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
    pcb->instrucciones = list_create();

    uint32_t offset = 0, tmp_len = 0;
    memcpy(&pcb->id, buffer + offset, tmp_len = sizeof(typeof(pcb->id)));
    offset += tmp_len;
    memcpy(&pcb->status, buffer + offset, tmp_len = sizeof(t_status));
    offset += tmp_len;
    memcpy(&pcb->tamanio, buffer + offset, tmp_len = sizeof(typeof(pcb->tamanio)));
    offset += tmp_len;
    memcpy(&pcb->instrucciones, buffer + offset, tmp_len = sizeof(t_list));
    offset += tmp_len;
    memcpy(&pcb->programCounter, buffer + offset, tmp_len = sizeof(typeof(pcb->programCounter)));
    offset += tmp_len;
    memcpy(&pcb->est_rafaga_actual, buffer + offset, tmp_len = sizeof(typeof(pcb->est_rafaga_actual)));

    return pcb;
}

