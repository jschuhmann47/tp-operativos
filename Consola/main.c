#include "include/main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../shared/include/protocolo.h"
#include <commons/collections/list.h>



int main(int argc, char**argv){
	//log_create(char* file, char *process_name, bool is_active_console, t_log_level level)
	consola_logger = log_create("consola.log","CONSOLA",1,LOG_LEVEL_DEBUG);

	char **instruccionLeida;
	Lista* lista;
	Node* actual;
	actual = NULL;
	int resultAgregar = 0;
	int resultIniciar = 0;

 	resultIniciar = Inicializar(lista);

	if (resultIniciar == -1)
		 exit(EXIT_FAILURE);

	t_list *listaInstrucciones = list_create();

	FILE* archivo = fopen(argv[1],"r"); //argv del archivo

	t_paquete instruccionAEncolar;


	while(!feof(archivo) ){
		char** lectura = parser(archivo);
		op_code codigoOp= devolverCodigoOperacion(lectura[0]);
		//pasarla a: op_code / size params / char params //todo
		void* instSerializada=serializar_instruccion(codigoOp,lectura); //si no tiene 2 ??
		resultAgregar = Agregar(lista, lista->fin, instSerializada);//A
		list_add(listaInstrucciones,instSerializada); //B
	}


	uint32_t cantInstrucciones=list_size(listaInstrucciones);
	Imprimir(lista);
	fclose(archivo);
	
	int sizeAMandar=0;
	void * paquete_a_mandar = preparar_paquete(cantInstrucciones,listaInstrucciones,&sizeAMandar); //todo

	t_config* consola_config=config_create("consola.config");
	char* ip_kernel=config_get_string_value(consola_config, "IP_KERNEL");
	char* puerto_kernel=config_get_string_value(consola_config, "PUERTO_KERNEL");

	int consola_fd=crear_conexion_a_server(consola_logger,"Kernel",ip_kernel,puerto_kernel);

	uint32_t handshake=1;
	uint32_t result;
	//handshake
	if(send(consola_fd, &handshake, sizeof(uint32_t), 0)==-1){
		return EXIT_FAILURE;
	}
	if(recv(consola_fd, &result, sizeof(uint32_t), MSG_WAITALL)==-1){
		return EXIT_FAILURE;
	}else{
		return EXIT_SUCCESS;
	}

	//mensaje en si
	 //todo
	//loggear todo esto con lo de handshake ok, instrucciones mandadas, etc

	if(send(consola_fd, &paquete_a_mandar, sizeAMandar, 0)==-1){ //uno de cant_inst y el
			return EXIT_FAILURE;
	}
	if(recv(consola_fd, &result, sizeAMandar, MSG_WAITALL)==-1){
		return EXIT_FAILURE;
	}else{
		return EXIT_SUCCESS;
	}


	return 0;

}

int Inicializar (Lista *lista){

if ((lista = (Lista *) malloc (sizeof (Lista))) == NULL)
		return -1;
  lista->inicio = NULL;
  lista->fin = NULL;
  lista->tamanio = 0;
  return 0;
}

int Agregar(Lista* lista, Node* actual, struct Instruccion dato)
{
	Node *nuevo_nodo;
	nuevo_nodo = (Node *) malloc(sizeof(Node));
	
	if(nuevo_nodo == NULL)
	{
		return -1;
	}
	if ((nuevo_nodo->Codigo = (char *) malloc (50 * sizeof (char))) == NULL) 
	{
		return -1;
	}
		//Copio los datos al nuevo nodo
		strcpy (nuevo_nodo->Codigo, dato.codigo_instruccion);
		nuevo_nodo->Parametro1 = dato.parametros[0];
		nuevo_nodo->Parametro2 = dato.parametros[1];
		nuevo_nodo->sig = NULL;	
		
		//Insertar al final
		actual->sig = nuevo_nodo; 
		nuevo_nodo->sig = NULL; 
		lista->fin = nuevo_nodo; 
		lista->tamanio++; 
	
	return 0;
}


void Imprimir (Lista * lista){
  Node *actual;
  actual = lista->inicio;
  printf("\nLista:\n\n");
  
  while (actual != NULL){
      printf ("%s\n", actual->Codigo);
      //printf ("%d\n", actual.Parametro1);
      //printf ("%d\n", actual.Parametro2);
      actual = actual->sig;
  }
}

uint32_t string_to_uint(char* string){
	uint32_t result = 0;
    int len = strlen(string);

	for(int i=0; i<len; i++){
		result = result * 10 + ( string[i] - '0' );
	}

	return result;
}

op_code devolverCodigoOperacion(char* palabra){

	if(strcmp(palabra,"EXIT")==0){
		return EXIT;
	}
	if(strcmp(palabra,"NO_OP")==0){
		return NO_OP;
	}
	if(strcmp(palabra,"I/O")==0){
		return IO;
	}
	if(strcmp(palabra,"READ")==0){
		return READ;
	}
	if(strcmp(palabra,"WRITE")==0){
		return WRITE;
	}
	if(strcmp(palabra,"COPY")==0){
		return COPY;
	}
	return EXIT_FAILURE;
}


void* serializar_instruccion(op_code opCode,char** leida){
	switch(opCode){
		case(NO_OP):
				return serializar_no_op(string_to_uint(leida[1]));
				break;
		case(EXIT):
				return serializar_exit();
				break;
		case(IO):
				return serializar_io(string_to_uint(leida[1]));
				break;
		case(READ):
				return serializar_read(string_to_uint(leida[1]));
				break;
		case(WRITE):
				return serializar_write(string_to_uint(leida[1]),string_to_uint(leida[2]));
				break;
		case(COPY):
				return serializar_write(string_to_uint(leida[1]),string_to_uint(leida[2]));
				break;
	}
	return NULL;
}


void *preparar_paquete(list_size(listaInstrucciones),t_list *listaInstrucciones,int *sizeAMandar){
	//concatenar todo, liberar dps de concatenar, y actualizar sizeAMandar
}
