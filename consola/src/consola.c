#include "consola.h"

int main(int argc, char *argv[])
{

    consolaLogger = log_create(CONSOLA_LOG_DEST, CONSOLA_MODULE_NAME, true, LOG_LEVEL_INFO);

    // ACEPTAR PARAMETROS POR EL MAIN
    char *url = argv[1];
    char *tamanio = argv[2];

    // LEER ARCHIVO DE PSEUDO
    char *instruccionesAEnviar = leer_archivo(url);

    // LEER CONFIG
    consolaCfg = consola_cfg_create();
    cargar_configuracion(CONSOLA_MODULE_NAME, consolaCfg, CONSOLA_CFG_PATH, consolaLogger, consola_config_initialize);

    // CONECTAR A KERNEL
    int socketKernel = conectar_a_servidor(consolaCfg->IP_KERNEL, consolaCfg->PUERTO_KERNEL);

    if (socketKernel == -1)
    {
        log_error(consolaLogger, "Consola: No se pudo establecer conexión con Kernel. Valor conexión %d", socketKernel);
        return -1;
    }

    // ENVIAR INFO A KERNEL - Lista de instrucciones y tamaño

    enviar_mensaje(tamanio, socketKernel, MENSAJE);
    enviar_mensaje(instruccionesAEnviar, socketKernel, INSTRUCCION);
    
    // ESPERAR MENSAJE DE FINALIZACION
    char* mensaje = recibir_mensaje_finalizacion(socketKernel);
    if(mensaje){
        terminar_conexion(socketKernel);
    }

    // TERMINAR
    liberar_modulo_consola(consolaLogger, consolaCfg);

    return EXIT_SUCCESS;
}

char *leer_archivo(char *nombre)
{
    int posicion;
    int cantidadLineas = 0;
    char temp[16];
    char delimitador[] = " ";

    FILE *file = fopen(nombre, "r");

    if (file == NULL)
    {
        log_error(consolaLogger, "Error al abrir el archivo");
    }

    while (!feof(file))
    {
        fgets(temp, 15, file);
        cantidadLineas++;
    }

    rewind(file);

    char palabras[cantidadLineas][16];
    posicion = 0;
    while (!feof(file))
    {
        fgets(palabras[posicion], 15, file);
        posicion++;
    }
    char *mensaje = string_new();
    for (int posicion = 0; posicion < cantidadLineas; posicion++)
    {
        char **token = string_split(palabras[posicion], delimitador);
        char *indicador = token[0];

        if (indicador != NULL)
        {
            if (strcmp(indicador, "NO_OP") == 0)
            {
                int repeticiones = atoi(token[1]); // atoi convierte una cadena en un entero: aca seria "5" -> 5
                for (int i = 0; i < repeticiones; i++)
                {
                    mensaje = agregarInstruccion(token, mensaje);
                    log_info(consolaLogger, "mensaje ---->: %s", mensaje);
                }
            }
            else
            {
                mensaje = agregarInstruccion(token, mensaje);
                log_info(consolaLogger, "mensaje ---->: %s", mensaje);
            }
        }
    }

    return mensaje;
}