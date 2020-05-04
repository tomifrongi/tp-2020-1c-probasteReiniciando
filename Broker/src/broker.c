/*
 ============================================================================
 Name        : Broker.c
 Author      : Tomas
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "broker.h"

/* COSAS A IR VIENDO
 *
 * Manejo de semaforos para los hilos, que se dara en el handler de clientes
 * Entender bien como carajo funciona pthread
 * Hacer una prueba de la funcion init_broker_server, corriendo un cliente basico
 * Revisar los header necesarios, se dejan en el .h los actuales, pero pueden cambiar
 * Que hacer dentro de cada caso para el manejo de las colas
 * Probar levantar los datos usando el config que ya se creo
 * Manejo del listado de procesos que se encuentran actualmente conectados
 * Revisar el codigo en general? Eso se puede? Alguien me puede reportar?
 *
 * MERCADOLIBRE ME PIDE QUE PONGA MAS CARACTERES LA VERDAD BUENISMO EL TP NO ENTIENDO MUCHO PERO GENIAL EL MONOPATIN SOLAR
 *
 */

int main(void) {
	inicializarLogger("./Debug"); //logea ok!!
	PUERTO = 8080;
	ID_INICIAL = 0;
	crearColas();
	init_broker_server();
	return EXIT_SUCCESS;
}

void init_broker_server() {
	listener_socket = init_server(PUERTO);
	log_info(logger, "Servidor levantado! Escuchando en %i",PUERTO);
	struct sockaddr broker_cli;
	socklen_t len = sizeof(broker_cli);
	do {
		int broker_sock = accept(listener_socket, &broker_cli, &len);
		if (broker_sock > 0) {
			log_info(logger, "NUEVA CONEXIÓN");
			pthread_t broker_cli_thread;
			pthread_create(&broker_cli_thread, NULL, handler_clients,
					(void*) (broker_sock));
			pthread_detach(broker_cli_thread);
		} else {
			log_error(logger, "Error aceptando conexiones: %s", strerror(errno));
		}
	} while (1);
}

void* handler_clients(void* socket){
	int broker_sock = (int) (socket);
	char* id_cliente; //Id del cliente actual
	bool executing = true;
	while(executing){
		t_message* message = recv_message(broker_sock);
		switch(message->head){
			case NEW_POKEMON:{
				log_info(logger, "NEW POKEMON RECIBIDO");
				void*aux=message->content;
				new_pokemon_message mensaje;
				memcpy(&mensaje.contenidoMensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(mensaje.contenidoMensaje.nombrePokemon,aux,mensaje.contenidoMensaje.sizeNombre);
				aux += mensaje.contenidoMensaje.sizeNombre;
				memcpy(&mensaje.contenidoMensaje.cantidad,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.contenidoMensaje.posicion.ejeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.contenidoMensaje.posicion.ejeY,aux,sizeof(uint32_t));
				mensaje.id = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(new_pokemon_queue, &mensaje);
				break;
			}
			case APPEARED_POKEMON:{
				log_info(logger, "APPEARED POKEMON RECIBIDO");
				void *aux = message->content;
				appeared_pokemon_message mensaje;
				memcpy(&mensaje.contenidoMensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(mensaje.contenidoMensaje.nombrePokemon,aux,mensaje.contenidoMensaje.sizeNombre);
				aux += mensaje.contenidoMensaje.sizeNombre;
				memcpy(&mensaje.contenidoMensaje.posicion.ejeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.contenidoMensaje.posicion.ejeY,aux,sizeof(uint32_t));
				mensaje.id = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(appeared_pokemon_queue, &mensaje);

				break;
			}
			case CATCH_POKEMON:{
				log_info(logger, "CATCH POKEMON RECIBIDO");
				void *aux = message->content;
				catch_pokemon_message mensaje;
				memcpy(&mensaje.contenidoMensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(mensaje.contenidoMensaje.nombrePokemon,aux,mensaje.contenidoMensaje.sizeNombre);
				aux += mensaje.contenidoMensaje.sizeNombre;
				memcpy(&mensaje.contenidoMensaje.posicion.ejeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.contenidoMensaje.posicion.ejeY,aux,sizeof(uint32_t));
				mensaje.id = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(catch_pokemon_queue, &mensaje);

				break;
			}
			case CAUGHT_POKEMON:{
				log_info(logger, "CAUGHT POKEMON RECIBIDO");
				void *aux = message->content;
				caught_pokemon_message mensaje;
				memcpy(&mensaje.idCorrelativo,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.contenidoMensaje.pokemonAtrapado,aux,sizeof(uint32_t));
				mensaje.id = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(caught_pokemon_queue, &mensaje);
				break;
			}
			case GET_POKEMON:{
				log_info(logger, "GET POKEMON RECIBIDO");
				void *aux = message->content;
				get_pokemon_message mensaje;
				memcpy(&mensaje.contenidoMensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(mensaje.contenidoMensaje.nombrePokemon,aux,mensaje.contenidoMensaje.sizeNombre);
				mensaje.id = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(get_pokemon_queue, &mensaje);

				break;
			}
			case LOCALIZED_POKEMON:{
				log_info(logger, "LOCALIZED POKEMON RECIBIDO");
				void *aux = message->content;
				localized_pokemon_message mensaje;
				memcpy(&mensaje.contenidoMensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(mensaje.contenidoMensaje.nombrePokemon,aux,mensaje.contenidoMensaje.sizeNombre);
				aux += mensaje.contenidoMensaje.sizeNombre;
				memcpy(&mensaje.contenidoMensaje.cantidadPosiciones,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);

				//TODO -  Faltaria ver de como guardar la lista de cordenadas que mandamos en el t_list

				mensaje.id = ID_INICIAL;
				ID_INICIAL ++;
				break;
			}
			case NO_CONNECTION:
				log_info(logger, "CLIENTE DESCONECTADO");
				free_t_message(message);
				pthread_exit(NULL);
				return NULL;
				break;
			case ERROR_RECV:
				free_t_message(message);
				log_info(logger, "ERROR COMUNIACIÓN");
				pthread_exit(NULL);
				return NULL;
				break;
			default:
				break;
			}
			free_t_message(message);
	}
	return NULL;
}




