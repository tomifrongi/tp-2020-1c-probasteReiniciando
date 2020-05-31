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

pthread_mutex_t mutexId;
pthread_mutex_t mutexLogger;

int main(void) {
	inicializarLogger("./Debug"); //logea ok!!
	PUERTO = 8080;
	ID_INICIAL = 0;
	crearEstructurasAdministrativas();
	iniciarMutexs();
	iniciarListasIds();
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

				pthread_mutex_lock(&mutexLogger);
				log_info(logger, "NEW POKEMON RECIBIDO");
				pthread_mutex_unlock(&mutexLogger);

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

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);

				log_info(logger,"%s", &mensaje.nombrePokemon);

				pthread_mutex_lock(&mutexQueueNew);
				queue_push(new_admin.queue, &mensaje);
				pthread_mutex_unlock(&mutexQueueNew);
				//enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				break;
			}
			case APPEARED_POKEMON:{
				pthread_mutex_lock(&mutexLogger);
				log_info(logger, "APPEARED POKEMON RECIBIDO");
				pthread_mutex_unlock(&mutexLogger);

				void *aux = message->content;
				appeared_pokemon_enviar mensaje;

				memcpy(&mensaje.idCorrelativo,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.nombrePokemon,aux,mensaje.sizeNombre);
				aux += mensaje.sizeNombre;
				memcpy(&mensaje.posicionEjeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.posicionEjeY,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);

				log_info(logger,"%s", &mensaje.nombrePokemon);
				log_info(logger,"%d", mensaje.sizeNombre);

				pthread_mutex_lock(&mutexQueueAppeared);
				if(list_contain(idsCorrelativosAppeared,&mensaje.idCorrelativo)==NULL){
					queue_push(appeared_admin.queue, &mensaje);
					list_add(idsCorrelativosAppeared,&mensaje.idCorrelativo);
					log_info(logger,"mensaje agregado a la cola");
				}
				pthread_mutex_unlock(&mutexQueueAppeared);


				//enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				break;
			}
			case CATCH_POKEMON:{

				pthread_mutex_lock(&mutexLogger);
				log_info(logger, "CATCH POKEMON RECIBIDO");
				pthread_mutex_unlock(&mutexLogger);

				void *aux = message->content;
				catch_pokemon_enviar mensaje;
				memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.nombrePokemon,aux,mensaje.sizeNombre);
				aux += mensaje.sizeNombre;
				memcpy(&mensaje.posicionEjeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.posicionEjeY,aux,sizeof(uint32_t));

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);

				pthread_mutex_lock(&mutexQueueCatch);
				queue_push(catch_admin.queue, &mensaje);
				pthread_mutex_unlock(&mutexQueueCatch);
				//enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				break;
			}
			case CAUGHT_POKEMON:{

				pthread_mutex_lock(&mutexLogger);
				log_info(logger, "CAUGHT POKEMON RECIBIDO");
				pthread_mutex_unlock(&mutexLogger);

				void *aux = message->content;
				caught_pokemon_enviar mensaje;
				memcpy(&mensaje.idCorrelativo,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.pokemonAtrapado,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.idCorrelativo,aux,sizeof(uint32_t));

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);

				pthread_mutex_lock(&mutexQueueCaught);
				if(list_contain(&idsCorrelativosCaught,&mensaje.idCorrelativo)==NULL){
					queue_push(caught_admin.queue, &mensaje);
					list_add(idsCorrelativosCaught,&mensaje.idCorrelativo);
				}
				pthread_mutex_unlock(&mutexQueueCaught);
				//enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				break;
			}
			case GET_POKEMON:{

				pthread_mutex_lock(&mutexLogger);
				log_info(logger, "GET POKEMON RECIBIDO");
				pthread_mutex_unlock(&mutexLogger);

				void *aux = message->content;
				get_pokemon_enviar mensaje;
				memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(mensaje.nombrePokemon,aux,mensaje.sizeNombre);

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);

				pthread_mutex_lock(&mutexQueueGet);
				queue_push(get_admin.queue, &mensaje);
				pthread_mutex_unlock(&mutexQueueGet);
				//enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				break;
			}
			case LOCALIZED_POKEMON:{

				pthread_mutex_lock(&mutexLogger);
				log_info(logger, "LOCALIZED POKEMON RECIBIDO");
				pthread_mutex_unlock(&mutexLogger);

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

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);

				pthread_mutex_lock(&mutexQueueLocalized);
				if(list_contain(&idsCorrelativosLocalized,&mensaje.idCorrelativo)==NULL){
					queue_push(localized_admin.queue, &mensaje);
					list_add(idsCorrelativosLocalized,&mensaje.idCorrelativo);
				}
				pthread_mutex_unlock(&mutexQueueLocalized);

				//enviarConfirmacion(mensaje.id_mensaje,broker_sock);

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
				log_info(logger, "ERROR COMUNICACIÓN");
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

	void* content = malloc(sizeof(id_mensaje));
	size_t size = sizeof(uint32_t);
	memcpy(content,&id,sizeof(uint32_t));
	send_message(broker_sock, CONFIRMACION,content,size);
}

void agregarSuscripcion (uint32_t id_cola, int broker_sock){
	switch(id_cola){
	case NEW:
		list_add(new_admin.suscriptores,&broker_sock);
		log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA NEW",broker_sock);
		break;
	case APPEARED:
		list_add(appeared_admin.suscriptores,&broker_sock);
		log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA APPEARED",broker_sock);
		break;
	case GET:
		list_add(get_admin.suscriptores,&broker_sock);
		log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA GET",broker_sock);
		break;
	case LOCALIZED:
		list_add(localized_admin.suscriptores,&broker_sock);
		log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA LOCALIZED",broker_sock);
		break;
	case CATCH:
		list_add(catch_admin.suscriptores,&broker_sock);
		log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA CATCH",broker_sock);
		break;
	case CAUGHT:
		list_add(caught_admin.suscriptores,&broker_sock);
		log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA CAUGHT",broker_sock);
		break;
	default:
		break;

	}
}

void iniciarMutexs(){
	pthread_mutex_init(&mutexId,NULL);
	pthread_mutex_init(&mutexLogger,NULL);
	pthread_mutex_init(&mutexQueueNew,NULL);
	pthread_mutex_init(&mutexQueueAppeared,NULL);
	pthread_mutex_init(&mutexQueueGet,NULL);
	pthread_mutex_init(&mutexQueueLocalized,NULL);
	pthread_mutex_init(&mutexQueueCatch,NULL);
	pthread_mutex_init(&mutexQueueCaught,NULL);
	pthread_mutex_unlock(&mutexQueueLocalized);

}
void iniciarListasIds(){
	idsCorrelativosAppeared = list_create();
	idsCorrelativosLocalized = list_create();
	idsCorrelativosCaught = list_create();
}
