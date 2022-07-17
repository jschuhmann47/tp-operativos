#include "planificador.h"

// Semaforos
sem_t gradoMultiprog;
sem_t hayPCBsParaAgregarAlSistema;
sem_t hayPCBsParaPasarASusBlocked;
sem_t transicionarSusReadyAready;
sem_t suspensionConcluida;
pthread_mutex_t mutexId;
sem_t pcbsEnExit;
pthread_mutex_t suspensionDePCB;
pthread_mutex_t colaDeIO;
pthread_mutex_t mutex_lista_sockets;
pthread_mutex_t tiempoIO;
pthread_mutex_t recibirPCB;
sem_t listoParaEjecutar;

//ID para el PCB
static uint32_t nextId;

// Colas de planificación
t_cola_planificacion* pcbsNew;
t_cola_planificacion* pcbsReady;
t_cola_planificacion* pcbsExec;
t_cola_planificacion* pcbsBlocked;
t_cola_planificacion* pcbsSusReady;
t_cola_planificacion* pcbsExit;

int SOCKET_DISPATCH;

int SOCKET_INTERRUPT;

int SOCKET_MEMORIA;

t_list* sockets;

void iniciar_planificacion() {
    //Conexiones
    conexion_de_dispatch();
    conexion_de_interrupt();
    conexion_de_memoria();

    nextId = 1;
    /* Inicializacion de semaforos */
    sem_init(&gradoMultiprog, 0, kernelCfg->GRADO_MULTIPROGRAMACION);   /* contador */
    sem_init(&hayPCBsParaAgregarAlSistema, 0, 0);                       /* contador */
    sem_init(&hayPCBsParaPasarASusBlocked, 0, 0);                       /* contador */
    sem_init(&transicionarSusReadyAready, 0, 0);                        /* binario  */
    sem_init(&suspensionConcluida, 0, 0);                               /* binario  */
    sem_init(&pcbsEnExit, 0, 0);
    sem_init(&listoParaEjecutar, 0, 1);

    sockets = list_create();

    /* Inicialización de colas de planificación */
    pcbsNew = cola_planificacion_create(0);
    pcbsReady = cola_planificacion_create(0);
    pcbsExec = cola_planificacion_create(0);
    pcbsBlocked = cola_planificacion_create(0);
    pcbsSusReady = cola_planificacion_create(0);
    pcbsExit = cola_planificacion_create(0);

    pthread_t thread;
    pthread_mutex_init(&mutexId, NULL);
    pthread_mutex_init(&mutex_lista_sockets, NULL);
    pthread_mutex_init(&suspensionDePCB, NULL);
    pthread_mutex_init(&colaDeIO, NULL);
    pthread_mutex_init(&tiempoIO, NULL);
    pthread_mutex_init(&recibirPCB, NULL);
    
    /* Planificador largo plazo */
    pthread_create(&thread, NULL, iniciar_largo_plazo, NULL);
    pthread_detach(thread);

    /* Planificador mediano plazo */
    pthread_create(&thread, NULL, pasar_de_susready_a_ready, NULL); 
    pthread_detach(thread); 

    /* Planificador corto plazo */
    pthread_create(&thread, NULL, iniciar_corto_plazo, NULL);
    pthread_detach(thread);

    pthread_create(&thread, NULL, traer_pcb_de_cpu, NULL);
    pthread_detach(thread);
}

/*---------------------------------------------- PLANIFICADOR CORTO PLAZO ----------------------------------------------*/

void* iniciar_corto_plazo(void* _) {
    for(;;) {
        sem_wait(&(pcbsReady->instanciasDisponibles)); //Llega un nuevo pcb a ready
        log_info(kernelLogger, "Corto Plazo: Se toma una instancia de READY");
        
        sem_wait(&listoParaEjecutar);
        
        t_pcb* pcbQuePasaAExec = elegir_pcb_segun_algoritmo(pcbsReady);

        cambiar_estado_pcb(pcbQuePasaAExec, EXEC);
        agregar_pcb_a_cola(pcbQuePasaAExec, pcbsExec);
        
        log_debug(kernelLogger, "Corto Plazo: Mando con rafaga %f",pcbQuePasaAExec->est_rafaga_actual);

        mandar_pcb_a_cpu(pcbQuePasaAExec);
        log_transition("Corto Plazo", "READY", "EXEC", pcbQuePasaAExec->id);
        sem_post(&(pcbsExec->instanciasDisponibles));

    }
    pthread_exit(NULL);
}

