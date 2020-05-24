#include "MensajesGamecard.h"

void* serializarNewContentGamecard(new_pokemon_gamecard parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(new_pokemon_gamecard));

	memcpy (content + bytes_escritos, &parametros.id_mensaje, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
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

void* serializarCatchContentGamecard(catch_pokemon_gamecard parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(catch_pokemon_gamecard));

	memcpy (content + bytes_escritos, &parametros.id_mensaje, sizeof(uint32_t));
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

void* serializarGetContentGamecard(get_pokemon_gamecard parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(get_pokemon_gamecard));

	memcpy (content + bytes_escritos, &parametros.id_mensaje, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, parametros.nombrePokemon, parametros.sizeNombre);
	bytes_escritos += parametros.sizeNombre;

	return content;
}
