#include "archivos.h"


pthread_mutex_t accesoAArchivo;

void generar_archivo(uint32_t pid, uint32_t tamanioArchivo)
{
    pthread_mutex_lock(&accesoAArchivo);
    char* path = obtener_path_archivo(pid);
    FILE* archivo = fopen(path, "w");
    if (archivo == NULL) {
        log_error(memoria_swapLogger, "Error al crear archivo");
        log_error(memoria_swapLogger, "Error: %s", strerror(errno));
        exit(-1);
    }
    if (ftruncate(fileno(archivo),tamanioArchivo+sizeof(char)) != 0){
        log_error(memoria_swapLogger,"Error al truncar el archivo del proceso %i",pid);
        log_error(memoria_swapLogger, "Error: %s", strerror(errno));
        exit(-1);
    }
    fseek(archivo, -1, SEEK_END);
    if(fwrite("\0", sizeof(char), 1, archivo)!=sizeof(char)){
        log_error(memoria_swapLogger, "Error al setear 0 al final del archivo");
        log_error(memoria_swapLogger, "Error: %s", strerror(errno));
        exit(-1);
    }
    rewind(archivo);
    if(fclose(archivo)!=0){
        log_error(memoria_swapLogger, "Error al cerrar archivo");
        log_error(memoria_swapLogger, "Error: %s", strerror(errno));
        exit(-1);
    }
    pthread_mutex_unlock(&accesoAArchivo);
    free(path);
}

void eliminar_archivo(uint32_t pid)
{
    pthread_mutex_lock(&accesoAArchivo);
    char* path = obtener_path_archivo(pid);
    remove(path);
    pthread_mutex_unlock(&accesoAArchivo);
    free(path);
}

void escribir_en_archivo(uint32_t pid, int nroMarco, int nroPagina)
{
    char* path = obtener_path_archivo(pid);
    pthread_mutex_lock(&accesoAArchivo);
    FILE *archivo = fopen(path, "r+");
    if (archivo == NULL) {
        log_error(memoria_swapLogger, "Error al abrir archivo");
        log_error(memoria_swapLogger, "Error: %s", strerror(errno));
        exit(-1);
    }
    void* paqueteAEscribir = leer_de_memoria(MEMORIA_PRINCIPAL,nroMarco,0,memoria_swapCfg->TAM_PAGINA);
    if(fseek(archivo,nroPagina*memoria_swapCfg->TAM_PAGINA,SEEK_SET)!=0){
        log_error(memoria_swapLogger, "Error al hacer seek en archivo");
        log_error(memoria_swapLogger, "Error: %s", strerror(errno));
        exit(-1);
    }
    uint32_t bytes;
    if((bytes=fwrite(paqueteAEscribir,sizeof(char),memoria_swapCfg->TAM_PAGINA,archivo))!=memoria_swapCfg->TAM_PAGINA){
        log_error(memoria_swapLogger, "Error al escribir archivo. deberia %i, escribe %i",memoria_swapCfg->TAM_PAGINA,bytes);
        log_error(memoria_swapLogger, "Error: %s", strerror(errno));
        exit(-1);
    }
    rewind(archivo);
    if(fclose(archivo)!=0){
        log_error(memoria_swapLogger, "Error al cerrar archivo");
        log_error(memoria_swapLogger, "Error: %s", strerror(errno));
        exit(-1);
    }
    sleep(memoria_swapCfg->RETARDO_SWAP/1000);
    pthread_mutex_unlock(&accesoAArchivo);
    free(paqueteAEscribir);
    free(path);
}

void* leer_de_archivo(uint32_t pid,int nroPagina){
    pthread_mutex_lock(&accesoAArchivo);
    char* path = obtener_path_archivo(pid);
    FILE *archivo = fopen(path, "r+");
    if (archivo == NULL) {
        log_error(memoria_swapLogger, "Error al abrir archivo");
        log_error(memoria_swapLogger, "Error: %s", strerror(errno));
        exit(-1);
    }
    void* lectura = malloc(memoria_swapCfg->TAM_PAGINA);
    fseek(archivo,nroPagina*memoria_swapCfg->TAM_PAGINA,SEEK_SET);
    uint32_t bytes;
    if((bytes=fread(lectura,sizeof(char),memoria_swapCfg->TAM_PAGINA,archivo))!=memoria_swapCfg->TAM_PAGINA){
        log_error(memoria_swapLogger, "Error al leer archivo. deberia %i, leyo %i",memoria_swapCfg->TAM_PAGINA,bytes);
        exit(-1);
    }
    rewind(archivo);
    if(fclose(archivo)!=0){
        log_error(memoria_swapLogger, "Error al cerrar archivo");
        log_error(memoria_swapLogger, "Error: %s", strerror(errno));
        exit(-1);
    }
    sleep(memoria_swapCfg->RETARDO_SWAP/1000);
    pthread_mutex_unlock(&accesoAArchivo);
    free(path);
    return lectura;
}

char* obtener_path_archivo(uint32_t pid){
    char *path = string_new();
    char *stringPid = string_itoa(pid);
    string_append(&path, memoria_swapCfg->PATH_SWAP);
    string_append(&path, "/");
    string_append(&path, stringPid);
    string_append(&path, ".swap");
    free(stringPid);
    return path;
}