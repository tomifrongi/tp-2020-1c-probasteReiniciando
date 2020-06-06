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
uint32_t ID_INICIAL;
pthread_mutex_t mutexQueueNew;
pthread_mutex_t mutexQueueAppeared;
pthread_mutex_t mutexQueueGet;
pthread_mutex_t mutexQueueLocalized;
pthread_mutex_t mutexQueueCatch;
pthread_mutex_t mutexQueueCaught;
pthread_mutex_t mutexId;
pthread_mutex_t mutexLogger;

t_list* idsCorrelativosAppeared;
t_list* idsCorrelativosLocalized;
t_list* idsCorrelativosCaught;

int listener_socket;


void* handler_clients(void* socket);
void init_broker_server();

void enviarConfirmacion(uint32_t id, int broker_sock);
void agregarSuscripcion(uint32_t id_cola,int broker_sock);

void iniciarMutexs();
void iniciarListasIds();
void* buscarIdCorrelativo(t_list* lista,uint32_t idCorrelativo);
#endif
