#include "tiposMensajesEnMemoria.h"


void* serializarMensajeNew(new_pokemon_memoria* mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*4+mensaje->sizeNombre);

	memcpy (content + bytes_escritos, &mensaje->sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, mensaje->nombrePokemon, mensaje->sizeNombre);
	bytes_escritos += mensaje->sizeNombre;
	memcpy (content + bytes_escritos, &mensaje->cantidad, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &mensaje->posicionEjeX, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &mensaje->posicionEjeY, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;
}

void* serializarMensajeAppeared(appeared_pokemon_memoria* mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*3+mensaje->sizeNombre);

	memcpy (content + bytes_escritos, &mensaje->sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, mensaje->nombrePokemon, mensaje->sizeNombre);
	bytes_escritos += mensaje->sizeNombre;
	memcpy (content + bytes_escritos, &mensaje->posicionEjeX, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &mensaje->posicionEjeY, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;

}

void* serializarMensajeGet(get_pokemon_memoria* mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)+mensaje->sizeNombre);

	memcpy (content + bytes_escritos, &mensaje->sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, mensaje->nombrePokemon, mensaje->sizeNombre);
	bytes_escritos += mensaje->sizeNombre;

	return content;

}

void* serializarMensajeLocalized(localized_pokemon_memoria* mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t) *2 + mensaje->sizeNombre + sizeof(uint32_t)*(mensaje->cantidadPosiciones)*2);

	memcpy (content + bytes_escritos, &mensaje->sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, mensaje->nombrePokemon, mensaje->sizeNombre);
	bytes_escritos += mensaje->sizeNombre;
	memcpy (content + bytes_escritos, &mensaje->cantidadPosiciones, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, mensaje->posiciones, sizeof(uint32_t)*(mensaje->cantidadPosiciones)*2);
	bytes_escritos += sizeof(uint32_t)*(mensaje->cantidadPosiciones)*2;

	return content;
}

void* serializarMensajeCatch(catch_pokemon_memoria* mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*3+mensaje->sizeNombre);

	memcpy (content + bytes_escritos, &mensaje->sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, mensaje->nombrePokemon, mensaje->sizeNombre);
	bytes_escritos += mensaje->sizeNombre;
	memcpy (content + bytes_escritos, &mensaje->posicionEjeX, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &mensaje->posicionEjeY, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;
}

void* serializarMensajeCaught(caught_pokemon_memoria* mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t));

	memcpy(content+bytes_escritos,&mensaje->pokemonAtrapado,sizeof(uint32_t));

	return content;
}

uint32_t obtenerTamanioMensaje(void* mensaje,id_cola id){
	uint32_t tamanio = 0;

	switch(id){
	case NEW: {
		new_pokemon_memoria* np = mensaje;
		tamanio = sizeof(uint32_t)*4 + np->sizeNombre;
		break;
	}
	case APPEARED: {
		appeared_pokemon_memoria* ap = mensaje;
		tamanio = sizeof(uint32_t)*3 + ap->sizeNombre;
		break;
	}
	case GET: {
		get_pokemon_memoria* gp = mensaje;
		tamanio = sizeof(uint32_t) + gp->sizeNombre;
		break;
	}
	case LOCALIZED: {
		localized_pokemon_memoria* lp = mensaje;
		tamanio = sizeof(uint32_t) *2 + lp->sizeNombre + sizeof(uint32_t)*(lp->cantidadPosiciones)*2;
		break;
	}
	case CATCH:{
		catch_pokemon_memoria* catchp = mensaje;
		tamanio = sizeof(uint32_t)*3 + catchp->sizeNombre;
		break;
	}
	case CAUGHT:{
		tamanio = sizeof(uint32_t);
		break;
	}
	}
	return tamanio;
}

void* serializarMensaje(void* mensaje,id_cola id){
	switch(id){
	case NEW: {
		return serializarMensajeNew(mensaje);
		break;
	}
	case APPEARED: {
		return serializarMensajeAppeared(mensaje);
		break;
	}
	case GET: {
		return serializarMensajeGet(mensaje);
		break;
	}
	case LOCALIZED: {
		return serializarMensajeLocalized(mensaje);
		break;
	}
	case CATCH:{
		return serializarMensajeCatch(mensaje);
		break;
	}
	case CAUGHT:{
		return serializarMensajeCaught(mensaje);
		break;
	}
	}
	return NULL;
}
