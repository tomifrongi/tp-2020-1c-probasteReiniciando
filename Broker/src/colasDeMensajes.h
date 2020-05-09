/*
 6 colas de mensajes y una lista de suscriptores por cada cola

 * */

#ifndef COLASDEMENSAJES_H_
#define COLASDEMENSAJES_H_

//INCLUDES
#include "ProtocoloDeMensajes.h"
#include <commons/collections/queue.h>

//ESTRUCTURAS

//-----------------------PAR DE MENSAJES 1-------
//no lleva idCorrelativo ya que es responsabilidad de team descartar los localized

typedef struct {
	uint32_t id;
	get_pokemon contenidoMensaje;
}get_pokemon_message;


typedef struct {
	uint32_t id;
	localized_pokemon contenidoMensaje;
}localized_pokemon_message;


t_queue* get_pokemon_queue;
t_queue* localized_pokemon_queue;


//-----------------------PAR DE MENSAJES 2-------
//Este par seguro que llevan id

typedef struct {
	uint32_t id;
	catch_pokemon contenidoMensaje;
}catch_pokemon_message;


typedef struct {
	uint32_t id;
	uint32_t idCorrelativo;
	caught_pokemon contenidoMensaje;
}caught_pokemon_message;


t_queue* catch_pokemon_queue;
t_queue* caught_pokemon_queue;


//-----------------------PAR DE MENSAJES 3-------
//no lleva idCorrelativo ya que es responsabilidad de team descartar los appeared

typedef struct {
	uint32_t id;
	new_pokemon contenidoMensaje;
}new_pokemon_message;


typedef struct {
	uint32_t id;
	appeared_pokemon contenidoMensaje;
}appeared_pokemon_message;


t_queue* new_pokemon_queue;
t_queue* appeared_pokemon_queue;


//FUNCIONES
uint32_t generarID();
void crearColas();
void destruirColas();

#endif /* COLASDEMENSAJES_H_ */
