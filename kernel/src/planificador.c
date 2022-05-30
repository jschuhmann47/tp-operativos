#include "planificador.h"

const char* INT_KEYWORD = "interrupcion";
// TODO: PASAJES EXEC => BLOCKED | BLOCKED => READY | SUSBLOCKED => SUSREADY | EXEC => EXIT

// Semaforos
sem_t gradoMultiprog;
sem_t hayPCBsParaAgregarAlSistema;
sem_t hayPCBsParaPasarASusBlocked;
sem_t transicionarSusReadyAready;
sem_t suspensionConcluida;
pthread_mutex_t mutexId;

//ID para el PCB
static uint32_t nextId;

// Colas de planificación
t_cola_planificacion* pcbsNew;
t_cola_planificacion* pcbsReady;
t_cola_planificacion* pcbsExec;
t_cola_planificacion* pcbsBlocked;
t_cola_planificacion* pcbsSusReady;
t_cola_planificacion* pcbsSusBlocked;
t_cola_planificacion* pcbsExit;

//Socket para dispatch
int SOCKET_DISPATCH;

//Socket para interrupt
int SOCKET_INTERRUPT;

void iniciar_planificacion(void) {


    nextId = 1;
    /* Inicializacion de semaforos */
    sem_init(&gradoMultiprog, 0, kernelCfg->GRADO_MULTIPROGRAMACION);   /* contador */
    sem_init(&hayPCBsParaAgregarAlSistema, 0, 0);                       /* contador */
    sem_init(&hayPCBsParaPasarASusBlocked, 0, 0);                       /* contador */
    sem_init(&transicionarSusReadyAready, 0, 0);                        /* binario  */
    sem_init(&suspensionConcluida, 0, 0);                               /* binario  */

    /* Inicialización de colas de planificación */
    pcbsNew = cola_planificacion_create(0);
    pcbsReady = cola_planificacion_create(0);
    pcbsExec = cola_planificacion_create(0);
    pcbsBlocked = cola_planificacion_create(0);
    pcbsSusReady = cola_planificacion_create(0);
    pcbsSusBlocked = cola_planificacion_create(0); //esta la volamos
    pcbsExit = cola_planificacion_create(0);
    pthread_mutex_init(&mutexId, NULL);

    pthread_t thread;
    pthread_mutex_init(&mutexId, NULL);

    /* Planificador largo plazo */
    pthread_create(&thread, NULL, iniciar_largo_plazo, NULL);
    pthread_detach(thread);

    /* Planificador mediano plazo
    pthread_create(&thread, NULL, iniciar_mediano_plazo, NULL);
    pthread_detach(thread); */

    /* Planificador corto plazo */
    pthread_create(&thread, NULL, iniciar_corto_plazo, NULL);
    pthread_detach(thread);

}

/*---------------------------------------------- PLANIFICADOR CORTO PLAZO ----------------------------------------------*/

void* iniciar_corto_plazo(void* _) {
    for(;;) {
        sem_wait(&(pcbsReady->instanciasDisponibles)); //Llega un nuevo pcb a ready


        log_info(kernelLogger, "Corto Plazo: Se toma una instancia de READY");

        /*if(algoritmo_srt_loaded()) {
            interrupcion_a_cpu();//Hay que pedirle el PCB que esta EXEC, y ponerlo en la cola de pcbsReady... Si es que hay alguno
            getPcbDeCPU();

            sem_wait(&(pcbsExec->instanciasDisponibles));
            
        }*/
        //sem_wait(&(pcbsExec->instanciasDisponibles)); para mi va aca xq en fifo tmb tiene que esperar que no haya nada en exec TODO
        t_pcb* pcbQuePasaAExec = elegir_pcb_segun_algoritmo(pcbsReady);

        //remover_pcb_de_cola(pcbQuePasaAExec, pcbsReady); //Ya lo estamos removiendo de la lista al elegir segun FIFO.
        cambiar_estado_pcb(pcbQuePasaAExec, EXEC);
        agregar_pcb_a_cola(pcbQuePasaAExec, pcbsExec);
        mandar_pcb_a_cpu(pcbQuePasaAExec);
        sem_post(&(pcbsExec->instanciasDisponibles));

        log_transition("Corto Plazo", "READY", "EXEC", pcbQuePasaAExec->id);

        

        //atender_peticiones_pcb(pcbQuePasaAExec);
    }
    pthread_exit(NULL);
}

