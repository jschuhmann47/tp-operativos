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

code_instruccion getCodeIntruccion(char *code){
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

int sizeArray(char **array)
{
    int cantidad = 0;

    for (cantidad; array[cantidad] != NULL; cantidad++)
        ;

    return cantidad;
}

uint32_t string_to_uint(char* string){
    uint32_t result = 0;
    int len = strlen(string);

    for(int i=0; i<len; i++){
        result = result * 10 + ( string[i] - '0' );
    }

    return result;
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
    
    uint32_t tamanioListaInstrucciones = 0;
    for(int i=0; i < (pcb->instrucciones->elements_count); i++){
        t_instruccion* instruccion = list_get(pcb->instrucciones,i);
        tamanioListaInstrucciones += tamanioInstruccion(instruccion->indicador);
    }
    
    void *empaquetado = malloc(tamanioListaInstrucciones +
                                sizeof(uint32_t) +
                                sizeof(typeof(pcb->id)) + //id
                                sizeof(t_status) +
                                sizeof(typeof(pcb->tamanio)) +//tamanio
                                sizeof(uint32_t) +//cantidadInstrucciones
                                sizeof(typeof(pcb->programCounter)) +//PC
                                sizeof(typeof(pcb->est_rafaga_actual))+
                                sizeof(typeof(pcb->dur_ultima_rafaga))+ //rafaga
                                sizeof(typeof(pcb->rafaga_instante_actual))+
                                sizeof(typeof(pcb->acum_rafaga))+
                                sizeof(typeof(pcb->tablaDePaginas)));//cantidadPaginas

    uint32_t offset = 0, tmp_size = 0;
    
    tmp_size = sizeof(tamanioListaInstrucciones);
    memcpy(empaquetado + offset, &tamanioListaInstrucciones, tmp_size);
    offset += tmp_size;

    tmp_size = sizeof(pcb->id);
    memcpy(empaquetado + offset, &(pcb->id), tmp_size);
    offset += tmp_size;
   
    tmp_size = sizeof(t_status);
    memcpy(empaquetado + offset, &(pcb->status), tmp_size);
    offset += tmp_size;
    
    tmp_size = sizeof(pcb->tamanio);
    memcpy(empaquetado + offset, &(pcb->tamanio), tmp_size);
    offset += tmp_size;

    tmp_size = sizeof(uint32_t);
    memcpy(empaquetado + offset, &(pcb->instrucciones->elements_count), tmp_size);
    offset += tmp_size;

    for(int i=0; i < (pcb->instrucciones->elements_count); i++){
        t_instruccion* instruccion = list_get(pcb->instrucciones,i);
        
        memcpy(empaquetado + offset, &(instruccion->indicador), tmp_size = sizeof(code_instruccion));
        offset += tmp_size;
       
        switch (instruccion->indicador){
            case NO_OP:
                continue;
            case I_O:
                memcpy(empaquetado + offset, list_get(instruccion->parametros,0), tmp_size = sizeof(uint32_t));
                offset += tmp_size;
                
                continue;
            case READ:
                memcpy(empaquetado + offset, list_get(instruccion->parametros,0), tmp_size = sizeof(uint32_t));
                offset += tmp_size;
                
                continue;
            case WRITE:
                memcpy(empaquetado + offset, list_get(instruccion->parametros,0), tmp_size = sizeof(uint32_t));
                offset += tmp_size;
                memcpy(empaquetado + offset, list_get(instruccion->parametros,1) , tmp_size = sizeof(uint32_t));
                offset += tmp_size;
                continue;
            case COPY:
                memcpy(empaquetado + offset, list_get(instruccion->parametros,0), tmp_size = sizeof(uint32_t));
                offset += tmp_size;
                memcpy(empaquetado + offset, list_get(instruccion->parametros,1) , tmp_size = sizeof(uint32_t));
                offset += tmp_size;
                continue;
            case EXIT_I:
                continue;
        }
    }
    
    
    tmp_size = sizeof(pcb->programCounter);
    memcpy(empaquetado + offset, &(pcb->programCounter), tmp_size);
    offset += tmp_size;

    tmp_size = sizeof(pcb->est_rafaga_actual);
    memcpy(empaquetado + offset, &(pcb->est_rafaga_actual), tmp_size);
    offset += tmp_size;

    tmp_size = sizeof(pcb->dur_ultima_rafaga);
    memcpy(empaquetado + offset, &(pcb->dur_ultima_rafaga), tmp_size);
    offset += tmp_size;

    tmp_size = sizeof(pcb->rafaga_instante_actual);
    memcpy(empaquetado + offset, &(pcb->rafaga_instante_actual), tmp_size);
    offset += tmp_size;

    tmp_size = sizeof(pcb->acum_rafaga);
    memcpy(empaquetado + offset, &(pcb->acum_rafaga), tmp_size);
    offset += tmp_size;

    tmp_size = sizeof(pcb->tablaDePaginas);
    memcpy(empaquetado + offset, &(pcb->tablaDePaginas), tmp_size);
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

t_pcb* recibir_pcb(void* buffer,uint32_t bytes)
{
    uint32_t offset = 0, tmp_len = 0;
    uint32_t tamanioListaInstrucciones;
    
    memcpy(&tamanioListaInstrucciones, buffer + offset, tmp_len = sizeof(tamanioListaInstrucciones));
    offset += tmp_len;

    t_pcb *pcb = malloc(bytes);
    pcb->instrucciones = list_create();

    memcpy(&pcb->id, buffer + offset, tmp_len = sizeof(typeof(pcb->id)));
    offset += tmp_len;

    memcpy(&pcb->status, buffer + offset, tmp_len = sizeof(t_status));
    offset += tmp_len;

    memcpy(&pcb->tamanio, buffer + offset, tmp_len = sizeof(typeof(pcb->tamanio)));
    offset += tmp_len;

    uint32_t cantidadInstrucciones;
    memcpy(&cantidadInstrucciones, buffer + offset, tmp_len = sizeof(uint32_t));
    offset += tmp_len;
    
    for (int i = 0; i < cantidadInstrucciones; i++){
        t_instruccion *instruccion = crear_instruccion();
        memcpy(&instruccion->indicador, buffer + offset, tmp_len = sizeof(code_instruccion));
        offset += tmp_len;
        uint32_t* param1=malloc(sizeof(uint32_t));
        uint32_t* param2=malloc(sizeof(uint32_t)); //free
        
        switch (instruccion->indicador){
            case NO_OP:
                list_add(pcb->instrucciones, instruccion);
                free(param1);
                free(param2);
                continue;
            case I_O:
                memcpy(param1, buffer + offset, tmp_len = sizeof(uint32_t));
                offset += tmp_len;
                list_add(instruccion->parametros, param1);
                list_add(pcb->instrucciones, instruccion);
                free(param2);
                continue;
            case READ:
                memcpy(param1, buffer + offset, tmp_len = sizeof(uint32_t));
                offset += tmp_len;
                list_add(instruccion->parametros, param1);
                list_add(pcb->instrucciones, instruccion);
                free(param2);
                continue;
            case WRITE:
                memcpy(param1, buffer + offset, tmp_len = sizeof(uint32_t));
                offset += tmp_len;
                list_add(instruccion->parametros, param1);
                memcpy(param2, buffer + offset, tmp_len = sizeof(uint32_t));
                offset += tmp_len;
                list_add(instruccion->parametros, param2);
                list_add(pcb->instrucciones, instruccion);
                continue;
            case COPY:
                memcpy(param1, buffer + offset, tmp_len = sizeof(uint32_t));
                offset += tmp_len;
                list_add(instruccion->parametros, param1);
                memcpy(param2, buffer + offset, tmp_len = sizeof(uint32_t));
                offset += tmp_len;
                list_add(instruccion->parametros, param2);
                list_add(pcb->instrucciones, instruccion);
                continue;
            case EXIT_I:
                list_add(pcb->instrucciones, instruccion);
                free(param1);
                free(param2);
                continue;
        }
    }
    
    memcpy(&pcb->programCounter, buffer + offset, tmp_len = sizeof(typeof(pcb->programCounter)));
    offset += tmp_len;

    memcpy(&pcb->est_rafaga_actual, buffer + offset, tmp_len = sizeof(typeof(pcb->est_rafaga_actual)));
    offset += tmp_len;

    memcpy(&pcb->dur_ultima_rafaga, buffer + offset, tmp_len = sizeof(typeof(pcb->dur_ultima_rafaga)));
    offset += tmp_len;

    memcpy(&pcb->rafaga_instante_actual, buffer + offset, tmp_len = sizeof(typeof(pcb->rafaga_instante_actual)));
    offset += tmp_len;

    memcpy(&pcb->acum_rafaga, buffer + offset, tmp_len = sizeof(typeof(pcb->acum_rafaga)));
    offset += tmp_len;

    memcpy(&pcb->tablaDePaginas, buffer + offset, tmp_len = sizeof(typeof(pcb->tablaDePaginas)));

    free(buffer);

    return pcb;
}

uint32_t tamanioInstruccion(code_instruccion codOp){
	switch (codOp)
	{
		case NO_OP:
			return sizeof(code_instruccion);
		break;
		case I_O:
			return sizeof(uint32_t)+sizeof(code_instruccion);
		break;
		case WRITE:
			return (sizeof(uint32_t)*2)+sizeof(code_instruccion);
		break;
		case COPY:
			return (sizeof(uint32_t)*2)+sizeof(code_instruccion);
		break;
        case READ:
			return sizeof(uint32_t)+sizeof(code_instruccion);
		break;
        case EXIT_I:
			return sizeof(code_instruccion);
		break;
        default:
        break;
	}
}

int mandar_instruccion(code_instruccion codOp,uint32_t param1,uint32_t param2,int socket,t_log* logger){
    if(codOp != WRITE && codOp != READ){
        log_error(logger, "Error al mandar instruccion a memoria, codigo de operacion invalido");
        return -1;
    }
    
    op_code opCode = INSTRUCCION;
    if(send(socket,&opCode,sizeof(op_code),0)<0){
        log_error(logger, "Error al mandar instruccion a memoria, no se pudo enviar el codigo de operacion");
    }
    
    uint32_t bytes = tamanioInstruccion(codOp);

    if(send(socket,&bytes,sizeof(uint32_t),0)<0){
        log_error(logger, "Error al mandar instruccion a memoria, no se pudo enviar el tamanio de la instruccion");
        return -1;
    }

    void* buffer = malloc(bytes);
    uint32_t offset = 0;

    memcpy(buffer, &codOp, sizeof(code_instruccion));
    offset += sizeof(code_instruccion);
    memcpy(buffer + offset, &param1, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    if(codOp == WRITE){
        memcpy(buffer + offset, &param2, sizeof(uint32_t));
        offset += sizeof(uint32_t);
    }

    if(send(socket, buffer, bytes, 0)){
        log_info(logger, "Se mando instruccion a memoria");
        free(buffer);
        return 1;
    }
    else {
        log_error(logger, "Error al mandar instruccion a memoria");
        free(buffer);
        return -1;
    }
}

void pcb_destroy(t_pcb *pcb) {
    list_destroy_and_destroy_elements(pcb->instrucciones, destruir_instruccion);
    free(pcb);
}

void destruir_instruccion(t_instruccion* instruccion) {
    list_destroy_and_destroy_elements(instruccion->parametros, free);
    free(instruccion);
}

t_log_level determinar_nivel_de_log(char* arg){
    if(strcmp("info",arg)==0){
        return LOG_LEVEL_INFO;
    }
    if(strcmp("debug",arg)==0){
        return LOG_LEVEL_DEBUG;
    }
}

