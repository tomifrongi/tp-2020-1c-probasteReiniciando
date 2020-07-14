
#include "MensajesSuscripcion.h"


void* serializarSuscripcionContent(suscripcion contenidoMensaje){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(suscripcion));

	memcpy (content + bytes_escritos, &contenidoMensaje.idCola, sizeof(id_cola));
	bytes_escritos += sizeof (uint32_t);
	memcpy(content+bytes_escritos,&contenidoMensaje.idSuscriptor,sizeof(pid_t));

	return content;
}

char* obtenerNombreCola(id_cola id){

	switch(id){
	case NEW:
		return "NEW";
		break;
	case APPEARED:
		return "APPEARED";
		break;
	case GET:
		return "GET";
		break;
	case LOCALIZED:
		return "LOCALIZED";
		break;
	case CATCH:
		return "CATCH";
		break;
	case CAUGHT:
		return "CAUGHT";
		break;
	}
return "error";
}

void deserializarMensaje(t_message* mensajeRecibido){
	switch(mensajeRecibido->head){
	case NEW_POKEMON:{
		deserializarNew(mensajeRecibido->content);
		break;
	}
	case APPEARED_POKEMON:{
		deserializarAppeared(mensajeRecibido->content);
		break;
	}
	case CATCH_POKEMON:{
		deserializarCatch(mensajeRecibido->content);
		break;
	}
	case CAUGHT_POKEMON:{
		deserializarCaught(mensajeRecibido->content);
		break;
	}
	case GET_POKEMON:{
		deserializarGet(mensajeRecibido->content);
		break;
	}
	case LOCALIZED_POKEMON:{
		deserializarLocalized(mensajeRecibido->content);
		break;
	}
	default:{
		break;
	}
	}
}

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
}

void deserializarCatch(void* content){
	catch_pokemon_suscripcion mensaje;
	int bytesLeidos = 0;

	memcpy(&mensaje.id_mensaje,content+bytesLeidos,sizeof(mensaje.id_mensaje));
	bytesLeidos+=(sizeof(mensaje.id_mensaje));
	memcpy(&mensaje.sizeNombre,content+bytesLeidos,sizeof(mensaje.sizeNombre));
	bytesLeidos+=(sizeof(mensaje.sizeNombre));
	memcpy(mensaje.nombrePokemon,content+bytesLeidos,mensaje.sizeNombre);
	bytesLeidos+=mensaje.sizeNombre;
	memcpy(&mensaje.posicionEjeX,content+bytesLeidos,sizeof(mensaje.posicionEjeX));
	bytesLeidos+=(sizeof(mensaje.posicionEjeX));
	memcpy(&mensaje.posicionEjeY,content+bytesLeidos,sizeof(mensaje.posicionEjeY));

	log_info(logger,"ID MENSAJE: %d",mensaje.id_mensaje);
	log_info(logger,"NOMBRE POKEMON: %s",mensaje.nombrePokemon);
	log_info(logger,"POSICION EJE X: %d",mensaje.posicionEjeX);
	log_info(logger,"POSICION EJE Y: %d",mensaje.posicionEjeY);
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
	memcpy(mensaje.nombrePokemon,content+bytesLeidos,mensaje.sizeNombre);

	log_info(logger,"ID MENSAJE: %d",mensaje.id_mensaje);
	log_info(logger,"NOMBRE POKEMON: %s",mensaje.nombrePokemon);
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
}
