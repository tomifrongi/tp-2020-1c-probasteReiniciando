
#ifndef TIPOSMENSAJESENMEMORIA_H_
#define TIPOSMENSAJESENMEMORIA_H_

//INCLUDES

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include "ProtocoloDeMensajes.h"

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
	void* posiciones;
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


void* serializarMensajeNew(new_pokemon_memoria* mensaje);
void* serializarMensajeAppeared(appeared_pokemon_memoria* mensaje);
void* serializarMensajeGet(get_pokemon_memoria* mensaje);
void* serializarMensajeLocalized(localized_pokemon_memoria* mensaje);
void* serializarMensajeCatch(catch_pokemon_memoria* mensaje);
void* serializarMensajeCaught(caught_pokemon_memoria* mensaje);

uint32_t obtenerTamanioMensaje(void* mensaje,id_cola id);
void* serializarMensaje(void* mensaje,id_cola id);

#endif /* TIPOSMENSAJESENMEMORIA_H_ */
