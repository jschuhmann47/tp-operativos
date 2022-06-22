#ifndef STATIC_COMMON_UTILS_H_INCLUDED
#define STATIC_COMMON_UTILS_H_INCLUDED

#include <stdlib.h>
#include <sys/socket.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <string.h>
#include "structs.h"

///////////////////////////// CONFIGS /////////////////////////////
int cargar_configuracion(const char *nombreModulo, void *moduleCfg, char *configPath, t_log *logger,
		void (*cargar_miembros)(void *cfg, t_config *localCfg));
char* agregarInstruccion(char **intruccion, char *mensaje);
t_instruccion *crear_instruccion(void);
int sizeArray(char **array);
uint32_t string_to_uint(char* string);
code_instruccion getCodeIntruccion(char *code);

uint32_t enviar_tamanio_mensaje(t_mensaje_tamanio *msjeTamanio, int socket);
char *serializar_tamanio(t_mensaje_tamanio *mensaje_tamanio, uint32_t *bytes);
void* serializar_pcb(t_pcb *pcb, uint32_t *bytes);
t_mensaje_tamanio* deserializar_tamanio(char *buffer);
uint32_t recibir_tamanio_mensaje(t_mensaje_tamanio *tamanio_mensaje, int socket);
t_pcb* recibir_pcb(void* buffer,uint32_t bytes);

//Utilities//
uint32_t tamanioInstruccion(code_instruccion codOp);
int mandar_instruccion(code_instruccion codOp,uint32_t param1,uint32_t param2,int socket);

void pcb_destroy(t_pcb *pcb);
void destruir_instruccion(t_instruccion* instruccion);


#endif
