#include "../include/protocolo.h"

/*
 * NO_OP: 1 parámetro (este va a ser cuantas veces se hace)
I/O y READ: 1 parámetro
COPY y WRITE: 2 parámetros
EXIT: 0 parámetros
 *
 * */


static void* serializar_exit(){
	void* stream = malloc(sizeof(op_code));

	op_code codigo = EXIT;
	memcpy(stream, &codigo, sizeof(op_code));
	return stream;
}

static void* serializar_no_op(uint32_t param){
	void* stream = malloc(sizeof(op_code) + sizeof(uint32_t));

	op_code codigo = NO_OP;
	memcpy(stream, &codigo, sizeof(op_code));
	memcpy(stream+sizeof(op_code), &param, sizeof(uint32_t));
	return stream;
}

static void* serializar_io(uint32_t param){
	void* stream = malloc(sizeof(op_code) + sizeof(uint32_t));

	op_code codigo = IO;
	memcpy(stream, &codigo, sizeof(op_code));
	memcpy(stream+sizeof(op_code), &param, sizeof(uint32_t));
	return stream;
}


static void* serializar_read(uint32_t param1){
	void* stream = malloc(sizeof(op_code) + sizeof(uint32_t));

	op_code codigo = READ;
	memcpy(stream, &codigo, sizeof(op_code));
	memcpy(stream+sizeof(op_code), &param1, sizeof(uint32_t));
	return stream;
}

static void* serializar_write(uint32_t param1,uint32_t param2){
	void* stream = malloc(sizeof(op_code) + sizeof(uint32_t) * 2);

	op_code codigo = WRITE;
	memcpy(stream, &codigo, sizeof(op_code));
	memcpy(stream+sizeof(op_code), &param1, sizeof(uint32_t));
	memcpy(stream+sizeof(op_code)+sizeof(uint32_t), &param2, sizeof(uint32_t));
	return stream;
}


static void deserializar_no_op(void* stream, uint32_t* param) {
    memcpy(param, stream, sizeof(uint32_t));
}

static void deserializar_io(void* stream, uint32_t* param) {
    memcpy(param, stream, sizeof(uint32_t));
}

static void deserializar_read(void* stream, uint32_t* param) {
    memcpy(param, stream, sizeof(uint32_t));
}

static void deserializar_copy(void* stream, uint32_t* param1,uint32_t* param2) {
    memcpy(param1, stream, sizeof(uint32_t));
    memcpy(param2, stream+sizeof(uint32_t), sizeof(uint32_t));
}

static void deserializar_write(void* stream, uint32_t* param1,uint32_t* param2) {
    memcpy(param1, stream, sizeof(uint32_t));
    memcpy(param2, stream+sizeof(uint32_t), sizeof(uint32_t));
}

//deserializar exit no hace falta, xq lee el cod op "exit" y termina



