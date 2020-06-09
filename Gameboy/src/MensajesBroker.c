/*
 * MensajesBroker.c
 *
 *  Created on: 23 may. 2020
 *      Author: utnso
 */
#include "MensajesBroker.h"

void* serializarNewContentBroker (new_pokemon_broker parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*4+parametros.sizeNombre);

	memcpy (content + bytes_escritos, &parametros.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, parametros.nombrePokemon, parametros.sizeNombre);
	bytes_escritos += parametros.sizeNombre;
	memcpy (content + bytes_escritos, &parametros.cantidad, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros.posicionEjeX, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros.posicionEjeY, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);


	return content;

}

void* serializarAppearedContentBroker (appeared_pokemon_broker parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*4+parametros.sizeNombre);

	memcpy (content + bytes_escritos, &parametros.idCorrelativo, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, parametros.nombrePokemon, parametros.sizeNombre);
	bytes_escritos += parametros.sizeNombre;
	memcpy (content + bytes_escritos, &parametros.posicionEjeX, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros.posicionEjeY, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;
}

void* serializarCatchContentBroker (catch_pokemon_broker parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*3+parametros.sizeNombre);

	memcpy (content + bytes_escritos, &parametros.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, parametros.nombrePokemon, parametros.sizeNombre);
	bytes_escritos += parametros.sizeNombre;
	memcpy (content + bytes_escritos, &parametros.posicionEjeX, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros.posicionEjeY, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;
}

void* serializarCaughtContentBroker (caught_pokemon_broker parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*2);

	memcpy (content + bytes_escritos, &parametros.idCorrelativo, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros.pokemonAtrapado, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;
}

void* serializarGetContentBroker (get_pokemon_broker parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)+parametros.sizeNombre);

	memcpy (content + bytes_escritos, &parametros.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, parametros.nombrePokemon, parametros.sizeNombre);
	bytes_escritos += parametros.sizeNombre;

	return content;
}
