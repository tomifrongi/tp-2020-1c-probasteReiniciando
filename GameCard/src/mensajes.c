#include "mensajes.h"

//Serializar

void* serializarNew(new_pokemon_gamecard parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*5+parametros.sizeNombre);

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

void* serializarCatch(catch_pokemon_gamecard parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*4+parametros.sizeNombre);

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

void* serializarGet(get_pokemon_gamecard parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*2+parametros.sizeNombre);

	memcpy (content + bytes_escritos, &parametros.id_mensaje, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, parametros.nombrePokemon, parametros.sizeNombre);
	bytes_escritos += parametros.sizeNombre;

	return content;
}

void* serializarAppeared (appeared_pokemon_suscripcion parametros){
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

void* serializarCatch(catch_pokemon_gamecard parametros){
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

void* serializarCaught(caught_pokemon_suscripcion parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*2);

	memcpy (content + bytes_escritos, &parametros.idCorrelativo, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros.pokemonAtrapado, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;
}

void* serializarGet(get_pokemon_gamecard parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)+parametros.sizeNombre);

	memcpy (content + bytes_escritos, &parametros.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, parametros.nombrePokemon, parametros.sizeNombre);
	bytes_escritos += parametros.sizeNombre;

	return content;
}

void* serializarMensaje (suscripcion contenidoMensaje){
	int bytes_escritos = 0;
		void* content = malloc(sizeof(suscripcion));

		memcpy (content + bytes_escritos, &contenidoMensaje.idCola, sizeof(id_cola));
		bytes_escritos += sizeof (uint32_t);
		memcpy(content+bytes_escritos,&contenidoMensaje.idSuscriptor,sizeof(pid_t));

		return content;
}
//Deserializar

void deserializarNew(void* content){
	new_pokemon_suscripcion mensaje;
	int bytesLeidos = 0;

	memcpy(&mensaje.id_mensaje,content+bytesLeidos,sizeof(mensaje.id_mensaje));
	bytesLeidos+=(sizeof(mensaje.id_mensaje));
	memcpy(&mensaje.sizeNombre,content+bytesLeidos,sizeof(mensaje.sizeNombre));
	bytesLeidos+=(sizeof(mensaje.sizeNombre));
	mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
	memcpy(mensaje.nombrePokemon,content+bytesLeidos,mensaje.sizeNombre);
	bytesLeidos+=mensaje.sizeNombre;
	memcpy(&mensaje.cantidad,content+bytesLeidos,sizeof(mensaje.cantidad));
	bytesLeidos+=(sizeof(mensaje.cantidad));
	memcpy(&mensaje.posicionEjeX,content+bytesLeidos,sizeof(mensaje.posicionEjeX));
	bytesLeidos+=(sizeof(mensaje.posicionEjeX));
	memcpy(&mensaje.posicionEjeY,content+bytesLeidos,sizeof(mensaje.posicionEjeY));

	log_info(logger,"ID MENSAJE: %d",mensaje.id_mensaje);
	log_info(logger,"NOMBRE POKEMON: %s",mensaje.nombrePokemon);
	log_info(logger,"CANTIDAD: %d",mensaje.cantidad);
	log_info(logger,"POSICION EJE X: %d",mensaje.posicionEjeX);
	log_info(logger,"POSICION EJE Y: %d",mensaje.posicionEjeY);
	free(mensaje.nombrePokemon);
}

void deserializarAppeared(void* content){
	appeared_pokemon_suscripcion mensaje;
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

	log_info(logger,"ID MENSAJE: %d",mensaje.id_mensaje);
	log_info(logger,"ID CORRELATIVO: %d",mensaje.idCorrelativo);
	log_info(logger,"NOMBRE POKEMON: %s",mensaje.nombrePokemon);
	log_info(logger,"POSICION EJE X: %d",mensaje.posicionEjeX);
	log_info(logger,"POSICION EJE Y: %d",mensaje.posicionEjeY);
	free(mensaje.nombrePokemon);
}

