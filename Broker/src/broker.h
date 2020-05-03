#ifndef BROKER_H_
#define BROKER_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <syscall.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <commons/string.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <sys/mman.h>
#include <pthread.h>
#include <commons/collections/queue.h>
#include "colasDeMensajes.h"

//Parametros a recibir desde el config
int TAMANIO_MEMORIA; //Tamaño de la memoria total
int PUERTO; //Puerto del socket
int ID_INICIAL;
pthread_mutex_t mut_listaProgramas; //mutex de la lista de programas actuales, podria ser uno por cola?
int listener_socket;
t_log* logger;

#define MAX_CLIENTS 128 //Queda por definir

typedef enum{
	NEW_POKEMON,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,
	ERROR,
	NO_CONNECTION = 100,
	ERROR_RECV = 101,
	HI_PLEASE_BE_MY_FRIEND = 102,
	ERROR_MESSAGE = 103
}t_header;

typedef struct{
	t_header head;
	size_t size;
	void* content;
}t_message;

void cargarConfiguracion();
void inicializarLogger(char* path);
void* handler_clients(void* socket);
void init_broker_server();
int connect_to_server(char* host,int port, void*(*callback)());
int init_server(int port);
int create_socket();
int send_message(int socket, t_header head,const void* content, size_t size);
void free_t_message(t_message* message);
t_message* recv_message(int socket);
t_message* create_t_message(t_header head, size_t size,const void* content);
t_message* error(int res);
int send_status(int sock,t_header head, int status);
int get_status(t_message* message);


#endif
