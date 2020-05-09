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
#include "funcionesEnvio.h"
#include "ProtocoloDeMensajes.h"

//Parametros a recibir desde el config
int ID_INICIAL;
pthread_mutex_t mut_listaProgramas; //mutex de la lista de programas actuales, podria ser uno por cola?
int listener_socket;

void* handler_clients(void* socket);
void init_broker_server();


#endif
