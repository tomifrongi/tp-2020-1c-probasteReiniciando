#include "ModoTeam.h"

#ifndef MODOTEAM_C_
#define MODOTEAM_C_


void ejecutarModoTeam(){
	char* ipTeam = config_get_string_value(config, "IP_TEAM");
	int puertoTeam = config_get_int_value(config, "PUERTO_TEAM");;
	int socketGame = connect_to_server(ipTeam, puertoTeam, NULL);
	if(socketGame != -errno)
		log_info(logger, "CONEXION EXITOSA CON EL PROCESO TEAM");
	while(1){
	t_message mensajeTeam = obtenerMensajeTeam();
	send_message(socketGame, mensajeTeam.head,mensajeTeam.content, mensajeTeam.size);
	}
}

t_message obtenerMensajeTeam(){
	t_message mensaje;
	appeared_pokemon_team parametros;
	parametros = obtenerParametrosAppearedTeam();
	mensaje.content = serializarAppearedContentTeam(parametros);
	mensaje.head = APPEARED_POKEMON;
	mensaje.size = sizeof(uint32_t)*5+parametros.sizeNombre;
	return mensaje;

}

#endif /* MODOTEAM_C_ */