void* getPcbDeCPU(void) {

    /*pthread_t th1;
    pthread_create(&th1, NULL, conexion_de_interrupt, NULL);
    pthread_detach(th1);*/

    pthread_t th2;
    pthread_create(&th2, NULL, conexion_de_dispatch, NULL); //donde devuele el socket? yo lo declare global. TODO
    pthread_detach(th2);

    sem_wait(&(pcbsExec->instanciasDisponibles));
    t_pcb* pcbQueMeDaCPU = traer_cpu_de_memoria();

    determinar_cola_pcb(pcbQueMeDaCPU);

    // remover_pcb_de_cola(pcbQueMeDaCPU, pcbsExec);
    // cambiar_estado_pcb(pcbQueMeDaCPU, READY);
    // agregar_pcb_a_cola(pcbQueMeDaCPU, pcbsReady);

    //log_transition("Corto Plazo", "EXEC", "READY", pcbQueMeDaCPU->id);

}

t_pcb* traer_cpu_de_memoria(){ //una vez que manda una pcb a cpu, se queda esperando que se la devuelva por el motivo que sea
    //lo del socket ver como viene
    //x ahora lo declaro asi nomas

    void* pcb = malloc(sizeof(t_pcb));
    if(recv(socketCpuDispatch, pcb, sizeof(t_pcb), MSG_WAITALL) == -1){
        log_error(kernelLogger, "Error al recibir el PCB de la CPU");
    }
    //t_pcb* pcbArribada = deserializar_pcb(pcb); //esta es la que devuelve la funcion
    
    t_list* asd;
    t_pcb* hola = pcb_create(1,1,asd,kernelCfg); //si fue a IO suspenderlo
    return hola;
}

void* determinar_cola_pcb(t_pcb* pcb){
    if(instruccion_actual_es(pcb,"EXIT")){
            //remover de cola no hace falta creo, xq no esta en ninguna, o esta en la de exec?
            cambiar_estado_pcb(pcb, EXIT);
            agregar_pcb_a_cola(pcb, pcbsExit);
            log_transition("Corto Plazo", "EXEC", "EXIT", pcb->id);
            sem_post(&(pcbsExit->instanciasDisponibles));
        }
    if(instruccion_actual_es(pcb,"I/O")){
            cambiar_estado_pcb(pcb, BLOCKED);
            agregar_pcb_a_cola(pcb, pcbsBlocked);
            log_transition("Corto Plazo", "EXEC", "BLOCKED", pcb->id);
            sem_post(&(pcbsBlocked->instanciasDisponibles));
    }else{
        cambiar_estado_pcb(pcb,READY);
        agregar_pcb_a_cola(pcb, pcbsReady);
        log_transition("Corto Plazo", "EXEC", "READY", pcb->id);
        sem_post(&(pcbsReady->instanciasDisponibles));
    }
}

bool instruccion_actual_es(t_pcb* pcb, char* codOp){
    t_instruccion* inst = list_get(pcb->instrucciones,pcb->programCounter);
    return codOp == inst->indicador;
}

// void determinar_ready_o_blocked(t_pcb* pcb){
//     t_instruccion* instruccionActual = list_get(pcb->instrucciones, pcb->programCounter);
//     if(strcmp(instruccionActual->indicador,"IO")==0){ 
            
//     } 
//     else{ //que volvio por una interrupcion y no por IO
//         pcb->status = READY;
//         agregar_pcb_a_cola(pcb, pcbsReady);
//         log_transition("Corto Plazo", "EXEC", "READY", pcb->id);
//         sem_post(&(pcbsReady->instanciasDisponibles));
//     }

// }


