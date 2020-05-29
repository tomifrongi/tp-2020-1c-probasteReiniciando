#include "Gameboy.h"



 int main(void) {

	t_config * config = config_create("Gameboy.config");
	t_log* log =  log_create("Gameboy.log", "Gameboy", 1, LOG_LEVEL_INFO);

//	TODO Llegada de un nuevo mensaje a una cola de mensajes.



	imprimirOpciones();
	int opcionProceso;
	scanf("%d",&opcionProceso);

	switch(opcionProceso){
	case 1:{
		char* ipBroker = config_get_string_value(config, "IP_BROKER");
		int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");;
		int socketGame = connect_to_server(ipBroker, puertoBroker, NULL);
		if(socketGame != -errno)
			log_info(log, "CONEXION EXITOSA CON EL BROKER");
		imprimirOpcionesMensajeBroker();
		int opcionMensaje;
		scanf("%d",&opcionMensaje);
		t_message mensajeBroker = obtenerMensajeBroker(opcionMensaje);
		if(mensajeBroker.head != ERROR_MESSAGE)
		send_message(socketGame, mensajeBroker.head,mensajeBroker.content, mensajeBroker.size);
		break;
	}

	case 2:{
		char* ipTeam = config_get_string_value(config, "IP_TEAM");
		int puertoTeam = config_get_int_value(config, "PUERTO_TEAM");;
		int socketGame = connect_to_server(ipTeam, puertoTeam, NULL);
		if(socketGame != -errno)
			log_info(log, "CONEXION EXITOSA CON EL PROCESO TEAM");
		t_message mensajeTeam = obtenerMensajeTeam();
		send_message(socketGame, mensajeTeam.head,mensajeTeam.content, mensajeTeam.size);
		break;
	}

	case 3:{
		char* ipGamecard = config_get_string_value(config, "IP_GAMECARD");
		int puertoGamecard = config_get_int_value(config, "PUERTO_GAMECARD");;
		int socketGame = connect_to_server(ipGamecard, puertoGamecard, NULL);
		if(socketGame != -errno)
			log_info(log, "CONEXION EXITOSA CON EL PROCESO GAMECARD");
		imprimirOpcionesMensajeGamecard();
		int opcionMensaje;
		scanf("%d",&opcionMensaje);
		t_message mensajeGamecard = obtenerMensajeGamecard(opcionMensaje);
		if(mensajeGamecard.head != ERROR_MESSAGE)
		send_message(socketGame, mensajeGamecard.head,mensajeGamecard.content, mensajeGamecard.size);
		break;
	}
//TODO modoSuscriptor
	case 4: {
		char* ipBroker = config_get_string_value(config, "IP_BROKER");
		int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");;
		int socketGame = connect_to_server(ipBroker, puertoBroker, NULL);
		if(socketGame != -errno)
			log_info(log, "CONEXION EXITOSA CON EL BROKER");
		id_cola id = obtenerID();
		t_message mensajeSuscripcion = obtenerMensajeSuscripcion(id);
		int tiempo = obtenerTiempo();
		send_message(socketGame, mensajeSuscripcion.head,mensajeSuscripcion.content, mensajeSuscripcion.size);
		char* nombreCola= obtenerNombreCola(id);
		log_info(log, "SUSCRIPCION EXITOSA A LA COLA %s\n",nombreCola);
/*		while(noSeTermineElTiempo())
			escuchar()
			if(llegoAlgo())
				logearMensaje()
*/
		break;
	}

	default:
		printf("%d no es una opcion valida\n",opcionProceso);
	}

	return 0;


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

t_message obtenerMensajeTeam(){
	t_message mensaje;
	appeared_pokemon_team parametros;
	parametros = obtenerParametrosAppearedTeam();
	mensaje.content = serializarAppearedContentTeam(parametros);
	mensaje.head = APPEARED_POKEMON;
	mensaje.size = sizeof(appeared_pokemon_team);
	return mensaje;

}

t_message obtenerMensajeGamecard(int opcionMensaje){
	t_message mensaje;
	switch(opcionMensaje){
	case 1:{
		new_pokemon_gamecard parametros;
		parametros = obtenerParametrosNewGamecard();
		mensaje.content = serializarNewContentGamecard(parametros);
		mensaje.head = NEW_POKEMON;
		mensaje.size = sizeof(new_pokemon_gamecard);
		break;
	}
	case 2:{
		catch_pokemon_gamecard parametros;
		parametros = obtenerParametrosCatchGamecard();
		mensaje.content = serializarCatchContentGamecard(parametros);
		mensaje.head = CATCH_POKEMON;
		mensaje.size = sizeof(catch_pokemon_gamecard);
		break;

	}
	case 3:{
		get_pokemon_gamecard parametros;
		parametros = obtenerParametrosGetGamecard();
		mensaje.content = serializarGetContentGamecard(parametros);
		mensaje.head = GET_POKEMON;
		mensaje.size = sizeof(get_pokemon_gamecard);
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

t_message obtenerMensajeSuscripcion(id_cola id){

	t_message mensaje;
	mensaje.content = serializarSuscripcionContent(id);
	mensaje.head = CONFIRMACION;
	mensaje.size = sizeof(suscripcion);

	return mensaje;

}

int obtenerTiempo(){
	printf("Escriba el tiempo:\n");
	int tiempo;
	scanf("%d",&tiempo);

	return tiempo;
}