void interrumpir_si_es_srt(){
    if(algoritmo_srt_loaded() && list_size(pcbsExec->lista) > 0) {
        
        log_info(kernelLogger, "Corto Plazo: Interrupción de SRT, se trae PCB del CPU");
        interrupcion_a_cpu();
    }
}


void* traer_pcb_de_cpu(){
    while(1){
        sem_wait(&(pcbsExec->instanciasDisponibles));
        t_pcb* pcb;
        void* buffer;
        log_info(kernelLogger, "Kernel: Esperando PCB de CPU");
        t_mensaje_tamanio *tamanio_mensaje = malloc(sizeof(t_mensaje_tamanio));

        pthread_mutex_lock(&recibirPCB);
        if (recibir_tamanio_mensaje(tamanio_mensaje, SOCKET_DISPATCH)){
            buffer = malloc(tamanio_mensaje->tamanio);
            log_debug(kernelLogger, "Kernel: Recibi el tamanio: %i", tamanio_mensaje->tamanio);
            if (recv(SOCKET_DISPATCH, buffer, tamanio_mensaje->tamanio, MSG_WAITALL)) {
                pcb = recibir_pcb(buffer,tamanio_mensaje->tamanio);
                log_info(kernelLogger, "Kernel: Recibi el PCB con ID: %i", pcb->id);
            }
        }
        
        uint32_t tiempoABloquearsePorIO; 
        t_instruccion* ultimaInstruccion = list_get(pcb->instrucciones, (pcb->programCounter)-1);
        if(ultimaInstruccion->indicador == I_O){
            if(recv(SOCKET_DISPATCH,&tiempoABloquearsePorIO,sizeof(uint32_t),MSG_WAITALL) < 0){
                log_error(kernelLogger, "Kernel: Error al recibir el mensaje de tiempo a bloquearse por IO");
            }
        }else{
            tiempoABloquearsePorIO = 0;
        }
        
        t_tiempo_io* tiempoIo = malloc(sizeof(t_tiempo_io));
        tiempoIo->pcb = pcb;
        tiempoIo->tiempo = tiempoABloquearsePorIO;

        pthread_mutex_unlock(&recibirPCB);    
        remover_pcb_de_cola(pcb, pcbsExec);
        
        calcular_nueva_estimacion_actual(pcb);

        pthread_t determinarCola;
        pthread_create(&determinarCola, NULL, determinar_cola, tiempoIo);
        pthread_detach(determinarCola);   
    }
    pthread_exit(NULL);
}

void determinar_cola(t_tiempo_io* pcbConTiempo)
{
    t_pcb* pcb = pcbConTiempo->pcb;
    uint32_t tiempoABloquearsePorIO = pcbConTiempo->tiempo;

    t_instruccion* ultimaInstruccion = list_get(pcb->instrucciones, (pcb->programCounter)-1);

    if(ultimaInstruccion->indicador != I_O && ultimaInstruccion->indicador != EXIT_I){ //fue por interrupcion
        cambiar_estado_pcb(pcb, READY);
        agregar_pcb_a_cola(pcb, pcbsReady);
        log_transition("Corto Plazo", "EXEC", "READY", pcb->id);
        sem_post(&(pcbsReady->instanciasDisponibles));
    }
    sem_post(&listoParaEjecutar);

    if(ultimaInstruccion->indicador == I_O){
        log_info(kernelLogger, "Kernel: Recibi el tiempo a bloquearse por IO: %i segundos", tiempoABloquearsePorIO/1000);
        cambiar_estado_pcb(pcb, BLOCKED);
        agregar_pcb_a_cola(pcb, pcbsBlocked);
        log_transition("Corto Plazo", "EXEC", "BLOCKED", pcb->id);
        sem_post(&(pcbsBlocked->instanciasDisponibles));
        atender_procesos_bloqueados(tiempoABloquearsePorIO);
    }
    if(ultimaInstruccion->indicador == EXIT_I){
        cambiar_estado_pcb(pcb, EXIT);
        agregar_pcb_a_cola(pcb, pcbsExit);
        sem_post(&(pcbsExit->instanciasDisponibles));
    }
    free(pcbConTiempo);
}

