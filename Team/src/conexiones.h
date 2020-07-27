#include <commons/log.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

//PROTOCOL----------------

typedef struct{
	t_header head;
	size_t size; //indica el tamaño del content
	void* content;
}t_message;

typedef enum{
	NEW_POKEMON,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,
	CONFIRMACION,
	SUSCRIPCION,
	ERROR,
	NO_CONNECTION = 100,
	ERROR_RECV = 101,
	HI_PLEASE_BE_MY_FRIEND = 102,
	ERROR_MESSAGE = 103
}t_header;

t_message* create_t_message(t_header head, size_t size,const void* content);
int send_message(int socket, t_header head,const void* content, size_t size);
t_message* recv_message(int socket);
void free_t_message(t_message* message);
t_message* error(int res);
int send_status(int sock,t_header head, int status);
int get_status(t_message* message);

//------------------------

//NET---------------------

#define MAX_CLIENTS 128

int connect_to_server(char* host, int port, void*(*callback)());
int create_socket();
int init_server(int port);

//------------------------


int crear_servidor(int);
int aceptar_cliente(struct socktadrr_in);
int enviar_mensaje(t_header ,size_t,void*);		//Son funciones provisorias que luego seran adptadas a los requisitos del tp.
t_message* recibir_mensaje(int);