void mandar_pcb_a_cpu(t_pcb* pcb) {
    uint32_t bytes = 0;
    log_info(kernelLogger, "Corto Plazo: Se manda el PCB %d a la CPU", pcb->id);

    char* pcbAMandar = serializar_pcb(pcb, &bytes);
    conexion_de_dispatch(); //setea la variable SOCKET_DISPATCH de arriba de todo 

    t_mensaje_tamanio *tamanio_mensaje = malloc(sizeof(t_mensaje_tamanio));
    tamanio_mensaje->tamanio = bytes;
    
    enviar_tamanio_mensaje(tamanio_mensaje, SOCKET_DISPATCH); //Enviamos primero el tamaño de mensaje, para saber que recibir.

    send(SOCKET_DISPATCH, pcbAMandar, bytes, 0); //Enviamos el mensaje con el PCB entero y el tamaño.

    log_info(kernelLogger, "Corto Plazo: Se mando el PCB %d a la CPU correctamente", pcb->id);
    free(tamanio_mensaje);
}

void interrupcion_a_cpu() {
    log_info(kernelLogger, "Corto Plazo: Se interrumpe la CPU");
    
    if(send(SOCKET_INTERRUPT, "interrupcion", sizeof("interrupcion"), 0) == -1) {
        log_error(kernelLogger, "Error al interrumpir la CPU");
    }
    log_info(kernelLogger, "Corto Plazo: Se interrumpió la CPU correctamente");
}

//La conexión de interrupt dedicada solamente a enviar mensajes de interrupción
void* conexion_de_interrupt() {
    log_info(kernelLogger, "Hilo interrupt inicializado");

    int SOCKET_INTERRUPT = conectar_a_servidor(kernelCfg->IP_CPU, kernelCfg->PUERTO_CPU_INTERRUPT);
    log_info(kernelLogger, "Kernel: Conectando a CPU");

    if (SOCKET_INTERRUPT == -1)
    {
        log_error(kernelCfg, "Consola: No se pudo establecer conexión con CPU. Valor conexión %d", kernelCfg);
        return -1;
    }
    

    //TODO: Le aviso a CPU que me tiene que dar el pcb que esta en Exec, si es que hay alguno
    //TODO: Si CPU maneja la cola de pcbsExec, solo tengo que sacarlo de ahi
}

//En todos los casos el PCB será recibido a través de la conexión de dispatch - Es bidireccional, por aca tambien le mando el PCB a CPU
void* conexion_de_dispatch() {
    log_info(kernelLogger, "Hilo dispatch EXEC->READY inicializado");

    SOCKET_DISPATCH = conectar_a_servidor(kernelCfg->IP_CPU, kernelCfg->PUERTO_CPU_DISPATCH);
    log_info(kernelLogger, "Kernel: Conectando a CPU");

    if (SOCKET_DISPATCH == -1)
    {
        log_error(kernelCfg, "Consola: No se pudo establecer conexión con CPU. Valor conexión %d", kernelCfg);
        return -1;
    }
    // CASO 1: Envio de PCB a CPU
    // CASO 2: Recibo PCB de CPU porque lo desalojo porque recibio un mensaje por conexion_de_interrupt
}


/*---------------------------------------------- PLANIFICADOR MEDIANO PLAZO ----------------------------------------------*/

void* iniciar_mediano_plazo(void* _) {
    pthread_t th;
    pthread_create(&th, NULL, pasar_de_susready_a_ready, NULL); 
    pthread_detach(th);
    //pthread_create(&th, NULL, recibir_pcb_bloqueado, NULL);
    //pthread_detach(th);
    for(;;) {
      
        sem_wait(&hayPCBsParaPasarASusBlocked); // TODO: Va a estar esperando por que le hagan el post
        t_pcb* pcbASuspender; //= pop_ultimo_de_cola(pcbsBlocked); //tiene que sacar segun el tiempo de configuracion, usar usleep
        //enviar_suspension_de_pcb_a_memoria(pcbASuspender);
        contar_tiempo_bloqueado(pcbASuspender); //HILO
        cambiar_estado_pcb(pcbASuspender, SUSBLOCKED);
        agregar_pcb_a_cola(pcbASuspender, pcbsSusBlocked);
        log_info(kernelLogger, "Mediano Plazo: Se libera una instancia de Grado Multiprogramación");
        log_transition("Mediano Plazo", "BLOCKED", "SUSP/BLOCKED", pcbASuspender->id);
        /* Aumenta el grado de multiprogramción al suspender a un proceso */
        sem_post(&gradoMultiprog);                              
        sem_wait(&suspensionConcluida); // Cuando termine la suspension sale del for
        //TODO: Aca pasar ya de SUSBLOCKED => SUSREADY ??? Un hilo?
        //dice esto: La transición (SUSPENDED-BLOCKED -> SUSPENDED-READY), al ser una transición que va a darse al finalizar una entrada/salida, 
        //no necesariamente forma parte del planificador de Mediano Plazo.
    }
    pthread_exit(NULL);
}

