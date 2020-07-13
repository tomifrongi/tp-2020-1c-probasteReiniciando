/*
 ============================================================================
 Name        : Broker.c
 Author      : Tomas y Juan
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
	PUERTO_BROKER = 8080;
	ID_INICIAL = 0;
	TAMANO_MEMORIA =64;
	TAMANO_MINIMO_PARTICION = 4;
	ALGORITMO_MEMORIA = "BS";
	ALGORITMO_REEMPLAZO ="FIFO";
	ALGORITMO_PARTICION_LIBRE ="FF";
	FRECUENCIA_COMPACTACION = 1;

	crearEstructurasAdministrativas();
	iniciarMutexs();
	iniciarListasIds();
	inicializarMemoriaBuddy();

	init_broker_server();
	return EXIT_SUCCESS;
}

void init_broker_server() {
	listener_socket = init_server(PUERTO_BROKER);
	log_info(logger, "Servidor levantado! Escuchando en %i",PUERTO_BROKER);
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

//				char* nombrePrueba = malloc(10);
				void*aux=message->content;
				new_pokemon_enviar mensaje;
				memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
				memcpy(mensaje.nombrePokemon,aux,mensaje.sizeNombre);
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


				if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
				{
					pthread_mutex_lock(&mutexMemoria);
					cachearMensaje(&mensaje,NEW);
					pthread_mutex_unlock(&mutexMemoria);
				}

				else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
				{
					pthread_mutex_lock(&mutexMemoria);
					cachearMensajeBuddy(&mensaje,NEW);
					pthread_mutex_unlock(&mutexMemoria);
				}

				free(mensaje.nombrePokemon);
				//QUEUE PUSH (POSICION MENSAJE)
//				log_info(logger,"%s", mensaje.nombrePokemon);



//				log_info(logger,"el size es: %d",mensaje.sizeNombre);
//				log_info(logger,"la cantidad es: %d",mensaje.cantidad);
//				log_info(logger,"la posicion x es: %d",mensaje.posicionEjeX);
//				log_info(logger,"la posicion y es: %d",mensaje.posicionEjeY);

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
				mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
				memcpy(mensaje.nombrePokemon,aux,mensaje.sizeNombre);
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
				if(buscarIdCorrelativo(idsCorrelativosAppeared,mensaje.idCorrelativo)==NULL){
					queue_push(appeared_admin.queue, &mensaje);
					uint32_t* idCorrelativoLista = malloc(sizeof(uint32_t));
					*idCorrelativoLista = mensaje.idCorrelativo;
					list_add(idsCorrelativosAppeared,idCorrelativoLista);
					log_info(logger,"mensaje agregado a la cola");
					if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
					{
						pthread_mutex_lock(&mutexMemoria);
						cachearMensaje(&mensaje,APPEARED);
						pthread_mutex_unlock(&mutexMemoria);
					}

					else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
					{
						pthread_mutex_lock(&mutexMemoria);
						cachearMensajeBuddy(&mensaje,APPEARED);
						pthread_mutex_unlock(&mutexMemoria);
					}
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
				mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
				memcpy(mensaje.nombrePokemon,aux,mensaje.sizeNombre);
				aux += mensaje.sizeNombre;
				log_info(logger,"NOMBRE CATCH: %s",mensaje.nombrePokemon);
				//log_info(logger,"",mensaje.)
				memcpy(&mensaje.posicionEjeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.posicionEjeY,aux,sizeof(uint32_t));

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);

				if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
				{
					pthread_mutex_lock(&mutexMemoria);
					cachearMensaje(&mensaje,CATCH);
					pthread_mutex_unlock(&mutexMemoria);
				}

				else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
				{
					pthread_mutex_lock(&mutexMemoria);
					cachearMensajeBuddy(&mensaje,CATCH);
					pthread_mutex_unlock(&mutexMemoria);
				}

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

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);



				pthread_mutex_lock(&mutexQueueCaught);
				if(buscarIdCorrelativo(idsCorrelativosCaught,mensaje.idCorrelativo)==NULL){
					queue_push(caught_admin.queue, &mensaje);
					uint32_t* idCorrelativoLista = malloc(sizeof(uint32_t));
					*idCorrelativoLista = mensaje.idCorrelativo;
					list_add(idsCorrelativosCaught,idCorrelativoLista);
					if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
					{
						pthread_mutex_lock(&mutexMemoria);
						cachearMensaje(&mensaje,CAUGHT);
						pthread_mutex_unlock(&mutexMemoria);
					}

					else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
					{
						pthread_mutex_lock(&mutexMemoria);
						cachearMensajeBuddy(&mensaje,CAUGHT);
						pthread_mutex_unlock(&mutexMemoria);
					}
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
				mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
				memcpy(mensaje.nombrePokemon,aux,mensaje.sizeNombre);

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);

				if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
				{
					pthread_mutex_lock(&mutexMemoria);
					cachearMensaje(&mensaje,GET);
					pthread_mutex_unlock(&mutexMemoria);
				}

				else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
				{
					pthread_mutex_lock(&mutexMemoria);
					cachearMensajeBuddy(&mensaje,GET);
					pthread_mutex_unlock(&mutexMemoria);
				}

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
				mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
				memcpy(mensaje.nombrePokemon,aux,mensaje.sizeNombre);
				aux += mensaje.sizeNombre;
				memcpy(&mensaje.cantidadPosiciones,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&largoLista,aux,sizeof(uint32_t)); //primero nos mandan el largo de la lista
				aux += sizeof(uint32_t);
				for(int i=0;i < largoLista;i++){
					memcpy(posicion,aux,sizeof(uint32_t));
					aux += sizeof(uint32_t);
					list_add(&mensaje.posiciones, &posicion);
				} //TODO CORREGIR
				aux += sizeof(uint32_t);
				memcpy(&mensaje.idCorrelativo,aux,sizeof(uint32_t));

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);




				pthread_mutex_lock(&mutexQueueLocalized);
				if(buscarIdCorrelativo(idsCorrelativosLocalized,mensaje.idCorrelativo)==NULL){
					queue_push(localized_admin.queue, &mensaje);
					uint32_t* idCorrelativoLista = malloc(sizeof(uint32_t));
					*idCorrelativoLista = mensaje.idCorrelativo;
					list_add(idsCorrelativosLocalized,idCorrelativoLista);
					if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
					{
						pthread_mutex_lock(&mutexMemoria);
						cachearMensaje(&mensaje,LOCALIZED);
						pthread_mutex_unlock(&mutexMemoria);
					}

					else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
					{
						pthread_mutex_lock(&mutexMemoria);
						cachearMensajeBuddy(&mensaje,LOCALIZED);
						pthread_mutex_unlock(&mutexMemoria);
					}
				}
				pthread_mutex_unlock(&mutexQueueLocalized);

				//enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				break;
			}
			case SUSCRIPCION:{
				suscripcion mensajeSuscripcion;
				void *aux = message->content;
				memcpy(&mensajeSuscripcion.idCola,aux,sizeof(uint32_t));
				aux +=sizeof(uint32_t);
				memcpy(&mensajeSuscripcion.idSuscriptor,aux,sizeof(pid_t));
				agregarSuscripcion(mensajeSuscripcion,broker_sock);

				enviarUltimosMensajesRecibidos(mensajeSuscripcion,broker_sock);

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

suscriptor* buscarSuscriptor(estructuraAdministrativa* estAdm,pid_t idSuscriptor){
	bool algo (void* suscriptorLista){
		suscriptor* suscriptorCasteado = suscriptorLista;
		return idSuscriptor == suscriptorCasteado->idSuscriptor;
	}

	return list_find(estAdm->suscriptores,algo);

}


void agregarSuscripcion (suscripcion mensajeSuscripcion, int broker_sock){
	switch(mensajeSuscripcion.idCola){
	case NEW:{
		suscriptor* suscriptorEncontrado;
		suscriptorEncontrado = buscarSuscriptor(new_admin,mensajeSuscripcion.idSuscriptor);
		if(suscriptorEncontrado == NULL)
		{
			suscriptor suscriptorNuevo;
			suscriptorNuevo.socket = broker_sock;
			suscriptorNuevo.idSuscriptor = mensajeSuscripcion.idSuscriptor;
			suscriptor* suscriptorNuevoCreado = crearSuscriptor(suscriptorNuevo);
			list_add(new_admin->suscriptores,suscriptorNuevoCreado);
			log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA NEW",broker_sock);
		}
		else{
			suscriptorEncontrado->socket = broker_sock;
		}

		break;
	}
	case APPEARED:{
		suscriptor* suscriptorEncontrado = buscarSuscriptor(appeared_admin,mensajeSuscripcion.idSuscriptor);
		if(suscriptorEncontrado == NULL)
		{
			suscriptor suscriptorNuevo;
			suscriptorNuevo.socket = broker_sock;
			suscriptorNuevo.idSuscriptor = mensajeSuscripcion.idSuscriptor;
			suscriptor* suscriptorNuevoCreado = crearSuscriptor(suscriptorNuevo);
			list_add(appeared_admin->suscriptores,suscriptorNuevoCreado);
			log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA NEW",broker_sock);
		}
		else{
			suscriptorEncontrado->socket = broker_sock;
		}

		break;
	}
	case GET:{
		suscriptor* suscriptorEncontrado = buscarSuscriptor(get_admin,mensajeSuscripcion.idSuscriptor);
		if(suscriptorEncontrado == NULL)
		{
			suscriptor suscriptorNuevo;
			suscriptorNuevo.socket = broker_sock;
			suscriptorNuevo.idSuscriptor = mensajeSuscripcion.idSuscriptor;
			suscriptor* suscriptorNuevoCreado = crearSuscriptor(suscriptorNuevo);
			list_add(get_admin->suscriptores,suscriptorNuevoCreado);
			log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA NEW",broker_sock);
		}
		else{
			suscriptorEncontrado->socket = broker_sock;
		}

		break;
	}

	case LOCALIZED:{
		suscriptor* suscriptorEncontrado = buscarSuscriptor(localized_admin,mensajeSuscripcion.idSuscriptor);
		if(suscriptorEncontrado == NULL)
		{
			suscriptor suscriptorNuevo;
			suscriptorNuevo.socket = broker_sock;
			suscriptorNuevo.idSuscriptor = mensajeSuscripcion.idSuscriptor;
			suscriptor* suscriptorNuevoCreado = crearSuscriptor(suscriptorNuevo);
			list_add(localized_admin->suscriptores,suscriptorNuevoCreado);
			log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA NEW",broker_sock);
		}
		else{
			suscriptorEncontrado->socket = broker_sock;
		}

		break;
	}
	case CATCH:{
		suscriptor* suscriptorEncontrado = buscarSuscriptor(catch_admin,mensajeSuscripcion.idSuscriptor);
		if(suscriptorEncontrado == NULL)
		{
			suscriptor suscriptorNuevo;
			suscriptorNuevo.socket = broker_sock;
			suscriptorNuevo.idSuscriptor = mensajeSuscripcion.idSuscriptor;
			suscriptor* suscriptorNuevoCreado = crearSuscriptor(suscriptorNuevo);
			list_add(catch_admin->suscriptores,suscriptorNuevoCreado);
			log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA NEW",broker_sock);
		}
		else{
			suscriptorEncontrado->socket = broker_sock;
		}

		break;
	}
	case CAUGHT:{
		suscriptor* suscriptorEncontrado = buscarSuscriptor(caught_admin,mensajeSuscripcion.idSuscriptor);
		if(suscriptorEncontrado == NULL)
		{
			suscriptor suscriptorNuevo;
			suscriptorNuevo.socket = broker_sock;
			suscriptorNuevo.idSuscriptor = mensajeSuscripcion.idSuscriptor;
			suscriptor* suscriptorNuevoCreado = crearSuscriptor(suscriptorNuevo);
			list_add(caught_admin->suscriptores,suscriptorNuevoCreado);
			log_info(logger, "SE AGREGO EL SUSCRIPTOR %d A LA COLA NEW",broker_sock);
		}
		else{
			suscriptorEncontrado->socket = broker_sock;
		}

		break;
	}
	default:
		break;

	}
}

void enviarUltimosMensajesRecibidos(suscripcion suscripcion,int socket){
	estructuraAdministrativa* estructuraAuxiliar;
	switch(suscripcion.idCola){
	case NEW:{
		if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
		{
			estructuraAuxiliar = new_admin;
			int sizeCola = list_size(estructuraAuxiliar->queue);
			int index = 0;
			while(index<sizeCola){
				uint32_t* idMensaje = list_get(estructuraAuxiliar->queue,index);
				particion_dinamica_memoria* particion = encontrarParticionDinamicaPorID(*idMensaje);
				if(particion != NULL){
					bool igualSuscriptor(void* suscriptorACK){
						suscriptor* suscriptorACKCasteado = suscriptorACK;
						return suscriptorACKCasteado->idSuscriptor == suscripcion.idSuscriptor;
					}
					suscriptor* suscriptorEncontrado = list_find(particion->suscriptoresACK,igualSuscriptor);
					if (suscriptorEncontrado == NULL){
						//ENVIAR
					}

				}
			}
		}

			else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS")){
				//IDEM ANTERIOR
			}

			//new_pokemon_enviar npe = buscarMensajeEnMemoria(idMensaje);
		}
		break;
	}

	case APPEARED:{
		estructuraAuxiliar = appeared_admin;
		break;
	}

	case GET:{
		estructuraAuxiliar = get_admin;
		break;
	}

	case LOCALIZED:{
		estructuraAuxiliar = localized_admin;
		break;
	}

	case CATCH:{
		estructuraAuxiliar = catch_admin;
		break;
	}

	case CAUGHT:{
		estructuraAuxiliar = caught_admin;
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

void* buscarIdCorrelativo(t_list* lista,uint32_t idCorrelativo){
	bool compararMensajesPorId(int* idLista){
		return *idLista == idCorrelativo;
	}
	return list_find(lista,(void*) compararMensajesPorId);
}

