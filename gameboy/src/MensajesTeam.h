
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MENSAJESTEAM_H_
#define MENSAJESTEAM_H_

typedef struct {
	uint32_t id_mensaje;
	uint32_t idCorrelativo;
	uint32_t sizeNombre;
	char* nombrePokemon;
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}appeared_pokemon_team;



void* serializarAppearedContentTeam(appeared_pokemon_team parametros);




#endif /* MENSAJESTEAM_H_ */