// void* recibir_pcb_bloqueado(void* pcb) { 
//     pthread_t th;

//     if(pcb->programCounter->parametros[0] > kernelCfg->TIEMPO_MAXIMO_BLOQUEADO) {
//         pthread_create(&th, NULL, blocked_a_susblocked, NULL);
//         pthread_detach(th);
//     } else {
//         pthread_create(&th, NULL, blocked_a_ready, NULL);
//         pthread_detach(th);
//     }
// }

// void* blocked_a_susblocked(void* _) {
//     // Usar remover_pcb_de_cola?
//     sem_post(&hayPCBsParaPasarASusBlocked);
// }

void* contar_tiempo_bloqueado(t_pcb* pcb){ //usar como HILO, porque sino la va a suspender siempre
    usleep(kernelCfg->TIEMPO_MAXIMO_BLOQUEADO);
    if(pcb->status==BLOCKED){
        //enviar_suspension_de_pcb_a_memoria(pcb);
        cambiar_estado_pcb(pcb, SUSBLOCKED);
        agregar_pcb_a_cola(pcb, pcbsSusBlocked);
        log_info(kernelLogger, "Mediano Plazo: Se libera una instancia de Grado Multiprogramación");
        log_transition("Mediano Plazo", "BLOCKED", "SUSP/BLOCKED", pcb->id);
        /* Aumenta el grado de multiprogramción al suspender a un proceso */  
        sem_post(&gradoMultiprog); 
    }
    pthread_exit(NULL);
}

void* pasar_de_susready_a_ready(void* _) {
    log_info(kernelLogger, "Mediano Plazo: Hilo pasar de SUSP/READY->READY inicializado");
    for(;;) {
        sem_wait(&transicionarSusReadyAready);
        t_pcb* pcbQuePasaAReady = get_and_remove_primer_pcb_de_cola(pcbsSusReady);
        cambiar_estado_pcb(pcbQuePasaAReady, READY);
        agregar_pcb_a_cola(pcbQuePasaAReady, pcbsReady);

        log_transition("Mediano Plazo", "SUSP/READY", "READY", pcbQuePasaAReady->id);

        sem_post(&(pcbsReady->instanciasDisponibles));
    }
    pthread_exit(NULL);
}

void* blocked_a_ready(t_pcb* pcb) {
    remover_pcb_de_cola(pcb, pcbsBlocked);
    cambiar_estado_pcb(pcb, READY);
    agregar_pcb_a_cola(pcb, pcbsReady);
    sem_post(&(pcbsReady->instanciasDisponibles));
}

/*void* enviar_suspension_de_pcb_a_memoria(t_pcb* pcb) {
    //se enviará un mensaje a Memoria con la información necesaria y se esperará la confirmación del mismo.
    //Una vez recibo la confirmacion
    int memoria_fd=conectar_a_servidor(kernelCfg->IP_MEMORIA, kernelCfg->PUERTO_MEMORIA);
    if (memoria_fd == -1)
    {
        log_error(kernelCfg, "Kernel: No se pudo establecer conexión con Memoria. Valor conexión %d", memoria_fd);//warning de kernelCfg
        return -1;
    }
    void* mensaje = serializar_pcb(pcb); //TODO
    if(send(memoria_fd, mensaje, sizeof(t_pcb), 0) == -1) {
        log_error(kernelCfg, "Kernel: No se pudo enviar el PCB a Memoria. Valor conexión %d", memoria_fd);
        return -1;
    }
    log_info(kernelLogger,"Kernel: Enviada PCB a Memoria");
    sem_post(&suspensionConcluida); //creo que este va un poco despues
}*/

void suspender_tiempo_de_io(t_pcb* pcb, uint32_t tiempo){ //el tiempo es el que llega por parametro de la instruccion
    usleep(tiempo);
    if(pcb->status==BLOCKED){
        blocked_a_ready(pcb);
    }
    if(pcb->status==SUSBLOCKED){
        //susp_a_ready(pcb);
        remover_pcb_de_cola(pcb, pcbsBlocked); //esto hace el wait del semaforo de instancias? 
        cambiar_estado_pcb(pcb, SUSREADY);
        agregar_pcb_a_cola(pcb,pcbsSusReady);
        
        sem_post(&(pcbsSusReady->instanciasDisponibles));
    }
}

