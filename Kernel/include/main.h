#ifndef MODKERNEL_H
#define MODKERNEL_H


struct PCB{
	uint32_t id; //o int???
	uint32_t tamanio; //va a ser constante durante toda la ejecucion
	//struct Intstruciones instrucciones; //lista de instrucciones a ejecutar.
	uint32_t program_counter; //número de la próxima instrucción a ejecutar.
	uint32_t tabla_paginas; //tabla de páginas del proceso en memoria, esta información la tendremos recién cuando el proceso pase a estado READY.
	uint32_t estimacion_rafaga; //Estimación utilizada para planificar los procesos en el algoritmo SRT,
							//la misma tendrá un valor inicial definido por archivo de configuración y será recalculada bajo la fórmula de promedio ponderado vista en clases.
}pcb;

#endif
