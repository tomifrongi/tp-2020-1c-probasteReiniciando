#include "ModoBroker.h"


void ejecutarModoBroker(){
	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");;
	int socketGame = connect_to_server(ipBroker, puertoBroker, NULL);
	if(socketGame != -errno)
		log_info(logger, "CONEXION EXITOSA CON EL BROKER");
	while(1){
		imprimirOpcionesMensajeBroker();
		int opcionMensaje;
		scanf("%d",&opcionMensaje);
		t_message mensajeBroker = obtenerMensajeBroker(opcionMensaje);
		if(mensajeBroker.head != ERROR_MESSAGE)
			send_message(socketGame, mensajeBroker.head,mensajeBroker.content, mensajeBroker.size);
		free(mensajeBroker.content);
	}
}

t_message obtenerMensajeBroker(int opcionMensaje){
	t_message mensaje;
	switch (opcionMensaje){
	case 1:{
		new_pokemon_broker parametros;
		parametros = obtenerParametrosNewBroker();
		mensaje.content = serializarNewContentBroker(parametros);
		mensaje.head = NEW_POKEMON;
		mensaje.size = sizeof(new_pokemon_broker);
		break;
	}
	case 2:{
		appeared_pokemon_broker parametros;
		parametros = obtenerParametrosAppearedBroker();
		mensaje.content = serializarAppearedContentBroker(parametros);
		mensaje.head = APPEARED_POKEMON;
		mensaje.size = sizeof(appeared_pokemon_broker);
		break;
	}
	case 3:{
		catch_pokemon_broker parametros;
		parametros = obtenerParametrosCatchBroker();
		mensaje.content = serializarCatchContentBroker(parametros);
		mensaje.head = CATCH_POKEMON;
		mensaje.size = sizeof(catch_pokemon_broker);
		break;
	}
	case 4:{
		caught_pokemon_broker parametros;
		parametros = obtenerParametrosCaughtBroker();
		mensaje.content = serializarCaughtContentBroker(parametros);
		mensaje.head = CAUGHT_POKEMON;
		mensaje.size = sizeof(caught_pokemon_broker);
		break;
	}
	case 5:{
		get_pokemon_broker parametros;
		parametros = obtenerParametrosGetBroker();
		mensaje.content = serializarGetContentBroker(parametros);
		mensaje.head = GET_POKEMON;
		mensaje.size = sizeof(get_pokemon_broker);
		break;
	}
	default:
		mensaje.head = ERROR_MESSAGE;
		printf("%d no es una opcion valida\n",opcionMensaje);
		break;
	}
	return mensaje;

}


