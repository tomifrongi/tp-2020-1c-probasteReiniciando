/*
 ============================================================================
 Name        : Game-Card.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "GameCard.h"

int main(void) {

	t_config * config = config_create("GameCard.config");
	t_log* log =  log_create("GameCard.log", "GameCard", 1, LOG_LEVEL_INFO);

	//primero hay que intentar asociarnos a las colas de mensajes del broker
	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
	int socketGame = connect_to_server(ipBroker, puertoBroker, NULL);
	if(socketGame != -errno){
		log_info(log, "CONEXION EXITOSA CON EL BROKER");
		t_message mensajeBroker;
		void* content = malloc(sizeof(uint32_t));
		uint32_t numero = 1; // es el new pero no se por que no funciona
		memcpy (content, &numero, sizeof(uint32_t));
		mensajeBroker.head = 7; // esto es lo mismo pero para suscripcion no se por que funciona mal eso
		mensajeBroker.content = content;
		mensajeBroker.size = sizeof(uint32_t);
		send_message(socketGame, mensajeBroker.head,mensajeBroker.content, mensajeBroker.size);
	}

}
