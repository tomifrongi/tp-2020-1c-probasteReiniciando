/*
 6 colas de mensajes y una lista de suscriptores por cada cola

 * */

#ifndef COLASDEMENSAJES_H_
#define COLASDEMENSAJES_H_

//INCLUDES
#include "ProtocoloDeMensajes.h"
#include <commons/collections/queue.h>
//ESTRUCTURAS


typedef struct {
	t_list* queue; // agrego al final de la cola y saco del principio
	t_list* suscriptores;
}estructuraAdministrativa;

typedef struct {
	pid_t idSuscriptor;
	int socket;
}suscriptor;

estructuraAdministrativa* get_admin;
estructuraAdministrativa* localized_admin;
estructuraAdministrativa* catch_admin;
estructuraAdministrativa* caught_admin;
estructuraAdministrativa* new_admin;
estructuraAdministrativa* appeared_admin;

pthread_mutex_t* mutexQueueNew;
pthread_mutex_t* mutexQueueAppeared;
pthread_mutex_t* mutexQueueGet;
pthread_mutex_t* mutexQueueLocalized;
pthread_mutex_t* mutexQueueCatch;
pthread_mutex_t* mutexQueueCaught;

pthread_mutex_t* mutexId;



pthread_mutex_t* mutexMemoria;

pthread_mutex_t* mutexSuscriptoresNew;
pthread_mutex_t* mutexSuscriptoresAppeared;
pthread_mutex_t* mutexSuscriptoresGet;
pthread_mutex_t* mutexSuscriptoresLocalized;
pthread_mutex_t* mutexSuscriptoresCatch;
pthread_mutex_t* mutexSuscriptoresCaught;

pthread_mutex_t* mutexIDsCorrelativosAppeared;
pthread_mutex_t* mutexIDsCorrelativosLocalized;
pthread_mutex_t* mutexIDsCorrelativosCaught;

//------------


//FUNCIONES
void crearEstructurasAdministrativas ();
void destruirEstructurasAdministrativas();
suscriptor* crearSuscriptor(suscriptor suscriptorNuevo);
void borrarSuscriptor(suscriptor* suscriptor);
void eliminarIdCola(uint32_t idMensaje,id_cola idCola);

uint32_t* crearElementoCola(uint32_t elemento);
void borrarElementoCola(uint32_t* elemento);
void iniciarMutexs();

#endif /* COLASDEMENSAJES_H_ */
