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

int main(void) {
	inicializarLogger("./Debug"); //logea ok!!
	PUERTO = 8080;
	ID_INICIAL = 0;
	crearEstructurasAdministrativas();
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
				new_pokemon_enviar mensaje;
				memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.nombrePokemon,aux,mensaje.sizeNombre);
				aux += mensaje.sizeNombre;
				memcpy(&mensaje.cantidad,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.posicionEjeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.posicionEjeY,aux,sizeof(uint32_t));
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(new_admin.queue, &mensaje);
				printf("%s", &mensaje.nombrePokemon);

				//enviarConfirmacion(ID_INICIAL,broker_sock);



				break;
			}
			case APPEARED_POKEMON:{
				log_info(logger, "APPEARED POKEMON RECIBIDO");
				void *aux = message->content;
				appeared_pokemon_enviar mensaje;
				memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.nombrePokemon,aux,mensaje.sizeNombre);
				aux += mensaje.sizeNombre;
				memcpy(&mensaje.posicionEjeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.posicionEjeY,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.idCorrelativo,aux,sizeof(uint32_t));
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(appeared_admin.queue, &mensaje);
				//enviarConfirmacion(ID_INICIAL,broker_sock);

				break;
			}
			case CATCH_POKEMON:{
				log_info(logger, "CATCH POKEMON RECIBIDO");
				void *aux = message->content;
				catch_pokemon_enviar mensaje;
				memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.nombrePokemon,aux,mensaje.sizeNombre);
				aux += mensaje.sizeNombre;
				memcpy(&mensaje.posicionEjeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.posicionEjeY,aux,sizeof(uint32_t));
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(catch_admin.queue, &mensaje);
				//enviarConfirmacion(ID_INICIAL,broker_sock);

				break;
			}
			case CAUGHT_POKEMON:{
				log_info(logger, "CAUGHT POKEMON RECIBIDO");
				void *aux = message->content;
				caught_pokemon_enviar mensaje;
				memcpy(&mensaje.idCorrelativo,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.pokemonAtrapado,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.idCorrelativo,aux,sizeof(uint32_t));
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(caught_admin.queue, &mensaje);
				//enviarConfirmacion(ID_INICIAL,broker_sock);

				break;
			}
			case GET_POKEMON:{
				log_info(logger, "GET POKEMON RECIBIDO");
				void *aux = message->content;
				get_pokemon_enviar mensaje;
				memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(mensaje.nombrePokemon,aux,mensaje.sizeNombre);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(get_admin.queue, &mensaje);
				//enviarConfirmacion(ID_INICIAL,broker_sock);

				break;
			}
			case LOCALIZED_POKEMON:{
				log_info(logger, "LOCALIZED POKEMON RECIBIDO");
				void *aux = message->content;
				localized_pokemon_enviar mensaje;
				uint32_t largoLista;
				uint32_t *posicion;
				memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.nombrePokemon,aux,mensaje.sizeNombre);
				aux += mensaje.sizeNombre;
				memcpy(&mensaje.cantidadPosiciones,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&largoLista,aux,sizeof(uint32_t)); //primero nos mandan el largo de la lista
				aux += sizeof(uint32_t);
				for(int i=0;i < largoLista;i++){
					memcpy(posicion,aux,sizeof(uint32_t));
					aux += sizeof(uint32_t);
					list_add(&mensaje.posiciones, posicion);
				}
				aux += sizeof(uint32_t);
				memcpy(&mensaje.idCorrelativo,aux,sizeof(uint32_t));
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(localized_admin.queue, &mensaje);
				//enviarConfirmacion(ID_INICIAL,broker_sock);

				break;
			}
			case SUSCRIPCION:{
				uint32_t id_cola;
				void *aux = message->content;
				memcpy(&id_cola,aux,sizeof(uint32_t));
				agregarSuscripcion(id_cola,broker_sock);
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


void enviarConfirmacion(uint32_t id, int broker_sock){

	//id_mensaje * mensaje;
	size_t size = sizeof(uint32_t);
	//memcpy(&mensaje.id_mensaje,id,sizeof(uint32_t));
	send_message(listener_socket, CONFIRMACION,&id,size);
}

void agregarSuscripcion (uint32_t id_cola, int broker_sock){
	switch(id_cola){
	case NEW:
		list_add(new_admin.suscriptores,broker_sock);
		log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA NEW",broker_sock);
		break;
	case APPEARED:
		list_add(appeared_admin.suscriptores,broker_sock);
		log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA APPEARED",broker_sock);
		break;
	case GET:
		list_add(get_admin.suscriptores,broker_sock);
		log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA GET",broker_sock);
		break;
	case LOCALIZED:
		list_add(localized_admin.suscriptores,broker_sock);
		log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA LOCALIZED",broker_sock);
		break;
	case CATCH:
		list_add(catch_admin.suscriptores,broker_sock);
		log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA CATCH",broker_sock);
		break;
	case CAUGHT:
		list_add(caught_admin.suscriptores,broker_sock);
		log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA CAUGHT",broker_sock);
		break;
	default:
		break;

	}
}

