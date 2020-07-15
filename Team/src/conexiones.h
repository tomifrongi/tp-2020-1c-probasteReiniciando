#include <commons/log.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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


int crear_servidor(int);
int aceptar_cliente(struct socktadrr_in);
int enviar_mensaje(t_header ,size_t,void*);		//Son funciones provisorias que luego seran adptadas a los requisitos del tp.
t_message* recibir_mensaje(int);
