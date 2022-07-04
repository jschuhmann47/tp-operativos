#include "manejo_memoria.h"



void* crear_espacio_de_memoria()
{
    return malloc(memoria_swapCfg->TAM_MEMORIA);
}

void escribir_en_memoria(void* memoria, void* contenido, uint32_t marco, uint32_t desplazamiento,int size){ //valido si el marco esta libre antes de llamar a esto
    uint32_t offset = (marco * memoria_swapCfg->TAM_PAGINA) + desplazamiento;
    memcpy(memoria + offset, contenido, size);
    //marcar_marco_modificado(marco); todo
}

void* leer_de_memoria(void* memoria, uint32_t marco, uint32_t desplazamiento, int size)
{
    void* contenido = malloc(size);
    memcpy(contenido, memoria + (marco * memoria_swapCfg->TAM_PAGINA) + desplazamiento, size);
    return contenido;
}
