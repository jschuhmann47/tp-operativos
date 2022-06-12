#include "mmu.h"

uint32_t traducir_direccion(uint32_t direccionLogica, 
                          uint32_t tamanioPagina, 
                          uint32_t paginasPorTabla)
{
    uint32_t numeroDePagina = floor(direccionLogica/tamanioPagina);
    uint32_t entradaTablaPrimerNivel = floor(numeroDePagina/paginasPorTabla);
    uint32_t entradaTablaSegundoNivel = numeroDePagina % paginasPorTabla;
    uint32_t desplazamiento = direccionLogica - numeroDePagina * tamanioPagina;

    log_info(cpuLogger, "CPU: Numero de Pagina: %i", numeroDePagina);
    log_info(cpuLogger, "CPU: Tabla Primer Nivel: %i", entradaTablaPrimerNivel);
    log_info(cpuLogger, "CPU: Tabla Segundo Nivel: %i", entradaTablaSegundoNivel);
    log_info(cpuLogger, "CPU: Desplazamiento: %i", desplazamiento);
}