/*
 6 colas de mensajes y una lista de suscriptores por cada cola

 * */



#ifndef COLASDEMENSAJES_H_
#define COLASDEMENSAJES_H_

//INCLUDES
#include "tiposMensajesEnMemoria.h"
#include <commons/collections/queue.h>

//ESTRUCTURAS

//-----------------------PAR DE MENSAJES 1
typedef struct {
	uint32_t id;
	get_pokemon contenidoMensaje;
}get_pokemon_message;


typedef struct {
	uint32_t id;
	uint32_t idCorrelativo;
	localized_pokemon contenidoMensaje;
}localized_pokemon_message;

t_queue get_pokemon_queue;
t_queue localized_pokemon_queue;











//FUNCIONES

#endif /* COLASDEMENSAJES_H_ */