/*---------------------------------------------- PLANIFICADOR LARGO PLAZO ----------------------------------------------*/

void* iniciar_largo_plazo(void* _) {
    pthread_t th;
    pthread_create(&th, NULL, liberar_procesos_en_exit, NULL);
    pthread_detach(th);
    log_info(kernelLogger, "Largo Plazo: Inicialización exitosa");
    for(;;) {
        /* Tanto NEW como SUSREADY son parte del mismo conjunto: "El conjunto a pasar a READY" */
        sem_wait(&hayPCBsParaAgregarAlSistema);
        log_info(kernelLogger, "Largo Plazo: Se toma una instancia de PCBs a agregar al sistema");
        
        /* Este semáforo debería ser "posteado" (sem_post) cuando un proceso se suspende (baja el grado de multiprogramación),
        o bien cuando un proceso del sistema pasa a exit */
        sem_wait(&gradoMultiprog);
        log_info(kernelLogger, "Largo Plazo: Se toma una instancia de Grado Multiprogramación");

        if(!list_is_empty(pcbsSusReady->lista)) {
            /* Como tiene mayor prioridad los procesos en susp/ready > new, vemos si hay procesos en dicha cola */
            t_pcb* pcbQuePasaAReady = get_and_remove_primer_pcb_de_cola(pcbsSusReady);
            cambiar_estado_pcb(pcbQuePasaAReady, READY);
            agregar_pcb_a_cola(pcbQuePasaAReady, pcbsReady);
            log_transition("Largo Plazo", "SUSP/READY", "READY", pcbQuePasaAReady->id);
            sem_post(&transicionarSusReadyAready);
            /* Con el post se libera el mecanismo de pasaje en el Planificador de Mediano Plazo */
        } else {                                    
            /* En caso de que no haya procesos en susp/ready, seguramente hay en new */
            t_pcb* pcbQuePasaAReady = get_and_remove_primer_pcb_de_cola(pcbsNew);
            pcbQuePasaAReady->est_rafaga_actual = kernelCfg->ESTIMACION_INICIAL; //Milisegundos
            /* Solamente coloco la ESTIMACION_INICIAL cuando provenga de NEW (pues si viene de SUSREADY quiere decir que ya estuvo inicializado antes) */
            log_info(kernelLogger, "Largo Plazo: Incialización de información del algoritmo correcta");

            cambiar_estado_pcb(pcbQuePasaAReady, READY);
            agregar_pcb_a_cola(pcbQuePasaAReady, pcbsReady);
            log_transition("Largo Plazo", "NEW", "READY", pcbQuePasaAReady->id);

            sem_post(&(pcbsReady->instanciasDisponibles));
        }
    }
    pthread_exit(NULL);
}

void* liberar_procesos_en_exit(void* _) {
    log_info(kernelLogger, "Largo Plazo: Hilo liberador de PCBs de Procesos en EXIT inicializado");
    for(;;) {
        sem_wait(&(pcbsExit->instanciasDisponibles));

        t_pcb* pcbALiberar = get_and_remove_primer_pcb_de_cola(pcbsExit);

        //TODO: Avisarle a Consola que finalizo su proceso. obtener el socket de esa consola en particular
        log_info(kernelLogger, "Kernel: Desconexión Proceso con ID %d", pcbALiberar->id);
        pcb_destroy(pcbALiberar);

        log_info(kernelLogger, "Largo Plazo: Se libera una instancia de Grado Multiprogramación");
        /* Aumenta el grado de multiprogramación al tener proceso en EXIT */
        sem_post(&gradoMultiprog);                          
    }
    pthread_exit(NULL);
}

/*---------------------------------------------- MANEJO DE PCBs ----------------------------------------------*/

