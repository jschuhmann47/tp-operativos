#include "mmu.h"

uint32_t traducir_direccion(uint32_t direccionLogica, 
                          uint32_t tamanioPagina, 
                          uint32_t paginasPorTabla,
                          int socket_memoria){

    uint32_t numeroDePagina = floor(direccionLogica/tamanioPagina);
    


    uint32_t entradaTablaPrimerNivel = floor(numeroDePagina/paginasPorTabla);

    uint32_t entradaTablaSegundoNivel = numeroDePagina % paginasPorTabla;

    uint32_t rtaTablaPrimerNivel,marco;

    uint32_t desplazamiento = direccionLogica - numeroDePagina * tamanioPagina;

}