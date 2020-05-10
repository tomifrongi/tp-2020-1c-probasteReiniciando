/*
 * ProtocoloDeMensajes.h
 *
 *  Created on: 7 may. 2020
 *      Author: utnso
 */

#ifndef PROTOCOLODEMENSAJES_H_
#define PROTOCOLODEMENSAJES_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "commons/collections/list.h"


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

//MENSAJE: confirmacion----------
typedef struct {
	uint32_t id_mensaje;
}id_mensaje;
//t_header = CONFIRMACION
//-------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------
//MENSAJE: new_pokemon

//Publisher envia:
typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidad;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}new_pokemon;
//t_header = NEW_POKEMON


//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidad;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}new_pokemon_enviar;
//t_header = NEW_POKEMON

//--------------------------------------------------------------------------------------------------------------------------------------
//MENSAJE: appeared_pokemon

//Publisher envia:
typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}appeared_pokemon;
//t_header = APPEARED_POKEMON


//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}appeared_pokemon_enviar;
//t_header = APPEARED_POKEMON

//--------------------------------------------------------------------------------------------------------------------------------------
//MENSAJE: get_pokemon;

//Publisher envia:
typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
}get_pokemon;
//t_header = GET_POKEMON


//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
}get_pokemon_enviar;
//t_header = GET_POKEMON


//--------------------------------------------------------------------------------------------------------------------------------------
//MENSAJE: localized_pokemon

//Publisher envia:
typedef struct {
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

//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidadPosiciones; //cantidad de posiciones y no la cantidad de pokemones
	t_list posiciones;
}localized_pokemon_enviar;
//t_header = LOCALIZED_POKEMON


//--------------------------------------------------------------------------------------------------------------------------------------
//MENSAJE: catch_pokemon
//Publisher envia:
typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}catch_pokemon;
//t_header = CATCH_POKEMON


//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}catch_pokemon_enviar;
//t_header = CATCH_POKEMON


//--------------------------------------------------------------------------------------------------------------------------------------
//MENSAJE: caught_pokemon

//Publisher envia:
typedef struct {
	uint32_t idCorrelativo;
	uint32_t pokemonAtrapado; // 0 o 1 si el pokemon fue atrapado
}caught_pokemon;
//t_header = CAUGHT_POKEMON


//Subscriber recibe:
typedef struct {
	uint32_t id_mensaje;
	uint32_t idCorrelativo;
	uint32_t pokemonAtrapado;
}caught_pokemon_enviar;
//t_header = CAUGHT_POKEMON


//--------------------------------------------------------------------------------------------------------------------------------------
#endif /* PROTOCOLODEMENSAJES_H_ */