void atender_procesos_bloqueados(uint32_t tiempoBloqueadoPorIo){
    log_info(kernelLogger, "Corto Plazo: Se atienden procesos bloqueados inicializado");
    
    sem_wait(&(pcbsBlocked->instanciasDisponibles));
    t_pcb* pcbABloquear = get_and_remove_primer_pcb_de_cola(pcbsBlocked);

    pthread_t contarTiempo;
    pthread_create(&contarTiempo,NULL,contar_tiempo_bloqueado,pcbABloquear);
    pthread_detach(contarTiempo);

    pthread_mutex_lock(&colaDeIO);
    log_info(kernelLogger, "Corto Plazo: Se bloquea el proceso %d por %i segundos", pcbABloquear->id, tiempoBloqueadoPorIo/1000);
    
    sleep(tiempoBloqueadoPorIo/1000);
    pthread_mutex_lock(&suspensionDePCB);
    if(pcbABloquear->status==BLOCKED){ //chequea que no lo hayan suspendido
        
        cambiar_estado_pcb(pcbABloquear, READY);
        agregar_pcb_a_cola(pcbABloquear, pcbsReady);
        log_transition("Corto Plazo", "BLOCKED", "READY", pcbABloquear->id);
        interrumpir_si_es_srt();
        sem_post(&(pcbsReady->instanciasDisponibles));
    }
    if(pcbABloquear->status == SUSBLOCKED){
        
        cambiar_estado_pcb(pcbABloquear, SUSREADY);
        agregar_pcb_a_cola(pcbABloquear, pcbsSusReady);
        log_transition("Kernel:", "SUSBLOCKED", "SUSREADY", pcbABloquear->id);
        sem_post(&(pcbsSusReady->instanciasDisponibles));
    }
    pthread_mutex_unlock(&suspensionDePCB);
    pthread_mutex_unlock(&colaDeIO);
}


bool instruccion_actual_es(t_pcb* pcb, code_instruccion codOp){
    t_instruccion* inst = list_get(pcb->instrucciones,pcb->programCounter);
    return codOp == inst->indicador;
}

void mandar_pcb_a_cpu(t_pcb* pcb) {
    uint32_t bytes = 0;
    log_info(kernelLogger, "Corto Plazo: Se manda el PCB %i a la CPU", pcb->id);

    void* pcbAMandar = serializar_pcb(pcb, &bytes);

    t_mensaje_tamanio *tamanio_mensaje = malloc(sizeof(t_mensaje_tamanio));
    tamanio_mensaje->tamanio = bytes;
    
    enviar_tamanio_mensaje(tamanio_mensaje, SOCKET_DISPATCH); //Enviamos primero el tamaño de mensaje, para saber que recibir.

    if(send(SOCKET_DISPATCH, pcbAMandar, bytes, 0)>0){
        log_info(kernelLogger, "Corto Plazo: Se mando el PCB %i a la CPU correctamente", pcb->id);
    } //Enviamos el mensaje con el PCB entero y el tamaño.

    free(pcbAMandar);
    free(tamanio_mensaje);
}

void interrupcion_a_cpu() {
    
    log_info(kernelLogger, "Corto Plazo: Se interrumpe la CPU");
    uint32_t mensaje = 1;
    
    if(send(SOCKET_INTERRUPT, &mensaje, sizeof(uint32_t), 0) == -1) {
        log_error(kernelLogger, "Error al interrumpir la CPU");
        exit(-1);
    }
}



/*---------------------------------------------- PLANIFICADOR MEDIANO PLAZO ----------------------------------------------*/