t_pcb* pcb_create(uint32_t id, uint32_t tamanio, t_list* instrucciones, t_kernel_config* config) {
    time_t tf,ti;
    t_pcb* self = malloc(sizeof(t_pcb));
    self->id = id;
    self->status = NEW;
    self->tamanio = tamanio;
    self->instrucciones = instrucciones;
    self->programCounter = 0;
    self->est_rafaga_actual = kernelCfg->ESTIMACION_INICIAL;
    if(algoritmo_srt_loaded()) {
        self->algoritmo_siguiente_estim = srt_actualizar_info_para_siguiente_estimacion;
    }
    return self;
}

void pcb_destroy(t_pcb *pcb) {
    // Realiza los free correspondientes
    //las variables no se si hacen falta free, las instrucciones seguro

    list_destroy_and_destroy_elements(pcb->instrucciones, (void*)destruir_instruccion);
    free(pcb);
}

void destruir_instruccion(t_instruccion* instruccion) {
    //free(instruccion->parametros...);
    //TODO
}

void cambiar_estado_pcb(t_pcb* pcb, t_status nuevoEstado) {
    pcb->status = nuevoEstado;
}

int get_grado_multiprog_actual(void) {
    int gradoMultiprogActual;
    sem_getvalue(&gradoMultiprog, &gradoMultiprogActual);
    return gradoMultiprogActual;
}

void log_transition(const char* entityName, const char* prev, const char* post, int id) {
    char* transicion = string_from_format("\e[1;93m%s->%s\e[0m", prev, post);
    log_info(kernelLogger, "%s: Transición de %s Proceso de ID %d", entityName, transicion, id);
    free(transicion);
}


void agregar_pcb_en_cola_new()
{
    char* mensaje;
    t_list* instrucciones;
    t_pcb* pcb;
    uint32_t tamanio;
    sem_t recibirInstruccion;
    sem_init(&recibirInstruccion, 0, 0);
    while (1) {
        op_code cod_op = recibir_operacion(kernelCfg->CONSOLA_SOCKET);
        switch (cod_op) {
            case MENSAJE:
                mensaje = recibir_mensaje(kernelCfg->CONSOLA_SOCKET);
                tamanio = atoi(mensaje);
                log_info(kernelLogger, "Me llego el mensaje %i", tamanio);
                continue;
            case INSTRUCCION:
                instrucciones = recibir_instrucciones(kernelCfg->CONSOLA_SOCKET);
                sem_post(&recibirInstruccion);
                break;
            default:
                break;
        }

        sem_wait(&recibirInstruccion);
        uint32_t idPcb = get_siguiente_id();
        //CREAMOS EL PCB
        pcb = pcb_create(idPcb, tamanio, instrucciones, kernelCfg); //utilizar socketEscucha como ID de PCB.
        log_info(kernelLogger, "ID PCB %i", pcb->id);
        if(pcb->status == NEW){
            log_info(kernelLogger, "STATUS PCB %s", "NEW");
        }
        log_info(kernelLogger, "TAMAÑO PCB %i", pcb->tamanio);


        agregar_pcb_a_cola(pcb, pcbsNew);
        log_info(kernelLogger, "Kernel: Creación PCB con ID %d exitosa", pcb->id);

        sem_post(&hayPCBsParaAgregarAlSistema); 
    }
    free(mensaje);
}

uint32_t get_siguiente_id() 
{
    pthread_mutex_lock(&mutexId);
    uint32_t id = nextId;
    nextId++;
    pthread_mutex_unlock(&mutexId);
    return id;
}

/*---------------------------------------------- MANEJO DE COLAS ----------------------------------------------*/

t_cola_planificacion* cola_planificacion_create(int semInitVal) {
    t_cola_planificacion* self = malloc(sizeof(t_cola_planificacion));
    self->lista = list_create();

    pthread_mutex_init(&(self->mutex), NULL);
    sem_init(&(self->instanciasDisponibles), 0, semInitVal);

    return self;
}

t_pcb* get_and_remove_primer_pcb_de_cola(t_cola_planificacion* cola) {
    t_pcb* pcb = NULL;
    pthread_mutex_lock(&(cola->mutex));
    if(!list_is_empty(cola->lista)) {
        pcb = (t_pcb*) list_remove(cola->lista, 0);
    }
    pthread_mutex_unlock(&(cola->mutex));

    return pcb;
}

