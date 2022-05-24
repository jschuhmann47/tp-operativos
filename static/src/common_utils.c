#include "common_utils.h"

///////////////////////////// CONFIGS /////////////////////////////
int cargar_configuracion(const char *nombreModulo, void *moduleCfg, char *configPath, t_log *logger,
                         void (*cargar_miembros)(void *cfg, t_config *localCfg))
{
    t_config *localCfg = config_create(configPath);
    if (localCfg == NULL)
    {
        log_error(logger, "No se encontró ", configPath);
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

void* serializar_pcb(t_pcb *pcb)
{
    void *buffer = malloc(sizeof(t_pcb));
    memcpy(buffer, pcb, sizeof(t_pcb)); //yo capaz q lo haria uno por uno para saber que es cada cosa, x ahora lo dejo asi
    return buffer;
}

// struct t_pcb
// {
//     uint32_t id;        
//     t_status status;
//     uint32_t tamanio;
//     t_list *instrucciones;
//     void *programCounter;
//     // TODO: Tabla de paginas
//     // Estos dos ultimos solo se usan cuando es SRT
//     double est_rafaga_actual; // Esta en Milisegundos
//     void (*algoritmo_siguiente_estim)(t_pcb *self, time_t tiempoFinal, time_t tiempoInicial);
// };