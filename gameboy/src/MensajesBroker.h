
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MENSAJESBROKER_H_
#define MENSAJESBROKER_H_

typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidad;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}new_pokemon_broker;


typedef struct {
	uint32_t idCorrelativo;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}appeared_pokemon_broker;

typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}catch_pokemon_broker;

typedef struct {
	uint32_t idCorrelativo;
	uint32_t pokemonAtrapado; // 0 o 1 si el pokemon fue atrapado
}caught_pokemon_broker;

typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
}get_pokemon_broker;


void* serializarNewContentBroker (new_pokemon_broker parametros);
void* serializarAppearedContentBroker (appeared_pokemon_broker parametros);
void* serializarCatchContentBroker (catch_pokemon_broker parametros);
void* serializarCaughtContentBroker (caught_pokemon_broker parametros);
void* serializarGetContentBroker (get_pokemon_broker parametros);

#endif /* MENSAJESBROKER_H_ */
