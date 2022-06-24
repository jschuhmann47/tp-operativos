#include "archivos.h"


pthread_mutex_t accesoAArchivo;



void generar_archivo(uint32_t pid)
{
    pthread_mutex_lock(&accesoAArchivo);
    char* path = obtener_path_archivo(pid);
    FILE *archivo = fopen(path, "w");
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

void escribir_en_archivo(uint32_t pid, int nroMarco){
    char* path = obtener_path_archivo(pid);
    pthread_mutex_lock(&accesoAArchivo);
    FILE *archivo = fopen(path, "w");
    void* paqueteAEscribir = leer_de_memoria(MEMORIA_PRINCIPAL,nroMarco,0,memoria_swapCfg->TAM_PAGINA);
    fseek(archivo,nroMarco*memoria_swapCfg->TAM_PAGINA,SEEK_SET);
    fwrite(paqueteAEscribir,memoria_swapCfg->TAM_PAGINA,1,archivo);
    //ver si esto anda, tmb existe mmap() pero hay que ver como se usa
    fseek(archivo,0,SEEK_SET);
    fclose(archivo);
    pthread_mutex_unlock(&accesoAArchivo);
}

void* leer_de_archivo(uint32_t pid,int nroMarco){
    pthread_mutex_lock(&accesoAArchivo);
    char* path = obtener_path_archivo(pid);
    FILE *archivo = fopen(path, "r");
    void* lectura;
    fseek(archivo,nroMarco*memoria_swapCfg->TAM_PAGINA,SEEK_SET);
    fread(lectura,memoria_swapCfg->TAM_PAGINA,1,archivo);
    fseek(archivo,0,SEEK_SET);
    fclose(archivo);
    pthread_mutex_unlock(&accesoAArchivo);

    return lectura; //ver como formatear esto, es el contenido del frame tal cual
}

char* obtener_path_archivo(uint32_t pid){
    char *path = string_new();
    string_append(&path, memoria_swapCfg->PATH_SWAP);
    string_append(&path, string_itoa(pid));
    string_append(&path, ".swap");
    return path;
}