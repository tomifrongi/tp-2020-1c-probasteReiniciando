#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MENSAJESGAMECARD_H_
#define MENSAJESGAMECARD_H_

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


void* serializarNewContentGamecard(new_pokemon_gamecard parametros);
void* serializarCatchContentGamecard(catch_pokemon_gamecard parametros);
void* serializarGetContentGamecard(get_pokemon_gamecard parametros);


#endif /* MENSAJESGAMECARD_H_ */
