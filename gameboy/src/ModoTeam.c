#include "ModoTeam.h"

#ifndef MODOTEAM_C_
#define MODOTEAM_C_


void ejecutarModoTeam(t_list* argumentos){
	char* ipTeam = config_get_string_value(config, "IP_TEAM");
	int puertoTeam = config_get_int_value(config, "PUERTO_TEAM");;
	int socketGame = connect_to_server(ipTeam, puertoTeam, NULL);
	if(socketGame != -errno){
	log_info(logger, "CONEXION EXITOSA CON EL PROCESO TEAM");
//	while(1){
	t_message mensajeTeam = obtenerMensajeTeam(argumentos);
	send_message(socketGame, mensajeTeam.head,mensajeTeam.content, mensajeTeam.size);

	free(mensajeTeam.content);
	t_message* mensajeACK = recv_message(socketGame);
	free_t_message(mensajeACK);
//	}
	}
	else
		log_info(logger,"ERROR AL CONECTARSE CON EL TEAM");
}

t_message obtenerMensajeTeam(t_list* argumentos){
	t_message mensaje;
	appeared_pokemon_team parametros;

	parametros.id_mensaje = 10100;
	parametros.idCorrelativo = 0;
	parametros.nombrePokemon = list_get(argumentos,3);
	parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;
	parametros.posicionEjeX = atoi(list_get(argumentos,4));
	parametros.posicionEjeY = atoi(list_get(argumentos,5));


//	printf("Ingrese el nombre del pokemon:\n");
//	char nombrePokemon[30];
//	scanf("%s", nombrePokemon);
//	parametros.nombrePokemon = nombrePokemon;
//	parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;
//
//	printf("Ingrese la posicion X:\n");
//	scanf("%d",&parametros.posicionEjeX);
//
//	printf("Ingrese la posicion Y:\n");
//	scanf("%d",&parametros.posicionEjeY);

	mensaje.content = serializarAppearedContentTeam(parametros);
	mensaje.head = APPEARED_POKEMON;
	mensaje.size = sizeof(uint32_t)*5+parametros.sizeNombre;
	return mensaje;

}

#endif /* MODOTEAM_C_ */