void* contar_tiempo_bloqueado(t_pcb* pcb){ //usar como HILO, porque sino la va a suspender siempre
    sleep(kernelCfg->TIEMPO_MAXIMO_BLOQUEADO/1000);
    pthread_mutex_lock(&suspensionDePCB);
    if(pcb->status==BLOCKED){
        enviar_suspension_de_pcb_a_memoria(pcb);
        cambiar_estado_pcb(pcb, SUSBLOCKED);
        log_info(kernelLogger, "Mediano Plazo: Se libera una instancia de Grado de Multiprogramación");
        log_transition("Mediano Plazo", "BLOCKED", "SUSP/BLOCKED", pcb->id);
        /* Aumenta el grado de multiprogramción al suspender a un proceso */  
        sem_post(&gradoMultiprog); 
    }
    pthread_mutex_unlock(&suspensionDePCB);
    pthread_exit(NULL);
}

void* pasar_de_susready_a_ready(void* _) {
    log_info(kernelLogger, "Mediano Plazo: Hilo pasar de SUSP/READY->READY inicializado");
    for(;;) {
        sem_wait(&(pcbsSusReady->instanciasDisponibles));
        sem_wait(&gradoMultiprog);
        t_pcb* pcbQuePasaAReady = get_and_remove_primer_pcb_de_cola(pcbsSusReady);
        liberar_pcb_de_memoria(pcbQuePasaAReady);
        cambiar_estado_pcb(pcbQuePasaAReady, READY);
        agregar_pcb_a_cola(pcbQuePasaAReady, pcbsReady);

        log_transition("Mediano Plazo", "SUSP/READY", "READY", pcbQuePasaAReady->id);
        interrumpir_si_es_srt();
        sem_post(&(pcbsReady->instanciasDisponibles));
    }
    pthread_exit(NULL);
}


void enviar_suspension_de_pcb_a_memoria(t_pcb* pcb) {
    op_code codOp = SUSPENSION;
    t_mensaje_tamanio* mensaje = malloc(sizeof(t_mensaje_tamanio));
    uint32_t bytes;
    void* buffer = serializar_pcb(pcb,&bytes);
    
    mensaje->tamanio=bytes;
    //poner mutex con la de liberar pcb de memoria
    if(send(SOCKET_MEMORIA, &codOp, sizeof(op_code), 0) == -1) {
        log_error(kernelLogger, "Error al enviar el codigo de operacion a la Memoria");
    }

    enviar_tamanio_mensaje(mensaje, SOCKET_MEMORIA);
    
    if(send(SOCKET_MEMORIA, buffer, bytes, 0) == -1) {
        log_error(kernelLogger, "Kernel: No se pudo enviar el PCB a Memoria. Valor conexión %d", SOCKET_MEMORIA);
        //return -1;
    }
    log_info(kernelLogger,"Kernel: Enviada PCB para suspension a Memoria");
    free(buffer);
    free(mensaje);

    sem_post(&suspensionConcluida); //creo que este va un poco despues
}


/*---------------------------------------------- PLANIFICADOR LARGO PLAZO ----------------------------------------------*/

