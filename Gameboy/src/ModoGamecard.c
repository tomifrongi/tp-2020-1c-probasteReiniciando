#include "ModoGamecard.h"

void ejecutarModoGamecard(){
	char* ipGamecard = config_get_string_value(config, "IP_GAMECARD");
	int puertoGamecard = config_get_int_value(config, "PUERTO_GAMECARD");;
	int socketGame = connect_to_server(ipGamecard, puertoGamecard, NULL);
	if(socketGame != -errno)
		log_info(logger, "CONEXION EXITOSA CON EL PROCESO GAMECARD");
	while(1){
	imprimirOpcionesMensajeGamecard();
	int opcionMensaje;
	scanf("%d",&opcionMensaje);
	t_message mensajeGamecard = obtenerMensajeGamecard(opcionMensaje);
	if(mensajeGamecard.head != ERROR_MESSAGE)
	send_message(socketGame, mensajeGamecard.head,mensajeGamecard.content, mensajeGamecard.size);
	}
}

t_message obtenerMensajeGamecard(int opcionMensaje){
	t_message mensaje;
	switch(opcionMensaje){
	case 1:{
		new_pokemon_gamecard parametros;
		parametros = obtenerParametrosNewGamecard();
		mensaje.content = serializarNewContentGamecard(parametros);
		mensaje.head = NEW_POKEMON;
		mensaje.size = sizeof(uint32_t)*5+parametros.sizeNombre;
		break;
	}
	case 2:{
		catch_pokemon_gamecard parametros;
		parametros = obtenerParametrosCatchGamecard();
		mensaje.content = serializarCatchContentGamecard(parametros);
		mensaje.head = CATCH_POKEMON;
		mensaje.size = sizeof(uint32_t)*4+parametros.sizeNombre;
		break;

	}
	case 3:{
		get_pokemon_gamecard parametros;
		parametros = obtenerParametrosGetGamecard();
		mensaje.content = serializarGetContentGamecard(parametros);
		mensaje.head = GET_POKEMON;
		mensaje.size = sizeof(uint32_t)*2+parametros.sizeNombre;
		break;
	}
	default:{
		mensaje.head = ERROR_MESSAGE;
		printf("%d no es una opcion valida\n",opcionMensaje);
		break;
	}

	}
	return mensaje;
}

