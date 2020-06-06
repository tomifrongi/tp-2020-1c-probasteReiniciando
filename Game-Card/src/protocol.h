/*
 * protocol.h
 *
 *  Created on: 8 sep. 2019
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#ifndef PROTOCOL_H_
#define PROTOCOL_H_


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
	size_t size;
	void* content;
}t_message;

t_message* create_t_message(t_header head, size_t size,const void* content);

int send_message(int socket, t_header head,const void* content, size_t size);

t_message* recv_message(int socket);

void free_t_message(t_message* message);

t_message* error(int res);


int send_status(int sock,t_header head, int status);

int get_status(t_message* message);
#endif /* PROTOCOL_H_ */