void* iniciar_largo_plazo(void* _) {
    pthread_t th;
    pthread_create(&th, NULL, liberar_procesos_en_exit, NULL);
    pthread_detach(th);
    log_info(kernelLogger, "Largo Plazo: Inicialización exitosa");
    for(;;) {
        sem_wait(&hayPCBsParaAgregarAlSistema); //new o susready
        log_info(kernelLogger, "Largo Plazo: Se toma una instancia de PCBs a agregar al sistema");
        
        sem_wait(&gradoMultiprog);
        log_info(kernelLogger, "Largo Plazo: Se toma una instancia de Grado Multiprogramación");

        if(!list_is_empty(pcbsSusReady->lista)) {
            t_pcb* pcbQuePasaAReady = get_and_remove_primer_pcb_de_cola(pcbsSusReady);
            liberar_pcb_de_memoria(pcbQuePasaAReady);
            cambiar_estado_pcb(pcbQuePasaAReady, READY);
            agregar_pcb_a_cola(pcbQuePasaAReady, pcbsReady);
            log_transition("Largo Plazo", "SUSP/READY", "READY", pcbQuePasaAReady->id);
            interrumpir_si_es_srt();
            sem_post(&(pcbsReady->instanciasDisponibles));
        } else { //hay en new pero no hay en susready
            t_pcb* pcbQuePasaAReady = get_and_remove_primer_pcb_de_cola(pcbsNew);
            pcbQuePasaAReady->est_rafaga_actual = kernelCfg->ESTIMACION_INICIAL;
            cambiar_estado_pcb(pcbQuePasaAReady, READY);
            agregar_pcb_a_cola(pcbQuePasaAReady, pcbsReady);
            solicitar_nueva_tabla_memoria(pcbQuePasaAReady);
            log_transition("Largo Plazo", "NEW", "READY", pcbQuePasaAReady->id);
            interrumpir_si_es_srt();
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
        
        
        log_info(kernelLogger, "Kernel: Desconexión Proceso con ID %d", pcbALiberar->id);
        log_transition("Corto Plazo", "EXEC", "EXIT", pcbALiberar->id);
        
        
        log_info(kernelLogger, "Largo Plazo: Se libera una instancia de Grado Multiprogramación");
        /* Aumenta el grado de multiprogramación al tener proceso en EXIT */
        
        finalizar_proceso_en_memoria(pcbALiberar);
        enviar_finalizacion_consola("Finish", get_socket_de_pid(pcbALiberar->id));
        cerrar_socket_de_pid(pcbALiberar->id);

        pcb_destroy(pcbALiberar);
        
        sem_post(&gradoMultiprog);
    }
    pthread_exit(NULL);
}

void solicitar_nueva_tabla_memoria(t_pcb* pcb)
{
    log_info(kernelLogger, "Largo Plazo: Solicitando nueva tabla de memoria para el proceso %d a Memoria",pcb->id);
    op_code opCode = NEWTABLE;
    uint32_t indice;
    if(send(SOCKET_MEMORIA, &opCode, sizeof(op_code), 0)<0){
        log_error(kernelLogger, "Kernel: No se pudo enviar el codigo de operacion a la Memoria");
        exit(-1);
    }
    if(send(SOCKET_MEMORIA, &(pcb->id), sizeof(uint32_t), 0)<0){
        log_error(kernelLogger, "Kernel: No se pudo enviar el ID del PCB a la Memoria");
        exit(-1);
    }
    if(send(SOCKET_MEMORIA, &(pcb->tamanio), sizeof(uint32_t), 0)<0){
        log_error(kernelLogger, "Kernel: No se pudo enviar el tamanio del PCB a la Memoria");
        exit(-1);
    }
    if(recv(SOCKET_MEMORIA, &indice, sizeof(uint32_t), MSG_WAITALL)){
        log_info(kernelLogger, "Largo Plazo: Recibi el indice de tabla %i correctamente para el PCB: %i ", indice ,pcb->id);
        pcb->tablaDePaginas = indice;
    }else{
        log_error(kernelLogger, "Kernel: No se pudo recibir el indice de tabla de la Memoria");
        exit(-1);
    }
}

void liberar_pcb_de_memoria(t_pcb* pcb){
    op_code opCode = FREEPCB;

    if(send(SOCKET_MEMORIA, &opCode, sizeof(op_code), 0)<0){
        log_error(kernelLogger, "Largo Plazo: Error al enviar el codigo de operacion");
    }

    uint32_t pid = pcb->id;
    if(send(SOCKET_MEMORIA, &pid, sizeof(uint32_t), 0)<0){
        log_error(kernelLogger, "Largo Plazo: Error al enviar el PID");
    }
    //log_info(kernelLogger, "Largo Plazo: Enviado el proceso de ID %i para su liberacion de Memoria ",pcb->id);

}

void finalizar_proceso_en_memoria(t_pcb* pcb)
{
    op_code opCode = FREEPROCESO;

    if(send(SOCKET_MEMORIA, &opCode, sizeof(op_code), 0)<0){
        log_error(kernelLogger, "Largo Plazo: Error al finalizar proceso");
    }

    if(send(SOCKET_MEMORIA, &(pcb->id), sizeof(uint32_t), 0)){
        log_info(kernelLogger, "Largo Plazo: Envio de PID para eliminar archivo");
    }

    uint32_t indiceAEliminar = pcb->tablaDePaginas;
    if(send(SOCKET_MEMORIA, &indiceAEliminar, sizeof(uint32_t), 0)<0){
        log_error(kernelLogger, "Largo Plazo: Error al enviar el indice de tabla");
    }
    log_info(kernelLogger, "Largo Plazo: Enviado el indice de tabla %i para su eliminacion de Memoria",indiceAEliminar);
}
    

/*---------------------------------------------- MANEJO DE PCBs ----------------------------------------------*/

t_pcb* pcb_create(uint32_t id, uint32_t tamanio, t_list* instrucciones, t_kernel_config* config) {
    t_pcb* self = malloc(sizeof(t_pcb));
    self->id = id;
    self->status = NEW;
    self->tamanio = tamanio;
    self->instrucciones = instrucciones;
    self->programCounter = 0;
    self->tablaDePaginas = 0;
    self->est_rafaga_actual = kernelCfg->ESTIMACION_INICIAL;
    self->dur_ultima_rafaga = kernelCfg->ESTIMACION_INICIAL;
    return self;
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


void agregar_pcb_en_cola_new(int socket)
{
    char* mensaje;
    t_list* instrucciones;
    t_pcb* pcb;
    uint32_t tamanio;
    sem_t recibirInstruccion;
    sem_init(&recibirInstruccion, 0, 0);
    while (1) {
        op_code cod_op = recibir_operacion(socket);
        switch (cod_op) {
            case MENSAJE:
                mensaje = recibir_mensaje(socket);
                tamanio = atoi(mensaje);
                log_debug(kernelLogger, "Me llego el mensaje %i", tamanio);
            continue;
            case INSTRUCCION:
                instrucciones = recibir_instrucciones(socket);
                sem_post(&recibirInstruccion);
            break;
            default:
                sem_wait(&pcbsEnExit);
			    log_error(kernelLogger, "Se desconecto la Consola");
            break;
        }

        sem_wait(&recibirInstruccion);
        uint32_t idPcb = get_siguiente_id();
        //CREAMOS EL PCB
        pcb = pcb_create(idPcb, tamanio, instrucciones, kernelCfg); //utilizar socketEscucha como ID de PCB.
        agregar_lista_sockets(pcb->id,socket);

        log_debug(kernelLogger, "TAMAÑO PCB %i", pcb->tamanio);

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
        t_pcb* pcbTemp = list_get(lista, posicion);
        if (pcb->id == pcbTemp->id) {
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
    //pthread_mutex_lock(&(colaPlanificacion->mutex));
    t_pcb* primerPcb = get_and_remove_primer_pcb_de_cola(colaPlanificacion);
    log_info(kernelLogger, "FIFO: Se toma una instancia de READY, PCB ID %d", primerPcb->id);
    //pthread_mutex_unlock(&(colaPlanificacion->mutex));
    return primerPcb;
}

/*---------------------------------------------- SRT ----------------------------------------------*/

t_pcb* srt_pcb_menor_estimacion_entre(t_pcb* unPcb, t_pcb* otroPcb) {
    return unPcb->est_rafaga_actual <= otroPcb->est_rafaga_actual ? unPcb : otroPcb;
}

t_pcb* elegir_en_base_a_srt(t_cola_planificacion* colaPlanificacion) {
    pthread_mutex_lock(&(colaPlanificacion->mutex));
    t_pcb* pcbMenorEstimacion = list_get_minimum(colaPlanificacion->lista, srt_pcb_menor_estimacion_entre);
    int posicion = pcb_get_posicion(pcbMenorEstimacion, colaPlanificacion->lista);
    if(posicion != -1) {
        list_remove(colaPlanificacion->lista, posicion);
    } else {
        log_error(kernelLogger, "Kernel: No existe tal elemento en la cola");
    }
    pthread_mutex_unlock(&(colaPlanificacion->mutex));
    return pcbMenorEstimacion;
}


double media_exponencial(double realAnterior, double estAnterior) {
    /* Est(n) = α . R(n-1) + (1 - α) . Est(n-1) */
    return kernelCfg->ALFA * realAnterior + (1 - kernelCfg->ALFA) * estAnterior;
}


void calcular_nueva_estimacion_actual(t_pcb* pcb){
    pcb->est_rafaga_actual = media_exponencial(pcb->dur_ultima_rafaga, pcb->est_rafaga_actual);
}


/*---------------------------------------------- CONEXIONES ----------------------------------------------*/

//La conexión de interrupt dedicada solamente a enviar mensajes de interrupción
void conexion_de_interrupt() {

    SOCKET_INTERRUPT = conectar_a_servidor(kernelCfg->IP_CPU, kernelCfg->PUERTO_CPU_INTERRUPT);
    

    if (SOCKET_INTERRUPT == -1)
    {
        log_error(kernelLogger, "Kernel: No se pudo establecer conexión interrupt con CPU. Valor conexión %d", SOCKET_INTERRUPT);
        exit(-1);
    }else{
        log_info(kernelLogger, "Kernel: Conexion a CPU hilo Interrupt exitosa");
    }
    
}

void conexion_de_memoria() {
    SOCKET_MEMORIA = conectar_a_servidor(kernelCfg->IP_MEMORIA, kernelCfg->PUERTO_MEMORIA);

    if (SOCKET_MEMORIA == -1){
        log_error(kernelLogger, "Kernel: No se pudo establecer conexión con Memoria. Valor conexión %d", socketMemoria);
        exit (-1);
    }else{
        log_info(kernelLogger, "Kernel: Conexion a Memoria exitosa");
    }
}


void conexion_de_dispatch() {

    SOCKET_DISPATCH = conectar_a_servidor(kernelCfg->IP_CPU, kernelCfg->PUERTO_CPU_DISPATCH);

    if (SOCKET_DISPATCH <=0)
    {
        log_error(kernelLogger, "Kernel: No se pudo establecer conexión dispatch con CPU. Valor conexión %d", SOCKET_DISPATCH);
        exit(-1);
    }else{
        log_info(kernelLogger, "Kernel: Conexion a CPU Dispatch exitosa");
    }
}

void agregar_lista_sockets(uint32_t id, int socket) {
    t_socket* socketAAgregar= malloc(sizeof(t_socket));
    socketAAgregar->id = id;
    socketAAgregar->socket = socket;
    pthread_mutex_lock(&mutex_lista_sockets);
    list_add(sockets,socketAAgregar);
    pthread_mutex_unlock(&mutex_lista_sockets);
}

void cerrar_socket_de_pid(uint32_t pid) {
    int posicion = buscar_indice_socket_de_pid(pid);
    pthread_mutex_lock(&mutex_lista_sockets);
    if(posicion != -1) {
        t_socket* pcbACerrar = list_remove(sockets, posicion);
        pthread_mutex_unlock(&mutex_lista_sockets);
        log_info(kernelLogger,"Se cierra socket del proceso %i",pcbACerrar->id);
        close(pcbACerrar->socket);
        free(pcbACerrar);
    }
    else{
        pthread_mutex_unlock(&mutex_lista_sockets);
        log_error(kernelLogger, "Kernel: No se encontro el socket de PID %d", pid);
    }
}

int buscar_indice_socket_de_pid(uint32_t pid) {
    int posicion = -1;
    for(int i = 0; i < list_size(sockets); i++) {
        t_socket* socketActual = list_get(sockets, i);
        if(socketActual->id == pid) {
            posicion = i;
            break;
        }
    }
    return posicion;
}

int get_socket_de_pid(uint32_t pid) {
    int posicion = buscar_indice_socket_de_pid(pid);
    if(posicion != -1) {
        t_socket* socketActual = list_get(sockets, posicion);
        return socketActual->socket;
    }
    else{
        log_error(kernelLogger, "Kernel: No se encontro el socket de PID %d", pid);
        return -1;
    }
}


