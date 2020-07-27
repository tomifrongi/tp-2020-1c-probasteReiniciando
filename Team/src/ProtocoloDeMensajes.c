
#include "ProtocoloDeMensajes.h"

appeared_pokemon deserializarAppeared(void* content){
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

	return mensaje;
}

caught_pokemon deserializarCaught(void* content){
	caught_pokemon mensaje;
	int bytesLeidos = 0;

	memcpy(&mensaje.id_mensaje,content+bytesLeidos,sizeof(mensaje.id_mensaje));
	bytesLeidos+=(sizeof(mensaje.id_mensaje));
	memcpy(&mensaje.idCorrelativo,content+bytesLeidos,sizeof(mensaje.idCorrelativo));
	bytesLeidos+=(sizeof(mensaje.idCorrelativo));
	memcpy(&mensaje.pokemonAtrapado,content+bytesLeidos,sizeof(mensaje.pokemonAtrapado));

	return mensaje;
}

localized_pokemon deserializarLocalized(void* content){
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

	return mensaje;
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


