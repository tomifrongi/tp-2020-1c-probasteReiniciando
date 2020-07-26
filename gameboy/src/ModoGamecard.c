#include "ModoGamecard.h"

void ejecutarModoGamecard(t_list* argumentos){
	char* ipGamecard = config_get_string_value(config, "IP_GAMECARD");
	int puertoGamecard = config_get_int_value(config, "PUERTO_GAMECARD");;
	int socketGame = connect_to_server(ipGamecard, puertoGamecard, NULL);
	if(socketGame != -errno){
	log_info(logger, "CONEXION EXITOSA CON EL PROCESO GAMECARD");
//	while(1){
//	imprimirOpcionesMensajeGamecard();
	int opcionMensaje;
	if(strcmp(list_get(argumentos,2),"NEW_POKEMON") == 0)
		opcionMensaje = 1;
	if(strcmp(list_get(argumentos,2),"CATCH_POKEMON") == 0)
		opcionMensaje = 2;
	if(strcmp(list_get(argumentos,2),"GET_POKEMON") == 0)
		opcionMensaje = 3;
//	scanf("%d",&opcionMensaje);
	t_message mensajeGamecard = obtenerMensajeGamecard(opcionMensaje,argumentos);
	if(mensajeGamecard.head != ERROR_MESSAGE)
	send_message(socketGame, mensajeGamecard.head,mensajeGamecard.content, mensajeGamecard.size);
	free(mensajeGamecard.content);
//	}
	}
	else
		log_info(logger,"ERROR AL CONECTARSE CON EL GAMECARD");
}

t_message obtenerMensajeGamecard(int opcionMensaje,t_list* argumentos){
	t_message mensaje;
	switch(opcionMensaje){
	case 1:{
		new_pokemon_gamecard parametros;
		//GAMECARD NEW_POKEMON [POKEMON] [POSX] [POSY] [CANTIDAD] [ID_MENSAJE]
		parametros.nombrePokemon = list_get(argumentos,3);
		parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;
		parametros.posicionEjeX = atoi(list_get(argumentos,4));
		parametros.posicionEjeY = atoi(list_get(argumentos,5));
		parametros.cantidad = atoi(list_get(argumentos,6));
		parametros.id_mensaje= atoi(list_get(argumentos,7));



//		printf("Ingrese el nombre del pokemon:\n");
//		char nombrePokemon[30];
//		scanf("%s", nombrePokemon);
//		parametros.nombrePokemon = &nombrePokemon[0];
//		parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;
//
//		printf("Ingrese la posicion X:\n");
//		scanf("%d",&parametros.posicionEjeX);
//
//		printf("Ingrese la posicion Y:\n");
//		scanf("%d",&parametros.posicionEjeY);
//
//		printf("Ingrese la cantidad:\n");
//		scanf("%d", &parametros.cantidad);
//
//		printf("Ingrese el id_mensaje mayor a 10000:\n");
//		scanf("%d", &parametros.id_mensaje);

		mensaje.content = serializarNewContentGamecard(parametros);
		mensaje.head = NEW_POKEMON;
		mensaje.size = sizeof(uint32_t)*5+parametros.sizeNombre;
		break;
	}
	case 2:{
		catch_pokemon_gamecard parametros;
		//GAMECARD CATCH_POKEMON [POKEMON] [POSX] [POSY] [ID_MENSAJE]
		parametros.nombrePokemon = list_get(argumentos,3);
		parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;
		parametros.posicionEjeX = atoi(list_get(argumentos,4));
		parametros.posicionEjeY = atoi(list_get(argumentos,5));
		parametros.id_mensaje= atoi(list_get(argumentos,6));

//		printf("Ingrese el nombre del pokemon:\n");
//		char nombrePokemon[30];
//		scanf("%s", nombrePokemon);
//		parametros.nombrePokemon = &nombrePokemon[0];
//		parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;
//
//		printf("Ingrese la posicion X:\n");
//		scanf("%d",&parametros.posicionEjeX);
//
//		printf("Ingrese la posicion Y:\n");
//		scanf("%d",&parametros.posicionEjeY);
//
//		printf("Ingrese el id_mensaje mayor a 10000:\n");
//		scanf("%d", &parametros.id_mensaje);

		mensaje.content = serializarCatchContentGamecard(parametros);
		mensaje.head = CATCH_POKEMON;
		mensaje.size = sizeof(uint32_t)*4+parametros.sizeNombre;
		break;

	}
	case 3:{
		get_pokemon_gamecard parametros;
		//GAMECARD GET_POKEMON [POKEMON] [ID_MENSAJE]
		parametros.nombrePokemon = list_get(argumentos,3);
		parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;
		parametros.id_mensaje= atoi(list_get(argumentos,6));

//		printf("Ingrese el nombre del pokemon:\n");
//		char nombrePokemon[30];
//		scanf("%s", nombrePokemon);
//		parametros.nombrePokemon = &nombrePokemon[0];
//		parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;
//
//		printf("Ingrese el id_mensaje mayor a 10000:\n");
//		scanf("%d", &parametros.id_mensaje);

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

