#include "tiposMensajesEnMemoria.h"


void* serializarMensajeNew(new_pokemon_memoria mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*4+mensaje.sizeNombre);

	memcpy (content + bytes_escritos, &mensaje.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, mensaje.nombrePokemon, mensaje.sizeNombre);
	bytes_escritos += mensaje.sizeNombre;
	memcpy (content + bytes_escritos, &mensaje.cantidad, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &mensaje.posicionEjeX, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &mensaje.posicionEjeY, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;
}

void* serializarMensajeAppeared(appeared_pokemon_memoria mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*3+mensaje.sizeNombre);

	memcpy (content + bytes_escritos, &mensaje.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, mensaje.nombrePokemon, mensaje.sizeNombre);
	bytes_escritos += mensaje.sizeNombre;
	memcpy (content + bytes_escritos, &mensaje.posicionEjeX, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &mensaje.posicionEjeY, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;

}

void* serializarMensajeGet(get_pokemon_memoria mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)+mensaje.sizeNombre);

	memcpy (content + bytes_escritos, &mensaje.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, mensaje.nombrePokemon, mensaje.sizeNombre);
	bytes_escritos += mensaje.sizeNombre;

	return content;

}

void* serializarMensajeCatch(catch_pokemon_memoria mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*3+mensaje.sizeNombre);

	memcpy (content + bytes_escritos, &mensaje.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, mensaje.nombrePokemon, mensaje.sizeNombre);
	bytes_escritos += mensaje.sizeNombre;
	memcpy (content + bytes_escritos, &mensaje.posicionEjeX, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &mensaje.posicionEjeY, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;
}

typedef struct {
	uint32_t pokemonAtrapado; // 0 o 1 si el pokemon fue atrapado
}caught_pokemon_memoria;

void* serializarMensajeCaught(caught_pokemon_memoria mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t));

	memcpy(content+bytes_escritos,&mensaje.pokemonAtrapado,sizeof(uint32_t));

	return content;
}
