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
	id_cola idCola;
	uint32_t idSuscriptor;
}suscripcion;

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

#endif
