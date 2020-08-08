
#include "ProtocoloDeMensajes.h"

appeared_pokemon* deserializarAppeared(void* content){
	appeared_pokemon mensaje;
	int bytesLeidos = 0;


	memcpy(&mensaje.id_mensaje,content+bytesLeidos,sizeof(mensaje.id_mensaje));
	bytesLeidos+=(sizeof(mensaje.id_mensaje));
	memcpy(&mensaje.idCorrelativo,content+bytesLeidos,sizeof(mensaje.idCorrelativo));
	bytesLeidos+=(sizeof(mensaje.idCorrelativo));
	memcpy(&mensaje.sizeNombre,content+bytesLeidos,sizeof(mensaje.sizeNombre));
	bytesLeidos+=(sizeof(mensaje.sizeNombre));
	mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
	memcpy(mensaje.nombrePokemon,content+bytesLeidos,mensaje.sizeNombre);
	bytesLeidos+=mensaje.sizeNombre;
	memcpy(&mensaje.posicionEjeX,content+bytesLeidos,sizeof(mensaje.posicionEjeX));
	bytesLeidos+=(sizeof(mensaje.posicionEjeX));
	memcpy(&mensaje.posicionEjeY,content+bytesLeidos,sizeof(mensaje.posicionEjeY));

	appeared_pokemon* mensaje_creado = malloc(sizeof(appeared_pokemon));
	mensaje_creado->nombrePokemon = malloc(mensaje.sizeNombre);
	mensaje_creado->id_mensaje = mensaje.id_mensaje;
	mensaje_creado->idCorrelativo = mensaje.idCorrelativo;
	mensaje_creado->sizeNombre = mensaje.sizeNombre;
	strcpy(mensaje_creado->nombrePokemon,mensaje.nombrePokemon);
	mensaje_creado->posicionEjeX = mensaje.posicionEjeX;
	mensaje_creado->posicionEjeY = mensaje.posicionEjeY;
	free(mensaje.nombrePokemon);
	return mensaje_creado;
}

caught_pokemon* deserializarCaught(void* content){
	caught_pokemon mensaje;
	int bytesLeidos = 0;

	memcpy(&mensaje.id_mensaje,content+bytesLeidos,sizeof(mensaje.id_mensaje));
	bytesLeidos+=(sizeof(mensaje.id_mensaje));
	memcpy(&mensaje.idCorrelativo,content+bytesLeidos,sizeof(mensaje.idCorrelativo));
	bytesLeidos+=(sizeof(mensaje.idCorrelativo));
	memcpy(&mensaje.pokemonAtrapado,content+bytesLeidos,sizeof(mensaje.pokemonAtrapado));

	caught_pokemon* mensaje_creado = malloc(sizeof(caught_pokemon));
	mensaje_creado->id_mensaje = mensaje.id_mensaje;
	mensaje_creado->idCorrelativo = mensaje.idCorrelativo;
	mensaje_creado->pokemonAtrapado = mensaje.pokemonAtrapado;
	return mensaje_creado;
}

