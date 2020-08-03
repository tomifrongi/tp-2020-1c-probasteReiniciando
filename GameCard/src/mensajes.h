#ifndef MENSAJES_H
#define MENSAJES_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ProtocoloDeMensajes.h"
//#include "protocol.h"

typedef struct {
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}t_envio_posicion;

typedef struct {
	id_cola idCola;
	pid_t idSuscriptor;
} t_suscripcion;

typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidad;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}new_pokemon_gamecard;

typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}catch_pokemon_gamecard;

typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
}get_pokemon_gamecard;

//////////////////////////////////////////////////////////////MENSAJES COLAS

typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidad;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}new_pokemon_suscripcion;

typedef struct {
	uint32_t id_mensaje;
	uint32_t idCorrelativo;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}appeared_pokemon_suscripcion;

typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
}get_pokemon_suscripcion;

typedef struct {
	uint32_t id_mensaje;
	uint32_t idCorrelativo;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidadPosiciones; //cantidad de posiciones y no la cantidad de pokemones
	void* posiciones;
}localized_pokemon_suscripcion;

typedef struct {
	uint32_t id_mensaje;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}catch_pokemon_suscripcion;

typedef struct {
	uint32_t id_mensaje;
	uint32_t idCorrelativo;
	uint32_t pokemonAtrapado;
}caught_pokemon_suscripcion;

typedef struct {
	uint32_t sizeNombre;
	uint32_t idCorrelativo;
	char* nombrePokemon;
	uint32_t cantidadPosiciones;
}localized_pokemon_suscipcion;

new_pokemon_suscripcion* deserializarNew(void* content);
catch_pokemon_suscripcion* deserializarCatch(void* content);
get_pokemon_suscripcion* deserializarGet(void* content);
void* serializarAppeared (appeared_pokemon* parametros);
void* serializarCaught(caught_pokemon* parametros);
void* serializarLocalized(localized_pokemon* parametros);

#endif
