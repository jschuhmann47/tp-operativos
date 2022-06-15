#ifndef PLANIFICADOR_H_INCLUDED
#define PLANIFICADOR_H_INCLUDED

#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <commons/collections/list.h>

#include "structs.h"
#include "conexiones.h"
#include "kernel_config.h"
#include "kernel.h"


typedef struct{
    uint32_t* id;
    int* socket;
} t_socket;


void iniciar_planificacion();

// Planificador Corto Plazo
void* iniciar_corto_plazo(void* _);
void* conexion_de_interrupt();
void* conexion_de_dispatch();
void interrupcion_a_cpu();
void mandar_pcb_a_cpu(t_pcb* pcb);
void* traer_pcb_de_cpu();
//void determinar_ready_o_blocked(t_pcb* pcb);
bool instruccion_actual_es(t_pcb* pcb, code_instruccion codOp);
void atender_procesos_bloqueados(uint32_t tiempoBloqueadoPorIo);


// Planificador Mediano Plazo
void* pasar_de_susready_a_ready(void* _);
void enviar_suspension_de_pcb_a_memoria(t_pcb* pcb);
void* contar_tiempo_bloqueado(t_pcb* pcb);
void suspender_tiempo_de_io(t_pcb* pcb, uint32_t tiempo);

// Planificador Largo Plazo
void* iniciar_largo_plazo(void* _);
void* liberar_procesos_en_exit(void* _);
void solicitar_nueva_tabla_memoria();

// Manejo de PCBs
t_pcb* pcb_create(uint32_t id, uint32_t tamanio, t_list* instrucciones, t_kernel_config* config);
void pcb_destroy(t_pcb *pcb);
void destruir_instruccion(t_instruccion* instruccion);
void cambiar_estado_pcb(t_pcb* pcb, t_status nuevoEstado);
int get_grado_multiprog_actual(void);
void log_transition(const char* entityName, const char* prev, const char* post, int pid);
void agregar_pcb_en_cola_new(int socket);
uint32_t get_siguiente_id();

// Manejo de colas
t_cola_planificacion* cola_planificacion_create(int semInitVal);
t_pcb* get_and_remove_primer_pcb_de_cola(t_cola_planificacion* cola);
int pcb_get_posicion(t_pcb* pcb, t_list* lista);
void agregar_pcb_a_cola(t_pcb* pcb, t_cola_planificacion* cola);
void remover_pcb_de_cola(t_pcb* pcb, t_cola_planificacion* cola);

// Algoritmos
t_pcb* elegir_pcb_segun_algoritmo(t_cola_planificacion* cola);
bool algoritmo_fifo_loaded(void);
bool algoritmo_srt_loaded(void);

// FIFO
t_pcb* elegir_en_base_a_fifo(t_cola_planificacion* colaPlanificacion);

// SRT
t_pcb* srt_pcb_menor_estimacion_entre(t_pcb* unPcb, t_pcb* otroPcb);
t_pcb* elegir_en_base_a_srt(t_cola_planificacion* colaPlanificacion);
double media_exponencial(double realAnterior, double estAnterior);
void calcular_nueva_estimacion_actual(t_pcb* pcb);

//Conexiones
void agregar_lista_sockets(uint32_t id, int socket);
void cerrar_socket_de_pid(uint32_t pid);
int buscar_indice_socket_de_pid(uint32_t pid);
int get_socket_de_pid(uint32_t pid);
void* conexion_de_memoria();




#endif