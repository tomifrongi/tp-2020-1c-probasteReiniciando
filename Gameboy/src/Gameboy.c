#include "Gameboy.h"


int main(void) {


	//TODO config
//	t_config * config = config_create("Gameboy");
	//TODO log

	imprimirOpciones();
	int opcionProceso;
	scanf("%d",&opcionProceso);

	switch(opcionProceso){
	case 1:{
//		int socketGame = connect_to_server(char* host, 8080, NULL);
		imprimirOpcionesMensajeBroker();
		int opcionMensaje;
		scanf("%d",&opcionMensaje);
		t_message mensajeBroker = obtenerMensajeBroker(opcionMensaje);
//		if(mensajeBroker.head != ERROR_MESSAGE)
//		send_message(socketGame, mensajeBroker.head,mensajeBroker.content, mensajeBroker.size);
		break;
	}

	case 2:{
//		int socketGame = connect_to_server(char* host, 8080, NULL);
//		t_message mensajeTeam = obtenerMensajeTeam();
//		send_message(socketGame, mensajeTeam.head,mensajeTeam.content, mensajeTeam.size);
		break;
	}

	case 3:{
//		int socketGame = connect_to_server(char* host, 8080, NULL);
		imprimirOpcionesMensajeGamecard();
		int opcionMensaje;
		scanf("%d",&opcionMensaje);
		t_message mensajeGamecard = obtenerMensajeGamecard(opcionMensaje);
//		if(mensajeGamecard.head != ERROR_MESSAGE)
//		send_message(socketGame, mensajeGamecard.head,mensajeGamecard.content, mensajeGamecard.size);
		break;
			}
//TODO modoSuscriptor
	case 4: {
		imprimirOpcionesColas();
		int opcionCola;
		scanf("%d",&opcionCola);
		imprimirOpcionTiempo();
		int tiempo;
		scanf("%d",&tiempo);
		t_message mensajeSuscripcion = obtenerMensajeGamecard(opcionCola);
//		int socketGame = connect_to_server(char* host, 8080, NULL);
//		send_message(socketGame, mensajeGamecard.head,mensajeGamecard.content, mensajeGamecard.size);
		//escuchar()
		//terminar()

		break;
	}

	default:
		printf("%d no es una opcion valida\n",opcionProceso);
	}


//		printf("Escriba el numero de mensaje:\n");
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
//TODO obtenermensajeteam
/*
t_message obtenerMensajeTeam(){
	t_message mensaje;
	appeared_pokemon_team parametros;
	parametros = obtenerParametrosAppearedTeam();
	mensaje.content = serializarAppearedContentTeam(parametros);
	mensaje.head = APPEARED_POKEMON;
	mensaje.size = sizeof(appeared_pokemon_team);
	return mensaje;

}
*/
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

