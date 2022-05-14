
#include "include/main.h"
#include "include/hilos.h"

//p_thread

t_log* kernel_logger;


int main(int argc, char** argv){ //falta handshake con consola

    kernel_logger = log_create("kernel.log","KERNEL",1,LOG_LEVEL_DEBUG);

    //Abrir server
    t_config* kernel_config=config_create("kernel.config");
    char* ip_kernel=config_get_string_value(kernel_config, "IP_KERNEL");
    char* puerto_kernel=config_get_string_value(kernel_config, "PUERTO_ESCUCHA");

    int server_fd = iniciar_servidor(kernel_logger,"Kernel",ip_kernel,puerto_kernel);
    log_info(kernel_logger, "Servidor Kernel inicializado");


    char* ip_cpu=config_get_string_value(kernel_config, "IP_CPU");
    char* puerto_cpu=config_get_string_value(kernel_config, "PUERTO_CPU");

    int cpu_fd=crear_conexion_a_server(kernel_logger,"Kernel",ip_cpu,puerto_cpu); //Conexion a CPU

    char* ip_memoria=config_get_string_value(kernel_config, "IP_MEMORIA");
    char* puerto_memoria=config_get_string_value(kernel_config, "PUERTO_MEMORIA");

    int memoria_fd=crear_conexion_a_server(kernel_logger,"Kernel",ip_memoria,puerto_memoria); //Conexion a Memoria

    //handshake?

    while(server_escuchar(kernel_logger,"Kernel",server_fd)){

    }; //escuchar peticiones de la consola

    //crear un hilo para atender la coenxion
    //hilo main siempre escucha y el resto atiende
    //otro hilo para el exit


    return 0;
    //cosas


}

//Agregado uno

t_paquete* deserializar_instruccion(op_code opCode,void* stream){ //devuelve una instruccion dps ver q devuelve y faltan los parametros de las de deserializar
    uint32_t param1,param2;

	switch(opCode){
        case(NO_OP):
                return deserializar_no_op(stream,&param1); //ver como pasarle param1 si es *, & o sin nada
                break;
        case(EXIT):
                //return serializar_exit(); //termina el programa
                break;
        case(IO):
                return deserializar_io(stream,&param1);
                break;
        case(READ):
                return deserializar_read(stream,&param1);
                break;
        case(WRITE):
                return deserializar_write(stream,&param1,&param2);
                break;
        case(COPY):
                return deserializar_copy(stream,&param1,&param2); //hay que serializar de kernel a cpu esto? o lo pasa como viene de consola/ serializar pcb?
                break; //mandar todo el pcb.
    }
    return NULL;
}

//Agregado 2

struct pcb *recibir_paquete_instrucciones(int socket_cliente)
{
    t_buffer* buffer=crear_buffer(); //todo
    int size;
    void *stream ;
    struct pcb* pcb ;

    stream = recibir_buffer(&size, socket_cliente);
    buffer->stream = stream ;

    pcb = deserializar_paquete_instrucciones_consola(buffer);
    free(buffer);
    return pcb ;
}
//Agregado 3 revisar algunas cosas todo
struct pcb *deserializar_paquete_instrucciones_consola(t_buffer* buffer) // Para deserializar las instrucciones de consola
 {

     struct pcb* proceso_pcb = malloc(sizeof(pcb)) ; //crear_pcb() para q este todo en NULL y 0
     int indice_split = 0 ;
     void* stream = buffer->stream ;
     char* mensaje_consola ; // leido de consola que se envia en el paquete


     // Deserializar los campos del buffer

    mensaje_consola = malloc(sizeof(strlen(mensaje_consola) + 1));
    memcpy(mensaje_consola, stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);

    char** split_buffer = malloc(sizeof(mensaje_consola));
    split_buffer = string_split(mensaje_consola, "\n");
    char** palabras = malloc(sizeof(split_buffer[indice_split])) ;
    proceso_pcb->instrucciones = list_create();

    while (split_buffer[indice_split] != NULL) {


        if(string_contains(split_buffer[indice_split], "NO_OP") ) {
            palabras = string_split(split_buffer[indice_split], " ") ;
            int parametro_NO_OP = atoi(palabras[1]);
            for(int i=0; i< parametro_NO_OP  ; i++){
                printf("NO_OP %d ", parametro_NO_OP);
                list_add(proceso_pcb->instrucciones,palabras[0]) ; // Para agregar a lista a medida quese vaya parseando
            }

        } else { // si no es no op directamente entra aca y se agrega a la lista
            printf("%s",split_buffer[indice_split]);
            list_add(proceso_pcb->instrucciones,split_buffer[indice_split]); // consultar si esto es lo mejor o volver a la anterior de if para cada uno.
        }

        indice_split++;
    }

    string_array_destroy(split_buffer);
    string_array_destroy(palabras);
    list_destroy(proceso_pcb->instrucciones);
    free(mensaje_consola);
    free(proceso_pcb);

    return proceso_pcb;
}

struct pcb *crear_pcb(){
	struct pcb *nueva_pcb= malloc(sizeof(struct pcb));
	nueva_pcb->id=1;
}

