#ifndef PROTOCOLO_H
#define PROTOCOLO_H


typedef enum {
	NO_OP, IO,COPY,READ,WRITE,EXIT
}cod_op;

static void* serializar_exit();
static void* serializar_no_op(uint32_t param);
static void* serializar_io(uint32_t param);
static void* serializar_read(uint32_t param1);
static void* serializar_write(uint32_t param1,uint32_t param2);
static void deserializar_no_op(void* stream, uint32_t* param);
static void deserializar_io(void* stream, uint32_t* param);
static void deserializar_read(void* stream, uint32_t* param);
static void deserializar_copy(void* stream, uint32_t* param1,uint32_t* param2);
static void deserializar_write(void* stream, uint32_t* param1,uint32_t* param2);

#endif
