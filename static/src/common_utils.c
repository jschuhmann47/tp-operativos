#include "common_utils.h"

const uint32_t PCB_IO_RETURN = 1;
const uint32_t PCB_NORMAL_RETURN = 0;

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
	instruccion->indicador = 0; //ver como se inicia si es enum
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

void* serializar_pcb(t_pcb *pcb, uint32_t *bytes)
{
    printf("hola1");
    uint32_t tamanioListaInstrucciones = 0;
    for(int i=0; i < (pcb->instrucciones->elements_count); i++){
        t_instruccion* instruccion = list_get(pcb->instrucciones,i);
        tamanioListaInstrucciones += tamanioInstruccion(instruccion->indicador);
    }
    printf("hola2");
    void *empaquetado = malloc(sizeof(uint32_t) +
                               sizeof(t_status) +
                               sizeof(uint32_t) +
                               sizeof(uint32_t) +
                               tamanioListaInstrucciones +
                               sizeof(uint32_t) +
                               sizeof(double));

    uint32_t offset = 0, tmp_size = 0;
    
    tmp_size = sizeof(uint32_t);
    memcpy(empaquetado + offset, tamanioListaInstrucciones, tmp_size);
    offset += tmp_size;

    tmp_size = sizeof(uint32_t);
    memcpy(empaquetado + offset, &(pcb->id), tmp_size);
    offset += tmp_size;
   
    tmp_size = sizeof(t_status);
    memcpy(empaquetado + offset, &(pcb->status), tmp_size);
    offset += tmp_size;
    
    tmp_size = sizeof(uint32_t);
    memcpy(empaquetado + offset, &(pcb->tamanio), tmp_size);
    offset += tmp_size;
    printf("hola3");
    for(int i=0; i < (pcb->instrucciones->elements_count); i++){
        t_instruccion* instruccion = list_get(pcb->instrucciones,i);
        memcpy(empaquetado + offset, &(instruccion->indicador), tmp_size = sizeof(code_instruccion));
        offset += tmp_size;
        printf("hola4");
        switch (instruccion->indicador){
            case NO_OP:
                break;
                printf("holaNOOP");
            case I_O:
                memcpy(empaquetado + offset, list_get(instruccion->parametros,0), tmp_size = sizeof(uint32_t));
                offset += tmp_size;
                printf("holaIO");
                break;
            case READ:
                memcpy(empaquetado + offset, list_get(instruccion->parametros,0), tmp_size = sizeof(uint32_t));
                offset += tmp_size;
                printf("READ");
                break;
            case WRITE:
                memcpy(empaquetado + offset, list_get(instruccion->parametros,0), tmp_size = sizeof(uint32_t));
                offset += tmp_size;
                memcpy(empaquetado + offset, list_get(instruccion->parametros,1), tmp_size = sizeof(uint32_t));
                offset += tmp_size;
                printf("WRITE");
                break;
            case COPY:
                memcpy(empaquetado + offset, list_get(instruccion->parametros,0), tmp_size = sizeof(uint32_t));
                offset += tmp_size;
                memcpy(empaquetado + offset, list_get(instruccion->parametros,1), tmp_size = sizeof(uint32_t));
                offset += tmp_size;
                printf("COPY");
                break;
            case EXIT_I:
                printf("EEXIT");
                break;
        }
    }
    printf("hola5");
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

t_pcb* recibir_pcb(void* buffer)
{
    uint32_t offset = 0, tmp_len = 0;
    
    
    uint32_t tamanioListaInstrucciones;
    printf("hola1");
    memcpy(&tamanioListaInstrucciones, buffer + offset, tmp_len = sizeof(uint32_t));
    offset += tmp_len;

    t_list* instruccionesPcb = list_create();

    t_pcb *pcb = malloc(sizeof(uint32_t) +
                        sizeof(t_status) +
                        sizeof(uint32_t) +
                        sizeof(uint32_t) +
                        tamanioListaInstrucciones +
                        sizeof(uint32_t) +
                        sizeof(double));

    printf("hola2");
    memcpy(&pcb->id, buffer + offset, tmp_len = sizeof(uint32_t));
    offset += tmp_len;

    memcpy(&pcb->status, buffer + offset, tmp_len = sizeof(t_status));
    offset += tmp_len;

    memcpy(&pcb->tamanio, buffer + offset, tmp_len = sizeof(uint32_t));
    offset += tmp_len;

    uint32_t cantidadInstrucciones;
    memcpy(&cantidadInstrucciones, buffer + offset, tmp_len = sizeof(uint32_t));
    offset += tmp_len;
    
    printf("hola3");
    for (int i = 0; i < cantidadInstrucciones; i++){
        t_instruccion *instruccion = crear_instruccion();
        memcpy(&instruccion->indicador, buffer + offset, tmp_len = sizeof(code_instruccion));
        offset += tmp_len;
        uint32_t param1,param2;
        printf("hola4");
        switch (instruccion->indicador){
            case NO_OP:
                break;
            case I_O:
                //uint32_t param1;
                memcpy(&param1, buffer + offset, tmp_len = sizeof(uint32_t));
                offset += tmp_len;
                
                list_add(instruccion->parametros, &param1);
                printf("%d\n", param1);
                list_add(instruccionesPcb, instruccion);
                printf("io");
                break;
            case READ:
                //uint32_t param1;
                memcpy(&param1, buffer + offset, tmp_len = sizeof(uint32_t));
                offset += tmp_len;
                list_add(instruccion->parametros, &param1);
                list_add(instruccionesPcb, instruccion);
                printf("read");
                break;
            case WRITE:
                //uint32_t param1,param2;
                memcpy(&param1, buffer + offset, tmp_len = sizeof(uint32_t));
                offset += tmp_len;
                memcpy(&param2, buffer + offset, tmp_len = sizeof(uint32_t));
                offset += tmp_len;
                list_add(instruccion->parametros, &param1);
                list_add(instruccion->parametros, &param2);
                list_add(instruccionesPcb, instruccion);
                printf("write");
                break;
            case COPY:
                //uint32_t param1,param2;
                memcpy(&param1, buffer + offset, tmp_len = sizeof(uint32_t));
                offset += tmp_len;
                memcpy(&param2, buffer + offset, tmp_len = sizeof(uint32_t));
                offset += tmp_len;
                list_add(instruccion->parametros, &param1);
                list_add(instruccion->parametros, &param2);
                list_add(instruccionesPcb, instruccion);
                printf("copy");
                break;
            case EXIT_I:
                break;
        }
        list_add(pcb->instrucciones, instruccion);
    }
    printf("holazp");
    memcpy(&pcb->programCounter, buffer + offset, tmp_len = sizeof(typeof(pcb->programCounter)));
    offset += tmp_len;

    memcpy(&pcb->est_rafaga_actual, buffer + offset, tmp_len = sizeof(typeof(pcb->est_rafaga_actual)));

    pcb->instrucciones=instruccionesPcb;
    return pcb;
}

// uint32_t serializar_instruccion_de_pcb(t_instruccion* instruccion){
//     uint32_t tamanioTotal = 0;
//     tamanioTotal += sizeof(strlen(instruccion->indicador)+1);
//     code_instruccion cod_op = getCodeIntruccion(instruccion);

//             switch(cod_op)
//             {
//                 case NO_OP:
//                     return tamanioTotal;
//                 case I_O:
//                     tamanioTotal += sizeof(strlen(list_get(instruccion->parametros,0)+1));
//                     return tamanioTotal;                    
//                 case WRITE:
//                     tamanioTotal += sizeof(strlen(list_get(instruccion->parametros,0)+1));
//                     tamanioTotal += sizeof(strlen(list_get(instruccion->parametros,1)+1));
//                     return tamanioTotal;
//                 case COPY:
//                     tamanioTotal += sizeof(strlen(list_get(instruccion->parametros,0)+1));
//                     tamanioTotal += sizeof(strlen(list_get(instruccion->parametros,1)+1));
//                     return tamanioTotal;
//                 case READ:
//                     tamanioTotal += sizeof(strlen(list_get(instruccion->parametros,0)+1));
//                     return tamanioTotal;
//                 case EXIT_I:
//                     return tamanioTotal;
//                 default:
//                     break;
//             }
// }

// t_instruccion* deserializar_instruccion_de_pcb(char* buffer, uint32_t* offset){
//     t_instruccion* instruccion = crear_instruccion();
//     uint32_t codOpTamanio;
//     memcpy(&codOpTamanio, buffer + *offset, sizeof(uint32_t));
//     *offset += sizeof(uint32_t);
//     char* codOp = malloc(codOpTamanio);
//     memcpy(&codOp, buffer + *offset, codOpTamanio);
//     *offset += codOpTamanio;
//     switch(getCodeIntruccion(codOp)){
//         case NO_OP:
//             instruccion->indicador = "NO_OP";
//             instruccion->parametros=NULL;
//             break;
//         case I_O:
//             instruccion->indicador = "I/O";
//             uint32_t param1;
//             memcpy(&param1, buffer + *offset, sizeof(uint32_t));
//             *offset += sizeof(uint32_t);
//             list_add(instruccion->parametros,param1);
//             break;
//         case WRITE:
//             instruccion->indicador = "WRITE";
//             break;
//         case COPY:
//             instruccion->indicador = "COPY";
//             break;
//         case READ:
//             instruccion->indicador = "READ";
//             break;
//         case EXIT_I:
//             instruccion->indicador = "EXIT";
//             break;
//         default:
//             break;
//     }
// }

uint32_t string_to_uint(char* string){
	uint32_t result = 0;
    int len = strlen(string);

	for(int i=0; i<len; i++){
		result = result * 10 + ( string[i] - '0' );
	}

	return result;
}

int tamanioInstruccion(op_code codOp){
	switch (codOp)
	{
		case NO_OP:
			return sizeof(uint32_t);
			break;
		case EXIT:
			return sizeof(uint32_t);
			break;
		case I_O:
			return sizeof(uint32_t)*2;
			break;
		case READ:
			return sizeof(uint32_t)*2;
			break;
		case WRITE:
			return sizeof(uint32_t)*3;
			break;
		case COPY:
			return sizeof(uint32_t)*3;
			break;
	}
}