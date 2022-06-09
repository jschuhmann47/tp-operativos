#include "archivos.h"


pthread_mutex_t accesoAArchivo;



void generar_archivo(uint32_t pid, char *contenido)
{
    pthread_mutex_lock(&accesoAArchivo);
    char *path = string_new();
    string_append(&path, memoria_swapCfg->PATH_SWAP);
    string_append(&path, string_itoa(pid));
    string_append(&path, ".swap");
    FILE *archivo = fopen(path, "a+"); //a+: append y read
    
    //escribir lo que haya que escribir
    
    //fseek(archivo, (pagina * tamanio), SEEK_SET);
    //fwrite(contenido, sizeof(contenido)+1, tamanio, archivo);
    fclose(archivo);
    pthread_mutex_unlock(&accesoAArchivo);
}

void eliminar_archivo(uint32_t pid)
{
    pthread_mutex_lock(&accesoAArchivo);
    char *path = string_new();
    string_append(&path, memoria_swapCfg->PATH_SWAP);
    string_append(&path, string_itoa(pid));
    string_append(&path, ".swap");
    remove(path);
    pthread_mutex_unlock(&accesoAArchivo);
}