/*

Porque este nombre? Porque asi esta en el anexo 2

Y al que no le gusta que se joda

 */

#ifndef TIPOSMENSAJESENMEMORIA_H_
#define TIPOSMENSAJESENMEMORIA_H_

//INCLUDES

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>

//MENSAJES
typedef struct {
	uint32_t ejeX;
	uint32_t ejeY;
}coordenada;

//-------PAR 1-------------------
typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidad;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}new_pokemon_memoria;

typedef struct {

	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}appeared_pokemon_memoria;
//------------------------------


//-------PAR 2-------------------
typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
}get_pokemon_memoria;


typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t cantidadPosiciones; //cantidad de posiciones y no la cantidad de pokemones
	t_list posiciones;
}localized_pokemon_memoria; //RESPUESTA A get_pokemon
//------------------------------


//-------PAR 3-------------------
typedef struct {
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}catch_pokemon_memoria;


typedef struct {
	uint32_t pokemonAtrapado; // 0 o 1 si el pokemon fue atrapado
}caught_pokemon_memoria;
//------------------------------


void* serializarMensajeNew(new_pokemon_memoria mensaje);

#endif /* TIPOSMENSAJESENMEMORIA_H_ */
