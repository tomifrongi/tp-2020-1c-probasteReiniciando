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

t_queue* get_pokemon_queue;
t_queue* localized_pokemon_queue;

//-----------------------PAR DE MENSAJES 2-------

t_queue* catch_pokemon_queue;
t_queue* caught_pokemon_queue;

//-----------------------PAR DE MENSAJES 3-------

t_queue* new_pokemon_queue;
t_queue* appeared_pokemon_queue;

//FUNCIONES
uint32_t generarID();
void crearColas();
void destruirColas();

#endif /* COLASDEMENSAJES_H_ */