int pcb_get_posicion(t_pcb* pcb, t_list* lista) {
    for (int posicion = 0; posicion < list_size(lista); posicion++) {
        if (pcb == (t_pcb*) list_get(lista, posicion)) {
            return posicion;
        }
    }
    return -1;
}

void agregar_pcb_a_cola(t_pcb* pcb, t_cola_planificacion* cola) {
    pthread_mutex_lock(&(cola->mutex));
    list_add(cola->lista, pcb);
    pthread_mutex_unlock(&(cola->mutex));
}

void remover_pcb_de_cola(t_pcb* pcb, t_cola_planificacion* cola) {
    pthread_mutex_lock(&(cola->mutex));
    int posicion = pcb_get_posicion(pcb, cola->lista);
    if(posicion != -1) {
        list_remove(cola->lista, posicion);
    } else {
        log_error(kernelLogger, "Kernel: No existe tal elemento en la cola");
    }
    pthread_mutex_unlock(&(cola->mutex));
}

/*---------------------------------------------- ALGORITMOS ----------------------------------------------*/

t_pcb* elegir_pcb_segun_algoritmo(t_cola_planificacion* cola) {
    t_pcb* pcb = NULL;
    if(algoritmo_fifo_loaded()) {
        pcb = elegir_en_base_a_fifo(cola);
    } else if(algoritmo_srt_loaded()) {
        pcb = elegir_en_base_a_srt(cola);
    }
    return pcb;
}

bool algoritmo_fifo_loaded(void) {
    return strcmp(kernelCfg->ALGORITMO_PLANIFICACION, "FIFO") == 0;
}

bool algoritmo_srt_loaded(void) {
    return strcmp(kernelCfg->ALGORITMO_PLANIFICACION, "SRT") == 0;
}

/*---------------------------------------------- FIFO ----------------------------------------------*/

t_pcb* elegir_en_base_a_fifo(t_cola_planificacion* colaPlanificacion) {
    t_pcb* primerPcb = get_and_remove_primer_pcb_de_cola(colaPlanificacion);
    pthread_mutex_lock(&(colaPlanificacion->mutex));
    log_info(kernelLogger, "FIFO: Se toma una instancia de READY, PCB ID %d", primerPcb->id);
    pthread_mutex_unlock(&(colaPlanificacion->mutex));
    return primerPcb;
}

/*---------------------------------------------- SRT ----------------------------------------------*/

t_pcb* srt_pcb_menor_estimacion_entre(t_pcb* unPcb, t_pcb* otroPcb) {
    return unPcb->est_rafaga_actual <= otroPcb->est_rafaga_actual ? unPcb : otroPcb;
}

t_pcb* elegir_en_base_a_srt(t_cola_planificacion* colaPlanificacion) {
    pthread_mutex_lock(&(colaPlanificacion->mutex));
    t_pcb* pcbMenorEstimacion = (t_pcb*) list_get_minimum(colaPlanificacion->lista, (void*) srt_pcb_menor_estimacion_entre);
    pthread_mutex_unlock(&(colaPlanificacion->mutex));
    return pcbMenorEstimacion;
}

double get_diferencial_de_tiempo(time_t tiempoFinal, time_t tiempoInicial) {
    /* difftime - calculate time difference
        The difftime() function returns the number of seconds elapsed
        between time time1 and time time0, represented as a double.*/
    double diferencialT = difftime(tiempoFinal, tiempoInicial);
    return diferencialT;
}

double media_exponencial(double realAnterior, double estAnterior) {
    /* Est(n) = α . R(n-1) + (1 - α) . Est(n-1) */
    return kernelCfg->ALFA * realAnterior + (1 - kernelCfg->ALFA) * estAnterior;
}

void srt_actualizar_info_para_siguiente_estimacion(t_pcb* pcb, time_t tiempoFinal, time_t tiempoInicial) {
    double realAnterior = get_diferencial_de_tiempo(tiempoFinal, tiempoInicial);
    pcb->est_rafaga_actual = media_exponencial(realAnterior, pcb->est_rafaga_actual);
}

uint32_t calcular_tiempo(){
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    //aca hacer lo que haya que hacer
    int a=2; //ej
    for (int i=0;i<1000000;i++){}//ej
    
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    
    u_int32_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;
    
    log_info(kernelLogger, "Prueba de timer %i", delta_us);

    return delta_us;
}

