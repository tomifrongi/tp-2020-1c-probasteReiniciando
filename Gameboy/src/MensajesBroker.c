/*
 * MensajesBroker.c
 *
 *  Created on: 23 may. 2020
 *      Author: utnso
 */
#include "MensajesBroker.h"

void* serializarNewContentBroker (new_pokemon_broker parametros){

		int bytes_escritos = 0;
		void* content = malloc(sizeof(new_pokemon_broker));
		uint32_t longitudNombre = strlen(parametros.nombrePokemon)+1;
		memcpy (content + bytes_escritos, &longitudNombre, sizeof(uint32_t));
		bytes_escritos += sizeof (uint32_t);
		memcpy (content + bytes_escritos, &parametros.nombrePokemon, longitudNombre);
		bytes_escritos += longitudNombre;
		memcpy (content + bytes_escritos, &parametros.posicionEjeX, sizeof(uint32_t));
		bytes_escritos += sizeof (uint32_t);
		memcpy (content + bytes_escritos, &parametros.posicionEjeY, sizeof(uint32_t));
		bytes_escritos += sizeof (uint32_t);
		memcpy (content + bytes_escritos, &parametros.cantidad, sizeof(uint32_t));
		bytes_escritos += sizeof (uint32_t);
		return content;

	return 0;
}

//TODO
void* serializarAppearedContentBroker (appeared_pokemon_broker parametros){
	void* content;
	return content;
}

//TODO
void* serializarCatchContentBroker (catch_pokemon_broker parametros){
	void* content;
	return content;
}

//TODO
void* serializarCaughtContentBroker (caught_pokemon_broker parametros){
	void* content;
	return content;
}

//TODO
void* serializarGetContentBroker (get_pokemon_broker parametros){
	void* content;
	return content;
}
