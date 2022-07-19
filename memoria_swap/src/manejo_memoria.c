#include "manejo_memoria.h"



void* crear_espacio_de_memoria()
{   
    void* mp = malloc(memoria_swapCfg->TAM_MEMORIA);
    memset(mp,0,memoria_swapCfg->TAM_MEMORIA);
    return mp;
}

void escribir_en_memoria(void* memoria, void* contenido, uint32_t marco, uint32_t desplazamiento,int size){
    uint32_t offset = (marco * memoria_swapCfg->TAM_PAGINA) + desplazamiento;
    memcpy(memoria + offset, contenido, size);
}

void* leer_de_memoria(void* memoria, uint32_t marco, uint32_t desplazamiento, int size)
{
    void* contenido = malloc(size);
    memcpy(contenido, memoria + (marco * memoria_swapCfg->TAM_PAGINA) + desplazamiento, size);
    return contenido;
}
