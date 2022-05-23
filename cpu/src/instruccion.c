#include "cpu.h"


// typedef struct t_pcb t_pcb; // Se declara este para poder usar el del "algortimo_update"
// struct t_pcb
// {
//     uint32_t id;        //como hacemos incremental el id? buscar.
//     t_status status;
//     uint32_t tamanio;
//     t_list *instrucciones;
//     void *programCounter;
//     // TODO: Tabla de paginas
//     // Estos dos ultimos solo se usan cuando es SRT
//     double est_rafaga_actual; // Esta en Milisegundos
//     void (*algoritmo_siguiente_estim)(t_pcb *self, time_t tiempoFinal, time_t tiempoInicial);
// };

void ejecutar_instruccion(t_pcb* pcb){
    t_instruccion instruccionAEjecutar = cpu_fetch(pcb); //utilizaremos el Program Counter (también llamado Instruction Pointer),  que representa el número de instrucción a buscar, para buscar la próxima instrucción. Al finalizar el ciclo, este último deberá ser actualizado (sumarle 1).
    cpu_decode(instruccionAEjecutar);
}




t_instruccion cpu_fetch (t_pcb* pcb){

    return list_get(pcb->instrucciones,programCounter); //uint32_t o int
}

void cpu_decode(t_instruccion instruccion){
    if(instruccion.codigo_operacion == COPY){
        cpu_fetch_operands(instruccion)
    }
    


}