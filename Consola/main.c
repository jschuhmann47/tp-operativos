
#include "include/main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Nodo{
	char *Codigo;
	int  Parametro1;
	int  Parametro2;
	struct Nodo *sig;
} Node;

typedef struct ListaIdentificar {
	Node *inicio;
	Node *fin;
	int tamanio;
}Lista;

int Inicializar(Lista*);
int Agregar(Lista*, Node*, struct Instruccion);
void Imprimir(Lista*);

t_log* consola_logger;

int main(){
	//log_create(char* file, char *process_name, bool is_active_console, t_log_level level)
	consola_logger = log_create("consola.log","CONSOLA",1,LOG_LEVEL_DEBUG);


	Lista* lista;
	Node* actual;
	actual = NULL;
	int resultAgregar = 0;
	int resultIniciar = 0;

 	resultIniciar = Inicializar(lista);

	if (resultIniciar == -1)
		 exit(EXIT_FAILURE);

	FILE* archivo = fopen("proceso1.txt","r");
	while(!feof(archivo) ){
		struct Instruccion InstruccionLeida = parser(archivo);
		resultAgregar = Agregar(lista, lista->fin, InstruccionLeida);	
	}

	Imprimir(lista);
	fclose(archivo);
	

	//Luego, leerá su archivo de configuración, se conectará al Kernel y le enviará toda la información del proceso (lista de instrucciones, tamaño).

	t_config* consola_config=config_create("consola.config");
	char* ip_kernel=config_get_string_value(consola_config, "IP_KERNEL");
	char* puerto_kernel=config_get_string_value(consola_config, "PUERTO_KERNEL");

	int consola_fd=crear_conexion_a_server(consola_logger,"Kernel",ip_kernel,puerto_kernel);

	//uint32_t handshake = 1;
	//uint32_t result;

	//send(consola_fd, &handshake, sizeof(uint32_t), 0);
	//recv(consola_fd, &result, sizeof(uint32_t), MSG_WAITALL);

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




