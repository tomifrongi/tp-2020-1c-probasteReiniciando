/*
 * ProtocoloDeMensajes.h

ALGUNAS ACLARACIONES
1. Notificación de recepción: Toodo mensaje entregado debe ser confirmado
por cada Suscriptor para marcarlo y no enviarse nuevamente al mismo.
Esto explica porque cada vez que recibe un mensaje el suscriptor debe responder
con el mensaje "id_mensaje"

2. Toodo mensaje debe tener un identificador unívoco generado por el Broker
que debe ser informado al módulo que generó el mismo.
Esto explica porque el publisher despues de enviar un mensaje a una cola debe
esperar al mensaje "id_mensaje"

3. Respetar el orden de las estructuras, de lo contrario se deserializara mal
 */

#ifndef PROTOCOLODEMENSAJES_H_
#define PROTOCOLODEMENSAJES_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "commons/collections/list.h"

//HEADERS--------------------------------------------------------------

typedef enum{
	NEW_POKEMON,
	APPEARED_POKEMON,
	CATCH_POKEMON,
	CAUGHT_POKEMON,
	GET_POKEMON,
	LOCALIZED_POKEMON,
	CONFIRMACION,
	SUSCRIPCION,
	ERROR,
	NO_CONNECTION = 100,
	ERROR_RECV = 101,
	HI_PLEASE_BE_MY_FRIEND = 102,
	ERROR_MESSAGE = 103
}t_header;

//---------------------------------------------------------------------

//ESTRUCTURA MENSAJE---------------------------------------------------

typedef struct{
	t_header head;
	size_t size; //indica el tamaño del content
	void* content;
}t_message;
//---------------------------------------------------------------------

//MENSAJES-------------------------------------------------------------
//DESCRIPCION: Seria lo que va en content del t_message;

//MENSAJE: suscripcion----------------
//DESCRIPCION: informar al broker a cual cola se quiere suscrbir el modulo

typedef enum {
	NEW,
	APPEARED,
	GET,
	LOCALIZED,
	CATCH,
	CAUGHT
}id_cola;

typedef struct {
	id_cola idCola;
} suscripcion;
//t_header = SUSCRIPCION


//MENSAJE: new_pokemon----------------

//Publisher envia:
typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidad;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}new_pokemon;
//t_header = NEW_POKEMON

//Publisher recibe:
typedef struct {
	uint32_t id_mensaje;
}id_mensaje;
//t_header = CONFIRMACION


//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidad;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}new_pokemon;
//t_header = NEW_POKEMON

//Subscriber envia
typedef struct {
	uint32_t id_mensaje; //Devuelve el mismo id que recibe en new_pokemon para confirmar
						//que recibio el mensaje
}id_mensaje;
//t_header = CONFIRMACION

//------------------------------------

//MENSAJE: appeared_pokemon-----------

//Publisher envia:
typedef struct {
	uint32_t idCorrelativo;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}appeared_pokemon;
//t_header = APPEARED_POKEMON

//Publisher recibe:
typedef struct {
	uint32_t id_mensaje;
}id_mensaje;
//t_header = CONFIRMACION


//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t idCorrelativo;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}appeared_pokemon;
//t_header = APPEARED_POKEMON

//Subscriber envia
typedef struct {
	uint32_t id_mensaje; //Devuelve el mismo id que recibe en appeared_pokemon para confirmar
						//que recibio el mensaje
}id_mensaje;
//t_header = CONFIRMACION

//------------------------------------

//MENSAJE: get_pokemon;---------------

//Publisher envia:
typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
}get_pokemon;
//t_header = GET_POKEMON

//Publisher recibe:
typedef struct {
	uint32_t id_mensaje;
}id_mensaje;
//t_header = CONFIRMACION


//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
}get_pokemon;
//t_header = GET_POKEMON

//Subscriber envia:
typedef struct {
	uint32_t id_mensaje; //Devuelve el mismo id que recibe en get_pokemon para confirmar
						//que recibio el mensaje
}id_mensaje;
//t_header = CONFIRMACION

//------------------------------------

//MENSAJE: localized_pokemon----------

//Publisher envia:
typedef struct {
	uint32_t idCorrelativo;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidadPosiciones; //cantidad de posiciones y no la cantidad de pokemones.
								//Este dato sirve para poder desserialzar el mensaje
	t_list posiciones; // lista de uint32_t, la longitud es siempre par ya que
					   // es una lista de numeros y no una lista de duplas(estructura dupla).
					   // supongo que si es una lista de numeros es mas facil serializar
					  // ejemplo: [5,2,4,8,4,9] posiciones: (5,2);(4,8);(4,9)
}localized_pokemon;
//t_header = LOCALIZED_POKEMON

//Publisher recibe:
typedef struct {
	uint32_t id_mensaje;
}id_mensaje;
//t_header = CONFIRMACION


//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t idCorrelativo;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidadPosiciones; //cantidad de posiciones y no la cantidad de pokemones
	t_list posiciones;
}localized_pokemon;
//t_header = LOCALIZED_POKEMON

//Subscriber envia:
typedef struct {
	uint32_t id_mensaje;
}id_mensaje;
//t_header = CONFIRMACION

//------------------------------------

//MENSAJE: catch_pokemon--------------

//Publisher envia:
typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}catch_pokemon;
//t_header = CATCH_POKEMON

//Publisher recibe:
typedef struct {
	uint32_t id_mensaje;
}id_mensaje;
//t_header = CONFIRMACION


//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}catch_pokemon;
//t_header = CATCH_POKEMON

//Subscriber envia:
typedef struct {
	uint32_t id_mensaje; //Devuelve el mismo id que recibe en catch_pokemon para confirmar
						//que recibio el mensaje
}id_mensaje;
//t_header = CONFIRMACION

//------------------------------------

//MENSAJE: caught_pokemon-------------

//Publisher envia:
typedef struct {
	uint32_t idCorrelativo;
	uint32_t pokemonAtrapado; // 0 o 1 si el pokemon fue atrapado
}caught_pokemon;
//t_header = CAUGHT_POKEMON

//Publisher recibe:
typedef struct {
	uint32_t id_mensaje;
}id_mensaje;
//t_header = CONFIRMACION


//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t idCorrelativo;
	uint32_t pokemonAtrapado;
}caught_pokemon;
//t_header = CAUGHT_POKEMON

//Subscriber envia
typedef struct {
	uint32_t id_mensaje; //Devuelve el mismo id que recibe en caught_pokemon para confirmar
						//que recibio el mensaje
}id_mensaje;
//t_header = CONFIRMACION

//------------------------------------

//---------------------------------------------------------------------
#endif /* PROTOCOLODEMENSAJES_H_ */