localized_pokemon* deserializarLocalized(void* content){
	localized_pokemon mensaje;
	int bytesLeidos = 0;

	memcpy(&mensaje.id_mensaje,content+bytesLeidos,sizeof(mensaje.id_mensaje));
	bytesLeidos+=(sizeof(mensaje.id_mensaje));
	memcpy(&mensaje.idCorrelativo,content+bytesLeidos,sizeof(mensaje.idCorrelativo));
	bytesLeidos+=(sizeof(mensaje.idCorrelativo));
	memcpy(&mensaje.sizeNombre,content+bytesLeidos,sizeof(mensaje.sizeNombre));
	bytesLeidos+=(sizeof(mensaje.sizeNombre));
	mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
	memcpy(mensaje.nombrePokemon,content+bytesLeidos,mensaje.sizeNombre);
	bytesLeidos+=mensaje.sizeNombre;
	memcpy(&mensaje.cantidadPosiciones,content+bytesLeidos,sizeof(mensaje.cantidadPosiciones));
	bytesLeidos+=(sizeof(mensaje.cantidadPosiciones));
	int longitudLista = mensaje.cantidadPosiciones *2;
	mensaje.posiciones = list_create();
	int index = 0;
	while(index<longitudLista){
		coordenada posicion;
		memcpy(&posicion.posicionEjeX,content+bytesLeidos,sizeof(uint32_t));
		bytesLeidos+=(sizeof(uint32_t));
		memcpy(&posicion.posicionEjeY,content+bytesLeidos,sizeof(uint32_t));
		bytesLeidos+=(sizeof(uint32_t));
		coordenada* posicionCreada = crear_coordenada(posicion);
		list_add(mensaje.posiciones,posicionCreada);
		index+=2;
	}

	localized_pokemon* mensaje_creado = malloc(sizeof(localized_pokemon));
	mensaje_creado->nombrePokemon = malloc (mensaje.sizeNombre);
	mensaje_creado->id_mensaje = mensaje.id_mensaje;
	mensaje_creado->idCorrelativo = mensaje.idCorrelativo;
	mensaje_creado->sizeNombre = mensaje.sizeNombre;
	strcpy(mensaje_creado->nombrePokemon,mensaje.nombrePokemon);
	mensaje_creado->cantidadPosiciones = mensaje.cantidadPosiciones;
	mensaje_creado->posiciones = mensaje.posiciones;

	free(mensaje.nombrePokemon);
	return mensaje_creado;

}

confirmacion deserializarConfirmacion(void* content){
	confirmacion mensaje;
	int bytesLeidos = 0;

	memcpy(&mensaje.id_mensaje,content+bytesLeidos,sizeof(mensaje.id_mensaje));
	bytesLeidos+=sizeof(uint32_t);

	return mensaje;
}


void* serializarSuscripcion(suscripcion mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(suscripcion));

	memcpy(content+bytes_escritos,&mensaje.idCola,sizeof(mensaje.idCola));
	bytes_escritos+=sizeof(mensaje.idCola);
	memcpy(content+bytes_escritos,&mensaje.idSuscriptor,sizeof(mensaje.idSuscriptor));
	bytes_escritos+=sizeof(mensaje.idSuscriptor);

	return content;
}

void* serializarGet(get_pokemon mensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)+mensaje.sizeNombre);

	memcpy (content + bytes_escritos, &mensaje.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, mensaje.nombrePokemon, mensaje.sizeNombre);
	bytes_escritos += mensaje.sizeNombre;

	return content;
}

void* serializarCatch(catch_pokemon mensaje){
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


coordenada* crear_coordenada(coordenada c){
	coordenada* c_creada  = malloc(sizeof(coordenada));
	c_creada->posicionEjeX = c.posicionEjeX;
	c_creada->posicionEjeY = c.posicionEjeY;
	return c_creada;
}

void borrar_coordenada(coordenada* c){
	free(c);
}

void borrar_caught_pokemon(caught_pokemon* cp){
	free(cp);
}

void borrar_appeared_pokemon(appeared_pokemon* ap){
	free(ap->nombrePokemon);
	free(ap);
}

void borrar_localized_pokemon(localized_pokemon* lp){
	free(lp->nombrePokemon);
	list_destroy_and_destroy_elements(lp->posiciones,(void*)borrar_coordenada);
	free(lp);
}

void* serializarACK(mensajeACK mensaje_confirmacion){
	void* content = malloc(sizeof(mensajeACK));
	int bytes_escritos = 0;

	memcpy (content + bytes_escritos, &mensaje_confirmacion.id_mensaje, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &mensaje_confirmacion.idSuscriptor, sizeof(mensaje_confirmacion.idSuscriptor));
	bytes_escritos +=  sizeof(mensaje_confirmacion.idSuscriptor);

	return content;

}
