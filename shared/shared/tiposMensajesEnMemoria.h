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

#include <commons/collections/list.h>





typedef struct {

	uint32_t ejeX;

	uint32_t ejeY;

}coordenada;



typedef struct {

	uint32_t sizeNombre;

	char* nombrePokemon; //no estoy seguro si se almacena asi el string

	uint32_t cantidad;

	coordenada posicion;



}new_pokemon_message;





typedef struct {

	uint32_t sizeNombre;

	char* nombrePokemon; //no estoy seguro si se almacena asi el string

	uint32_t cantidadPosiciones; //cantidad de posiciones y no la cantidad de pokemones

	t_list posiciones;





}localized_pokemon_message;





typedef struct {

	uint32_t sizeNombre;

	char* nombrePokemon;

}get_pokemon_message;





typedef struct {

	uint32_t sizeNombre;

	char* nombrePokemon;

	coordenada posicion;

}appeared_pokemon_message;





typedef struct {

	uint32_t sizeNombre;

	char* nombrePokemon;

	coordenada posicion;

}catch_pokemon_message;





typedef struct {

	uint32_t pokemonAtrapado; // 0 o 1 si el pokemon fue atrapado

}caught_pokemon_message;



#endif /* TIPOSMENSAJESENMEMORIA_H_ */
