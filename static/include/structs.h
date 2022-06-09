#ifndef STATIC_STRUCTS_H_INCLUDED
#define STATIC_STRUCTS_H_INCLUDED

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <string.h>
#include "conexiones.h"

#include <commons/collections/list.h>

typedef enum
{
    MENSAJE,
    INSTRUCCION
} op_code;

typedef enum
{
    NO_OP,
    I_O,    // I_O === I/
    WRITE,
    COPY,
    READ,
    EXIT_I  // EXIT_I === EXIT -> SI PONIAMOS EXIT ENTRABA EN CONFLICTO CON EL ENUM DE PLANIFICACION, LINEA 35
} code_instruccion;

/*----------------------------- PLANIFICACION -----------------------------*/
typedef enum
{
    NEW,
    READY,
    EXEC,
    BLOCKED,
    SUSBLOCKED,
    SUSREADY,
    EXIT
} t_status;

// TODO: Ver si cuando tomamos el archivo de instrucciones lo podemos parsear a esta estructura
typedef struct
{
    code_instruccion indicador;
    t_list *parametros;
} t_instruccion;

typedef struct t_pcb t_pcb; // Se declara este para poder usar el del "algortimo_update"
struct t_pcb
{
    uint32_t id;        //como hacemos incremental el id? buscar.
    t_status status;
    uint32_t tamanio;
    t_list *instrucciones;
    uint32_t programCounter;
    // TODO: Tabla de paginas
    // Estos dos ultimos solo se usan cuando es SRT
    double est_rafaga_actual; // Esta en Milisegundos
    double dur_ultima_rafaga;
    //void (*algoritmo_siguiente_estim)(t_pcb *self, time_t tiempoFinal, time_t tiempoInicial);
};

typedef struct
{
    t_list *lista;
    sem_t instanciasDisponibles; // Grado de Multiprogramacion
    pthread_mutex_t mutex;
} t_cola_planificacion;

typedef struct t_mensaje_tamanio {
    uint32_t tamanio;
} t_mensaje_tamanio;

typedef struct{
	uint32_t size;
	void* stream;
} t_buffer;

typedef struct
{
    int socket;
    struct sockaddr sockAddr;
    socklen_t sockrAddrLen;
} conexion;

#endif

