#include "ModoBroker.h"


void ejecutarModoBroker(t_list* argumentos){
	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");;
	int socketGame = connect_to_server(ipBroker, puertoBroker, NULL);
	if(socketGame != -errno)
		log_info(logger, "CONEXION EXITOSA CON EL BROKER");

//		imprimirOpcionesMensajeBroker();

	int opcionMensaje;
	if(strcmp((char*) list_get(argumentos,2),"NEW_POKEMON") == 0)
		opcionMensaje = 1;
	if(strcmp((char*) list_get(argumentos,2),"APPEARED_POKEMON") == 0)
		opcionMensaje = 2;
	if(strcmp((char*) list_get(argumentos,2),"CATCH_POKEMON") == 0)
		opcionMensaje = 3;
	if(strcmp((char*) list_get(argumentos,2),"CAUGHT_POKEMON") == 0)
		opcionMensaje = 4;
	if(strcmp((char*) list_get(argumentos,2),"GET_POKEMON") == 0)
		opcionMensaje = 5;

//		scanf("%d",&opcionMensaje);

	t_message mensajeBroker = obtenerMensajeBroker(opcionMensaje,argumentos);
	if(mensajeBroker.head != ERROR_MESSAGE)
		send_message(socketGame, mensajeBroker.head,mensajeBroker.content, mensajeBroker.size);
	free(mensajeBroker.content);
	t_message* mensajeConfirmacionID =recv_message(socketGame);
	uint32_t idConf;
	memcpy(&idConf,mensajeConfirmacionID->content,sizeof(uint32_t));
	//log_info(logger,"ACK %d RECIBIDO DEL BROKER",idConf);

}

t_message obtenerMensajeBroker(int opcionMensaje,t_list* argumentos){
	t_message mensaje;
	switch (opcionMensaje){
	case 1:{
		new_pokemon_broker parametros;

		parametros.nombrePokemon = list_get(argumentos,3);
		parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;
		parametros.posicionEjeX = atoi(list_get(argumentos,4));
		parametros.posicionEjeY = atoi(list_get(argumentos,5));
		parametros.cantidad = atoi(list_get(argumentos,6));

//		printf("Ingrese el nombre del pokemon:\n");
//		char nombrePokemon[100];
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

		mensaje.content = serializarNewContentBroker(parametros);
		mensaje.head = NEW_POKEMON;
		mensaje.size = sizeof(uint32_t)*4+parametros.sizeNombre;
		break;
	}
	case 2:{
		appeared_pokemon_broker parametros;
		parametros.nombrePokemon = list_get(argumentos,3);
		parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;
		parametros.posicionEjeX = atoi(list_get(argumentos,4));
		parametros.posicionEjeY = atoi(list_get(argumentos,5));
		parametros.idCorrelativo = atoi(list_get(argumentos,6));

//		printf("Ingrese el nombre del pokemon:\n");
//		char nombrePokemon[100];
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
//		printf("Ingrese el id_mensaje_correlativo:\n");
//		scanf("%d", &parametros.idCorrelativo);

		mensaje.content = serializarAppearedContentBroker(parametros);
		mensaje.head = APPEARED_POKEMON;
		mensaje.size = sizeof(uint32_t)*4+parametros.sizeNombre;
		break;
	}
	case 3:{
	        catch_pokemon_broker parametros;
			parametros.nombrePokemon = list_get(argumentos,3);
			parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;
			parametros.posicionEjeX = atoi(list_get(argumentos,4));
			parametros.posicionEjeY = atoi(list_get(argumentos,5));

//	        printf("Ingrese el nombre del pokemon:\n");
//	        char nombrePokemon[100];
//	        scanf("%s", nombrePokemon);
//	        parametros.nombrePokemon = &nombrePokemon[0];
//	        parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;
//
//	        printf("Ingrese la posicion X:\n");
//	        scanf("%d",&parametros.posicionEjeX);
//
//	        printf("Ingrese la posicion Y:\n");
//	        scanf("%d",&parametros.posicionEjeY);
	        mensaje.content = serializarCatchContentBroker(parametros);
	        mensaje.head = CATCH_POKEMON;
	        mensaje.size = sizeof(uint32_t)*3+parametros.sizeNombre;
	        break;
	    }
	case 4:{
		caught_pokemon_broker parametros;
		parametros.idCorrelativo = atoi(list_get(argumentos,3));
		char* atrapado = list_get(argumentos,4);
		if(strcmp(atrapado,"OK") == 0)
			parametros.pokemonAtrapado = 1;
		else if(strcmp(atrapado,"FAIL") == 0)
			parametros.pokemonAtrapado = 0;

//		printf("Ingrese el id_mensaje_correlativo:\n");
//		scanf("%d", &parametros.idCorrelativo);
//
//		printf("Ingrese 1 si fue atrapado y 0 si no:\n");
//		scanf("%d", &parametros.pokemonAtrapado);

		mensaje.content = serializarCaughtContentBroker(parametros);
		mensaje.head = CAUGHT_POKEMON;
		mensaje.size = sizeof(caught_pokemon_broker);
		break;
	}
	case 5:{
		get_pokemon_broker parametros;
		parametros.nombrePokemon = list_get(argumentos,3);
		parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;

//		printf("Ingrese el nombre del pokemon:\n");
//		char nombrePokemon[100];
//		scanf("%s", nombrePokemon);
//		parametros.nombrePokemon = &nombrePokemon[0];
//		parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;

		mensaje.content = serializarGetContentBroker(parametros);
		mensaje.head = GET_POKEMON;
		mensaje.size = sizeof(uint32_t)+parametros.sizeNombre;
		break;
	}
	default:
		mensaje.head = ERROR_MESSAGE;
		printf("%d no es una opcion valida\n",opcionMensaje);
		break;
	}
	return mensaje;

}


