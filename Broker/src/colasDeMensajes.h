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
	t_queue* queue;
	t_list* suscriptores;
}estructuraAdministrativa;

estructuraAdministrativa get_admin;
estructuraAdministrativa localized_admin;
estructuraAdministrativa catch_admin;
estructuraAdministrativa caught_admin;
estructuraAdministrativa new_admin;
estructuraAdministrativa appeared_admin;

//------------


//FUNCIONES
void crearEstructurasAdministrativas ();
void destruirEstructurasAdministrativas();

#endif /* COLASDEMENSAJES_H_ */
