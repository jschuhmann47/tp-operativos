#include "../Consola/main.c"
#include <cspecs/cspec.h>
#include <string.h>


context (test_de_strings) {
    describe("consola") {
        it("serializar instruccion") {
            void* instruccion = malloc(sizeof(uint32_t)*2+sizeof(op_code));
            op_code codop=COPY;
            uint32_t param1=100;
            uint32_t param2=4;
            char** mandar = ["100","4",NULL];
            memcpy(instruccion,&codop,sizeof(op_code));
            memcpy(instruccion+sizeof(op_code),&param1,sizeof(uint32_t));
            memcpy(instruccion+sizeof(op_code)+sizeof(uint32_t),&param2,sizeof(uint32_t));
            should_ptr(serializar_instruccion(COPY,mandar)) be equal to(instruccion);
        } end
        it("devuelve 0 para una cadena vacía") {
            should_int(strlen("")) be equal to(0);
        } end
    } end
}


// void *preparar_paquete(uint32_t cantInstrucciones,t_list *listaInstrucciones,int *sizeAMandar){
// 	//concatenar todo, liberar dps de concatenar, y actualizar sizeAMandar
// 	void* stream = malloc(sizeof(uint32_t));
// 	uint32_t desplazamiento = 0;
// 	for (int i = 0;i<cantInstrucciones;i++){
// 		uint32_t instruccionActual = list_get(listaInstrucciones,i);
// 		memcpy(stream,instruccionActual,sizeof(instruccionActual)+desplazamiento);
// 		desplazamiento+=sizeof(instruccionActual);
// 	}
// 	sizeAMandar=sizeof(stream);
// 	return stream;
// }