void deserializarCatch(void* content){
	catch_pokemon_suscripcion mensaje;
	int bytesLeidos = 0;

	memcpy(&mensaje.id_mensaje,content+bytesLeidos,sizeof(mensaje.id_mensaje));
	bytesLeidos+=(sizeof(mensaje.id_mensaje));
	memcpy(&mensaje.sizeNombre,content+bytesLeidos,sizeof(mensaje.sizeNombre));
	bytesLeidos+=(sizeof(mensaje.sizeNombre));
	mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
	memcpy(mensaje.nombrePokemon,content+bytesLeidos,mensaje.sizeNombre);
	bytesLeidos+=mensaje.sizeNombre;
	memcpy(&mensaje.posicionEjeX,content+bytesLeidos,sizeof(mensaje.posicionEjeX));
	bytesLeidos+=(sizeof(mensaje.posicionEjeX));
	memcpy(&mensaje.posicionEjeY,content+bytesLeidos,sizeof(mensaje.posicionEjeY));

	log_info(logger,"ID MENSAJE: %d",mensaje.id_mensaje);
	log_info(logger,"NOMBRE POKEMON: %s",mensaje.nombrePokemon);
	log_info(logger,"POSICION EJE X: %d",mensaje.posicionEjeX);
	log_info(logger,"POSICION EJE Y: %d",mensaje.posicionEjeY);
	free(mensaje.nombrePokemon);
}

void deserializarCaught(void* content){
	caught_pokemon_suscripcion mensaje;
	int bytesLeidos = 0;

	memcpy(&mensaje.id_mensaje,content+bytesLeidos,sizeof(mensaje.id_mensaje));
	bytesLeidos+=(sizeof(mensaje.id_mensaje));
	memcpy(&mensaje.idCorrelativo,content+bytesLeidos,sizeof(mensaje.idCorrelativo));
	bytesLeidos+=(sizeof(mensaje.idCorrelativo));
	memcpy(&mensaje.pokemonAtrapado,content+bytesLeidos,sizeof(mensaje.pokemonAtrapado));

	log_info(logger,"ID MENSAJE: %d",mensaje.id_mensaje);
	log_info(logger,"ID CORRELATIVO: %d",mensaje.idCorrelativo);
	if(mensaje.pokemonAtrapado)
		log_info(logger,"POKEMON FUE ATRAPADO");
	else
		log_info(logger,"POKEMON NOOO FUE ATRAPADO");
}

void deserializarGet(void* content){
	get_pokemon_suscripcion mensaje;
	int bytesLeidos = 0;

	memcpy(&mensaje.id_mensaje,content+bytesLeidos,sizeof(mensaje.id_mensaje));
	bytesLeidos+=(sizeof(mensaje.id_mensaje));
	memcpy(&mensaje.sizeNombre,content+bytesLeidos,sizeof(mensaje.sizeNombre));
	bytesLeidos+=(sizeof(mensaje.sizeNombre));
	mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
	memcpy(mensaje.nombrePokemon,content+bytesLeidos,mensaje.sizeNombre);

	log_info(logger,"ID MENSAJE: %d",mensaje.id_mensaje);
	log_info(logger,"NOMBRE POKEMON: %s",mensaje.nombrePokemon);
	free(mensaje.nombrePokemon);
}

void deserializarLocalized(void* content){
	localized_pokemon_suscripcion mensaje;
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
	uint32_t posiciones[longitudLista];
	int index = 0;
	while(index<longitudLista){
		memcpy(&posiciones[index],content+bytesLeidos,sizeof(uint32_t));
		bytesLeidos+=(sizeof(uint32_t));
	}

	log_info(logger,"ID MENSAJE: %d",mensaje.id_mensaje);
	log_info(logger,"ID CORRELATIVO: %d",mensaje.idCorrelativo);
	log_info(logger,"NOMBRE POKEMON: %s",mensaje.nombrePokemon);
	index = 0;
	while(index<longitudLista){
		log_info(logger,"POSICION: (%d,%d)",posiciones[index],posiciones[index+1]);
		index+=2;
	}
	free(mensaje.nombrePokemon);
}


