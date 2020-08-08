
#ifndef SRC_PROTOCOLODEMENSAJES_H_
#define SRC_PROTOCOLODEMENSAJES_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>

//MENSAJES-------------------------------------------------------------
//DESCRIPCION: Seria lo que va en content del t_message;

//MENSAJE: suscripcion----------------
//DESCRIPCION: informar al broker a cual cola se quiere suscrbir el modulo

typedef enum {
	NEWW, //con doble w porque ya hay un enum igual
	APPEARED,
	GET,
	LOCALIZED,
	CATCH,
	CAUGHT
}id_cola;

typedef struct {
	id_cola idCola;
	pid_t idSuscriptor;
}suscripcion;
//t_header = SUSCRIPCION

//MENSAJE: confirmacion del broker----
//DESCRIPCION: por cada mensaje que se envia al broker, el mismo te devuelve este tipo de mensaje

typedef struct {
	uint32_t id_mensaje;
}confirmacion;
//t_header = CONFIRMACION

//------------------------------------

//MENSAJE: ack para el broker---------
//DESCRIPCION: por cada mensaje que se recibe del broker, se debe enviar este mensaje

typedef struct {
	uint32_t id_mensaje;
	pid_t idSuscriptor;
}mensajeACK;
//t_header = CONFIRMACION

//------------------------------------

//MENSAJE: appeared_pokemon-----------

typedef struct {
	uint32_t id_mensaje;
	uint32_t idCorrelativo;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}appeared_pokemon;
//t_header = APPEARED_POKEMON

//------------------------------------

//MENSAJE: get_pokemon;---------------

typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
}get_pokemon;
//t_header = GET_POKEMON

//------------------------------------

//MENSAJE: localized_pokemon----------

//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t idCorrelativo;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidadPosiciones; //cantidad de posiciones y no la cantidad de pokemones
	void* posiciones;
}localized_pokemon_recibido;
//t_header = LOCALIZED_POKEMON

typedef struct {
	uint32_t id_mensaje;
	uint32_t idCorrelativo;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidadPosiciones; //cantidad de posiciones y no la cantidad de pokemones
	t_list* posiciones;
}localized_pokemon;
//t_header = LOCALIZED_POKEMON

typedef struct {
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}coordenada;

//------------------------------------
//---------------------------------------------------------------------
//MENSAJE: catch_pokemon--------------

typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}catch_pokemon;
//t_header = CATCH_POKEMON

//------------------------------------

//MENSAJE: caught_pokemon-------------

typedef struct {
	uint32_t id_mensaje;
	uint32_t idCorrelativo;
	uint32_t pokemonAtrapado;
}caught_pokemon;
//t_header = CAUGHT_POKEMON

//------------------------------------

//---------------------------------------------------------------------

//FUNCIONES------------------------------------------------------------

appeared_pokemon* deserializarAppeared(void* content); //tiene un malloc en nombrePokemon
caught_pokemon* deserializarCaught(void* content);
localized_pokemon* deserializarLocalized(void* content); //tiene un malloc en nombrePokemon, un list_create
														//y cada elemento de la lista tiene un malloc
confirmacion deserializarConfirmacion(void* content);


void* serializarSuscripcion(suscripcion mensaje);
void* serializarGet(get_pokemon mensaje); //devuelve un content que tiene un malloc
void* serializarCatch(catch_pokemon mensaje); //devuelve un content que tiene un malloc


coordenada* crear_coordenada(coordenada c);
void borrar_coordenada(coordenada* c);

void borrar_caught_pokemon(caught_pokemon* cp);

void borrar_appeared_pokemon(appeared_pokemon* ap);


void borrar_localized_pokemon(localized_pokemon* lp);

void* serializarACK(mensajeACK mensaje_confirmacion);
//---------------------------------------------------------------------
#endif /* SRC_PROTOCOLODEMENSAJES_H_ */
