/*
 6 colas de mensajes y una lista de suscriptores por cada cola

 * */

#ifndef COLASDEMENSAJES_H_
#define COLASDEMENSAJES_H_

//INCLUDES
#include "ProtocoloDeMensajes.h"
#include <commons/collections/queue.h>
#include "broker.h"
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

//------------


//FUNCIONES
void crearEstructurasAdministrativas ();
void destruirEstructurasAdministrativas();
suscriptor* crearSuscriptor(suscriptor suscriptorNuevo);
void borrarSuscriptor(suscriptor* suscriptor);
void eliminarIdCola(uint32_t idMensaje,id_cola idCola);

uint32_t* crearElementoCola(uint32_t elemento);
void borrarElementoCola(uint32_t* elemento);

#endif /* COLASDEMENSAJES_H_ */
