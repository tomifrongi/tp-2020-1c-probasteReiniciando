
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"
#include "Gameboy.h"

#ifndef MENSAJESSUSCRIPCION_H_
#define MENSAJESSUSCRIPCION_H_

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
	pid_t idSuscriptor;
}suscripcion;

//MENSAJES COLAS

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


void* serializarSuscripcionContent(suscripcion contenidoMensaje);
char* obtenerNombreCola(id_cola id);

void deserializarMensaje(t_message* mensajeRecibido);

void deserializarNew(void* content);
void deserializarAppeared(void* content);
void deserializarCatch(void* content);
void deserializarCaught(void* content);
void deserializarGet(void* content);
void deserializarLocalized(void* content);
#endif /* MENSAJESSUSCRIPCION_H_ */
