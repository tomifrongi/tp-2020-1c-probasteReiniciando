/*
 6 colas de mensajes y una lista de suscriptores por cada cola

 * */



#ifndef COLASDEMENSAJES_H_
#define COLASDEMENSAJES_H_

//INCLUDES
#include "tiposMensajesEnMemoria.h"
#include <commons/collections/queue.h>
#include "broker.h"

//ESTRUCTURAS

//-----------------------PAR DE MENSAJES 1-------

//no lleva id ni idCorrelativo ya que es responsabilidad de team descartar los localized
//entonces no creo estructuras nuevas, con las que estan en tiposMensajesEnMemoria alcanza
//aun asi, los mensajes que se guardan en memoria tienen un id unico

/* por las dudas lo dejo comentado
typedef struct {
	uint32_t id;
	get_pokemon contenidoMensaje;
}get_pokemon_message;


typedef struct {
	uint32_t idCorrelativo;
	localized_pokemon contenidoMensaje;
}localized_pokemon_message;
*/

t_queue get_pokemon_queue;
t_queue localized_pokemon_queue;


//-----------------------PAR DE MENSAJES 2-------
//Este par seguro que llevan id
typedef struct {
	uint32_t id;
	catch_pokemon contenidoMensaje;
}catch_pokemon_message;


typedef struct {
	uint32_t idCorrelativo;
	caught_pokemon contenidoMensaje;
}caught_pokemon_message;


t_queue catch_pokemon_queue;
t_queue caught_pokemon_message;


//-----------------------PAR DE MENSAJES 3-------
//en este par de mensajes ocurre lo mismo que en el par 1

t_queue new_pokemon_queue;
t_queue appeared_pokemon_queue;


//FUNCIONES
uint32_t generarID();


#endif /* COLASDEMENSAJES_H_ */
