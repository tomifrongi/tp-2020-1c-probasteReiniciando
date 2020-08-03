#include "mensajes.h"
void* serializarMensaje (t_suscripcion contenidoMensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(t_suscripcion));

	memcpy (content + bytes_escritos, &contenidoMensaje.idCola, sizeof(id_cola));
	bytes_escritos += sizeof (uint32_t);
	memcpy(content+bytes_escritos,&contenidoMensaje.idSuscriptor,sizeof(pid_t));

	return content;
}

void* serializarAppeared (appeared_pokemon* parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*4+parametros->sizeNombre);

	memcpy (content + bytes_escritos, &parametros->idCorrelativo, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros->sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, parametros->nombrePokemon, parametros->sizeNombre);
	bytes_escritos += parametros->sizeNombre;
	memcpy (content + bytes_escritos, &parametros->posicionEjeX, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros->posicionEjeY, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;
}

void* serializarCaught(caught_pokemon* parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*2);

	memcpy (content + bytes_escritos, &parametros->idCorrelativo, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros->pokemonAtrapado, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;
}

void* serializarLocalized(localized_pokemon* parametros)
{
	int bytes_escritos = 0;
	uint32_t cantidadLista = parametros->cantidadPosiciones;

	void* content = malloc(sizeof(uint32_t)*4 + parametros->sizeNombre + 2*sizeof(uint32_t)*cantidadLista);

	memcpy (content + bytes_escritos, &parametros->idCorrelativo, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros->sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, parametros->nombrePokemon, parametros->sizeNombre);
	bytes_escritos += parametros->sizeNombre;
	memcpy (content + bytes_escritos, &parametros->cantidadPosiciones, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	for (int i = 0; i < parametros->cantidadPosiciones; i++) {
		t_envio_posicion* pos = list_get(parametros->posiciones, i);
		memcpy (content + bytes_escritos, &pos->posicionEjeX, sizeof(uint32_t));
		bytes_escritos += sizeof (uint32_t);
		memcpy (content + bytes_escritos, &pos->posicionEjeY, sizeof(uint32_t));
		bytes_escritos += sizeof (uint32_t);
	}

	return content;
}


new_pokemon_suscripcion* deserializarNew(void* content){
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

	new_pokemon_suscripcion* mensaje_creado = malloc(sizeof(new_pokemon_suscripcion));
	mensaje_creado->nombrePokemon = malloc(mensaje.sizeNombre);
	mensaje_creado->id_mensaje = mensaje.id_mensaje;
	mensaje_creado->sizeNombre = mensaje.sizeNombre;
	strcpy(mensaje_creado->nombrePokemon,mensaje.nombrePokemon);
	mensaje_creado->cantidad = mensaje.cantidad;
	mensaje_creado->posicionEjeX = mensaje.posicionEjeX;
	mensaje_creado->posicionEjeY = mensaje.posicionEjeY;

	log_info(logger,"ID MENSAJE: %d",mensaje.id_mensaje);
	log_info(logger,"NOMBRE POKEMON: %s",mensaje.nombrePokemon);
	log_info(logger,"CANTIDAD: %d",mensaje.cantidad);
	log_info(logger,"POSICION EJE X: %d",mensaje.posicionEjeX);
	log_info(logger,"POSICION EJE Y: %d",mensaje.posicionEjeY);

	free(mensaje.nombrePokemon);
	return mensaje_creado;
}

catch_pokemon_suscripcion* deserializarCatch(void* content){
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

	catch_pokemon_suscripcion* mensaje_creado = malloc(sizeof(catch_pokemon_suscripcion));
	mensaje_creado->nombrePokemon = malloc(mensaje.sizeNombre);
	mensaje_creado->id_mensaje = mensaje.id_mensaje;
	mensaje_creado->sizeNombre = mensaje.sizeNombre;
	strcpy(mensaje_creado->nombrePokemon,mensaje.nombrePokemon);
	mensaje_creado->posicionEjeX = mensaje.posicionEjeX;
	mensaje_creado->posicionEjeY = mensaje.posicionEjeY;

	log_info(logger,"ID MENSAJE: %d",mensaje.id_mensaje);
	log_info(logger,"NOMBRE POKEMON: %s",mensaje.nombrePokemon);
	log_info(logger,"POSICION EJE X: %d",mensaje.posicionEjeX);
	log_info(logger,"POSICION EJE Y: %d",mensaje.posicionEjeY);

	free(mensaje.nombrePokemon);
	return mensaje_creado;

}

get_pokemon_suscripcion* deserializarGet(void* content){
	get_pokemon_suscripcion mensaje;
	int bytesLeidos = 0;

	memcpy(&mensaje.id_mensaje,content+bytesLeidos,sizeof(mensaje.id_mensaje));
	bytesLeidos+=(sizeof(mensaje.id_mensaje));
	memcpy(&mensaje.sizeNombre,content+bytesLeidos,sizeof(mensaje.sizeNombre));
	bytesLeidos+=(sizeof(mensaje.sizeNombre));
	mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
	memcpy(mensaje.nombrePokemon,content+bytesLeidos,mensaje.sizeNombre);

	get_pokemon_suscripcion* mensaje_creado = malloc(sizeof(get_pokemon_suscripcion));
	mensaje_creado->nombrePokemon = malloc(mensaje.sizeNombre);
	mensaje_creado->id_mensaje = mensaje.id_mensaje;
	mensaje_creado->sizeNombre = mensaje.sizeNombre;
	strcpy(mensaje_creado->nombrePokemon,mensaje.nombrePokemon);

	log_info(logger,"ID MENSAJE: %d",mensaje.id_mensaje);
	log_info(logger,"NOMBRE POKEMON: %s",mensaje.nombrePokemon);
	free(mensaje.nombrePokemon);
	return mensaje_creado;

}



