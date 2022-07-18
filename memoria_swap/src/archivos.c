#include "archivos.h"


pthread_mutex_t accesoAArchivo;

void generar_archivo(uint32_t pid, uint32_t tamanioArchivo)
{
    pthread_mutex_lock(&accesoAArchivo);
    char* path = obtener_path_archivo(pid);
    FILE* archivo = fopen(path, "wb");
    if (ftruncate(fileno(archivo),tamanioArchivo+1) != 0){
        log_error(memoria_swapLogger,"Error al truncar el archivo del proceso %i",pid);
        exit(-1);
    }
    fseek(archivo, 1, SEEK_END);
    fwrite("0", 1, 1, archivo);
    fseek(archivo, 0, SEEK_SET);
    fclose(archivo);
    pthread_mutex_unlock(&accesoAArchivo);
}

void eliminar_archivo(uint32_t pid)
{
    pthread_mutex_lock(&accesoAArchivo);
    char* path = obtener_path_archivo(pid);
    remove(path);
    pthread_mutex_unlock(&accesoAArchivo);
}

void escribir_en_archivo(uint32_t pid, int nroMarco, int nroPagina)
{
    char* path = obtener_path_archivo(pid);
    pthread_mutex_lock(&accesoAArchivo);
    FILE *archivo = fopen(path, "wb");
    void* paqueteAEscribir = leer_de_memoria(MEMORIA_PRINCIPAL,nroMarco,0,memoria_swapCfg->TAM_PAGINA);
    fseek(archivo,nroPagina*memoria_swapCfg->TAM_PAGINA,SEEK_SET);
    fwrite(paqueteAEscribir,memoria_swapCfg->TAM_PAGINA,1,archivo);
    fseek(archivo,0,SEEK_SET);
    fclose(archivo);
    sleep(memoria_swapCfg->RETARDO_SWAP/1000);
    pthread_mutex_unlock(&accesoAArchivo);
    free(paqueteAEscribir);
}

void* leer_de_archivo(uint32_t pid,int nroPagina){
    pthread_mutex_lock(&accesoAArchivo);
    char* path = obtener_path_archivo(pid);
    FILE *archivo = fopen(path, "rb");
    void* lectura = malloc(memoria_swapCfg->TAM_PAGINA);
    fseek(archivo,nroPagina*memoria_swapCfg->TAM_PAGINA,SEEK_SET);
    fread(lectura,memoria_swapCfg->TAM_PAGINA,1,archivo);
    fseek(archivo,0,SEEK_SET);
    fclose(archivo);
    sleep(memoria_swapCfg->RETARDO_SWAP/1000);
    pthread_mutex_unlock(&accesoAArchivo);
    return lectura;
}

char* obtener_path_archivo(uint32_t pid){
    char *path = string_new();
    string_append(&path, memoria_swapCfg->PATH_SWAP);
    string_append(&path, "/");
    string_append(&path, string_itoa(pid));
    string_append(&path, ".swap");
    return path;
}

bool existe_archivo(uint32_t pid){
    char* path = obtener_path_archivo(pid);
    return access(path, F_OK) != -1;
}