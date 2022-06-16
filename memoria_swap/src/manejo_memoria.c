#include "manejo_memoria.h"


void* crear_espacio_de_memoria()
{
    return malloc(memoria_swapCfg->TAM_MEMORIA);
}

void escribir_en_memoria(void* memoria, void* contenido, uint32_t marco, uint32_t desplazamiento,int size){
    uint32_t offset = marco * memoria_swapCfg->TAM_PAGINA + desplazamiento;
    memcpy(memoria + offset, contenido, size);
}

void* leer_de_memoria(void* memoria, uint32_t marco, uint32_t desplazamiento, int size)
{
    void* contenido = malloc(size); //con marco
    memcpy(contenido, memoria + desplazamiento, size);
    return contenido;
}
