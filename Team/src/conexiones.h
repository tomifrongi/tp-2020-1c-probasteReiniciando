#include <commons/log.h>
#include <commons/collections/list.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <stdarg.h>
#include <pthread.h>

#include "configuracion.h"
#include "ProtocoloDeMensajes.h"



//PROTOCOL----------------

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

typedef struct{
	t_header head;
	size_t size; //indica el tamaño del content
	void* content;
}t_message;

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

//+CONEXIONES-------------

void* handler_broker(void *cola);
void escuchar_mensajes_gameboy(int listener_socket);
void* handler_appeared(void* socket);
void handler_suscripciones(int socketTeam);

//------------------------

//int crear_servidor(int);
//int aceptar_cliente(struct socktadrr_in);
//int enviar_mensaje(t_header ,size_t,void*);		//Son funciones provisorias que luego seran adptadas a los requisitos del tp.
//t_message* recibir_mensaje(int);
