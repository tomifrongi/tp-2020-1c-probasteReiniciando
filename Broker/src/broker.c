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

int main(void) {
	cargarConfiguracion();
	inicializarLogger(LOG_FILE); //logea ok!!
	ID_INICIAL = 0;


	crearEstructurasAdministrativas();
	iniciarMutexs();
	iniciarListasIds();
	if(string_equals_ignore_case(ALGORITMO_MEMORIA, "PARTICIONES"))
		inicializarMemoria();
	else if(string_equals_ignore_case(ALGORITMO_MEMORIA, "BS"))
		inicializarMemoriaBuddy();

	signal(SIGUSR1, imprimirEstadoActualMemoria);
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
				uint32_t idMenCola = mensaje.id_mensaje;

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

				int sizeCola = list_size(new_admin->suscriptores);
				int indice = 0;
				while(indice<sizeCola){
					t_message_envio* mensajeAEnviar = malloc(sizeof(t_message_envio));
					mensajeAEnviar->mensajeEnvio = malloc(sizeof(t_message));
					void* envio = malloc(message->size + sizeof(uint32_t));
					int desplazamientoEnvio = 0;
					memcpy(envio + desplazamientoEnvio,&mensaje.id_mensaje,sizeof(uint32_t));
					desplazamientoEnvio += sizeof(uint32_t);
					memcpy(envio + desplazamientoEnvio,message->content,message->size);
					mensajeAEnviar->mensajeEnvio->head = message->head;
					mensajeAEnviar->mensajeEnvio->content = envio;
					mensajeAEnviar->mensajeEnvio->size = message->size + sizeof(uint32_t);

					pthread_mutex_lock(&mutexSuscriptoresNew);
					suscriptor* suscriptorEnviar = list_get(new_admin->suscriptores,indice);
					mensajeAEnviar->socketEnvio = suscriptorEnviar->socket;
					mensajeAEnviar->idSuscriptor = suscriptorEnviar->idSuscriptor;
					pthread_mutex_unlock(&mutexSuscriptoresNew);

					pthread_t envioMensajes;
					pthread_create(&envioMensajes, NULL, handler_envio_mensajes,(void*) (mensajeAEnviar));
					pthread_detach(envioMensajes);

					bool mismoSocket(void* sus){
						suscriptor* suscriptorCasteado = sus;
						return suscriptorCasteado->socket == mensajeAEnviar->socketEnvio;
					}

					if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
					{
						pthread_mutex_lock(&mutexMemoria);
						particion_dinamica_memoria* particionMenEnv = encontrarParticionDinamicaPorID(mensaje.id_mensaje);
						pthread_mutex_lock(&mutexQueueNew);
						suscriptor* sub = list_find(new_admin->suscriptores,mismoSocket);
//						suscriptor subAux;
//						subAux.idSuscriptor = sub->idSuscriptor;
//						subAux.socket = sub->socket;
//						suscriptor* subAuxCreado = crearSuscriptor(subAux);
						list_add(particionMenEnv->suscriptoresMensajeEnviado,sub);
						pthread_mutex_unlock(&mutexQueueNew);
						pthread_mutex_unlock(&mutexMemoria);

					}

					else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
					{
						pthread_mutex_lock(&mutexMemoria);
						particion_buddy_memoria* particionMenEnv = encontrarParticionBuddyPorID(mensaje.id_mensaje);
						pthread_mutex_lock(&mutexQueueNew);
						suscriptor* sub = list_find(new_admin->suscriptores,mismoSocket);
						suscriptor subAux;
						subAux.idSuscriptor = sub->idSuscriptor;
						subAux.socket = sub->socket;
						suscriptor* subAuxCreado = crearSuscriptor(subAux);
						list_add(particionMenEnv->suscriptoresMensajeEnviado,subAuxCreado);
						pthread_mutex_unlock(&mutexQueueNew);
						pthread_mutex_unlock(&mutexMemoria);
					}

					indice++;
				}

				pthread_mutex_lock(&mutexQueueNew);
				uint32_t* idNuevo = crearElementoCola(idMenCola);
				list_add(new_admin->queue,idNuevo);
				pthread_mutex_unlock(&mutexQueueNew);

				enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				free(mensaje.nombrePokemon);
				break;
			}
			case APPEARED_POKEMON:{

				log_info(logger, "APPEARED POKEMON RECIBIDO");


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


				//TODO podriamos cambiar este mutex de aca
				pthread_mutex_lock(&mutexQueueAppeared);
				if(buscarIdCorrelativo(idsCorrelativosAppeared,mensaje.idCorrelativo)==NULL){


					uint32_t* idCorrelativoLista = malloc(sizeof(uint32_t));
					*idCorrelativoLista = mensaje.idCorrelativo;
					list_add(idsCorrelativosAppeared,idCorrelativoLista);
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
					uint32_t* idNuevo = crearElementoCola(mensaje.id_mensaje);
					list_add(appeared_admin->queue,idNuevo);

					int sizeCola = list_size(appeared_admin->suscriptores);
					int indice = 0;
					while(indice<sizeCola){
						t_message_envio* mensajeAEnviar = malloc(sizeof(t_message_envio));
						mensajeAEnviar->mensajeEnvio = malloc(sizeof(t_message));
						void* envio = malloc(message->size + sizeof(uint32_t));
						int desplazamientoEnvio = 0;
						memcpy(envio + desplazamientoEnvio,&mensaje.id_mensaje,sizeof(uint32_t));
						desplazamientoEnvio += sizeof(uint32_t);
						memcpy(envio + desplazamientoEnvio,message->content,message->size);
						mensajeAEnviar->mensajeEnvio->head = message->head;
						mensajeAEnviar->mensajeEnvio->content = envio;
						mensajeAEnviar->mensajeEnvio->size = message->size + sizeof(uint32_t);

						pthread_mutex_lock(&mutexSuscriptoresAppeared);
						suscriptor* suscriptorEnviar = list_get(appeared_admin->suscriptores,indice);
						mensajeAEnviar->socketEnvio = suscriptorEnviar->socket;
						mensajeAEnviar->idSuscriptor = suscriptorEnviar->idSuscriptor;
						pthread_mutex_unlock(&mutexSuscriptoresAppeared);

						pthread_t envioMensajes;
						pthread_create(&envioMensajes, NULL, handler_envio_mensajes,(void*) (mensajeAEnviar));
						pthread_detach(envioMensajes);

						bool mismoSocket(void* sus){
							suscriptor* suscriptorCasteado = sus;
							return suscriptorCasteado->socket == suscriptorEnviar->socket;
						}

						if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
						{
							pthread_mutex_lock(&mutexMemoria);
							particion_dinamica_memoria* particionMenEnv = encontrarParticionDinamicaPorID(mensaje.id_mensaje);
							suscriptor* sub = list_find(appeared_admin->suscriptores,mismoSocket);
							list_add(particionMenEnv->suscriptoresMensajeEnviado,sub);
							pthread_mutex_unlock(&mutexMemoria);

						}

						else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
						{
							pthread_mutex_lock(&mutexMemoria);
							particion_buddy_memoria* particionMenEnv = encontrarParticionBuddyPorID(mensaje.id_mensaje);
							suscriptor* sub = list_find(appeared_admin->suscriptores,mismoSocket);
							list_add(particionMenEnv->suscriptoresMensajeEnviado,sub);
							pthread_mutex_unlock(&mutexMemoria);
						}
						indice++;
					}

				}
				pthread_mutex_unlock(&mutexQueueAppeared);




				free(mensaje.nombrePokemon);

				enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				break;
			}
			case CATCH_POKEMON:{


				log_info(logger, "CATCH POKEMON RECIBIDO");


				void *aux = message->content;
				catch_pokemon_enviar mensaje;
				memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
				memcpy(mensaje.nombrePokemon,aux,mensaje.sizeNombre);
				aux += mensaje.sizeNombre;
				memcpy(&mensaje.posicionEjeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.posicionEjeY,aux,sizeof(uint32_t));

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);

				int sizeCola = list_size(catch_admin->suscriptores);


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

				int indice = 0;
				while(indice<sizeCola){
					t_message_envio* mensajeAEnviar = malloc(sizeof(t_message_envio));
					mensajeAEnviar->mensajeEnvio = malloc(sizeof(t_message));
					void* envio = malloc(message->size + sizeof(uint32_t));
					int desplazamientoEnvio = 0;
					memcpy(envio + desplazamientoEnvio,&mensaje.id_mensaje,sizeof(uint32_t));
					desplazamientoEnvio += sizeof(uint32_t);
					memcpy(envio + desplazamientoEnvio,message->content,message->size);
					mensajeAEnviar->mensajeEnvio->head = message->head;
					mensajeAEnviar->mensajeEnvio->content = envio;
					mensajeAEnviar->mensajeEnvio->size = message->size + sizeof(uint32_t);

					pthread_mutex_lock(&mutexSuscriptoresCatch);
					suscriptor* suscriptorEnvio = list_get(catch_admin->suscriptores,indice);
					mensajeAEnviar->socketEnvio = suscriptorEnvio->socket;
					mensajeAEnviar->idSuscriptor = suscriptorEnvio->idSuscriptor;
					pthread_mutex_unlock(&mutexSuscriptoresCatch);

					pthread_t envioMensajes;
					pthread_create(&envioMensajes, NULL, handler_envio_mensajes,(void*) (mensajeAEnviar));
					pthread_detach(envioMensajes);
					bool mismoSocket(void* sus){
						suscriptor* suscriptorCasteado = sus;
						return suscriptorCasteado->socket == suscriptorEnvio->socket;
					}

					if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
					{
						pthread_mutex_lock(&mutexMemoria);
						particion_dinamica_memoria* particionMenEnv = encontrarParticionDinamicaPorID(mensaje.id_mensaje);
						pthread_mutex_lock(&mutexQueueCatch);
						suscriptor* sub = list_find(catch_admin->suscriptores,mismoSocket);
						list_add(particionMenEnv->suscriptoresMensajeEnviado,sub);
						pthread_mutex_unlock(&mutexQueueCatch);
						pthread_mutex_unlock(&mutexMemoria);

					}

					else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
					{
						pthread_mutex_lock(&mutexMemoria);
						particion_buddy_memoria* particionMenEnv = encontrarParticionBuddyPorID(mensaje.id_mensaje);
						pthread_mutex_lock(&mutexQueueCatch);
						suscriptor* sub = list_find(catch_admin->suscriptores,mismoSocket);
						list_add(particionMenEnv->suscriptoresMensajeEnviado,sub);
						pthread_mutex_unlock(&mutexQueueCatch);
						pthread_mutex_unlock(&mutexMemoria);
					}
					indice++;
				}

				pthread_mutex_lock(&mutexQueueCatch);
				uint32_t* idNuevo = crearElementoCola(mensaje.id_mensaje);
				list_add(catch_admin->queue,idNuevo);
				pthread_mutex_unlock(&mutexQueueCatch);
				enviarConfirmacion(mensaje.id_mensaje,broker_sock);
				free(mensaje.nombrePokemon);
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

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);



				pthread_mutex_lock(&mutexQueueCaught);
				if(buscarIdCorrelativo(idsCorrelativosCaught,mensaje.idCorrelativo)==NULL){
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

					uint32_t* idNuevo = crearElementoCola(mensaje.id_mensaje);
					list_add(caught_admin->queue,idNuevo);
					int sizeCola = list_size(caught_admin->suscriptores);
					int indice = 0;
					while(indice<sizeCola){
						t_message_envio* mensajeAEnviar = malloc(sizeof(t_message_envio));
						mensajeAEnviar->mensajeEnvio = malloc(sizeof(t_message));
						void* envio = malloc(message->size + sizeof(uint32_t));
						int desplazamientoEnvio = 0;
						memcpy(envio + desplazamientoEnvio,&mensaje.id_mensaje,sizeof(uint32_t));
						desplazamientoEnvio += sizeof(uint32_t);
						memcpy(envio + desplazamientoEnvio,message->content,message->size);
						mensajeAEnviar->mensajeEnvio->head = message->head;
						mensajeAEnviar->mensajeEnvio->content = envio;
						mensajeAEnviar->mensajeEnvio->size = message->size + sizeof(uint32_t);

						pthread_mutex_lock(&mutexSuscriptoresCaught);
						suscriptor* suscriptorEnvio = list_get(caught_admin->suscriptores,indice);
						mensajeAEnviar->socketEnvio = suscriptorEnvio->socket;
						mensajeAEnviar->idSuscriptor = suscriptorEnvio->idSuscriptor;
						pthread_mutex_unlock(&mutexSuscriptoresCaught);

						pthread_t envioMensajes;
						pthread_create(&envioMensajes, NULL, handler_envio_mensajes,(void*) (mensajeAEnviar));
						pthread_detach(envioMensajes);

						bool mismoSocket(void* sus){
							suscriptor* suscriptorCasteado = sus;
							return suscriptorCasteado->socket == suscriptorEnvio->socket;;
						}

						if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
						{
							pthread_mutex_lock(&mutexMemoria);
							particion_dinamica_memoria* particionMenEnv = encontrarParticionDinamicaPorID(mensaje.id_mensaje);

							suscriptor* sub = list_find(caught_admin->suscriptores,mismoSocket);
							list_add(particionMenEnv->suscriptoresMensajeEnviado,sub);

							pthread_mutex_unlock(&mutexMemoria);

						}

						else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
						{
							pthread_mutex_lock(&mutexMemoria);
							particion_buddy_memoria* particionMenEnv = encontrarParticionBuddyPorID(mensaje.id_mensaje);

							suscriptor* sub = list_find(caught_admin->suscriptores,mismoSocket);
							list_add(particionMenEnv->suscriptoresMensajeEnviado,sub);

							pthread_mutex_unlock(&mutexMemoria);
						}
						indice++;
					}
				}
				pthread_mutex_unlock(&mutexQueueCaught);
				enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				break;
			}
			case GET_POKEMON:{


				log_info(logger, "GET POKEMON RECIBIDO");


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

				int sizeCola = list_size(get_admin->suscriptores);
				int indice = 0;
				while(indice<sizeCola){
					t_message_envio* mensajeAEnviar = malloc(sizeof(t_message_envio));
					mensajeAEnviar->mensajeEnvio = malloc(sizeof(t_message));
					void* envio = malloc(message->size + sizeof(uint32_t));
					int desplazamientoEnvio = 0;
					memcpy(envio + desplazamientoEnvio,&mensaje.id_mensaje,sizeof(uint32_t));
					desplazamientoEnvio += sizeof(uint32_t);
					memcpy(envio + desplazamientoEnvio,message->content,message->size);
					mensajeAEnviar->mensajeEnvio->head = message->head;
					mensajeAEnviar->mensajeEnvio->content = envio;
					mensajeAEnviar->mensajeEnvio->size = message->size + sizeof(uint32_t);

					pthread_mutex_lock(&mutexSuscriptoresGet);
					suscriptor* suscriptorEnviar = list_get(get_admin->suscriptores,indice);
					mensajeAEnviar->socketEnvio = suscriptorEnviar->socket;
					mensajeAEnviar->idSuscriptor = suscriptorEnviar->idSuscriptor;
					pthread_mutex_unlock(&mutexSuscriptoresGet);

					pthread_t envioMensajes;
					pthread_create(&envioMensajes, NULL, handler_envio_mensajes,(void*) (mensajeAEnviar));
					pthread_detach(envioMensajes);
					bool mismoSocket(void* sus){
						suscriptor* suscriptorCasteado = sus;
						return suscriptorCasteado->socket == suscriptorEnviar->socket;;
					}

					if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
					{
						pthread_mutex_lock(&mutexMemoria);
						particion_dinamica_memoria* particionMenEnv = encontrarParticionDinamicaPorID(mensaje.id_mensaje);
						pthread_mutex_lock(&mutexQueueGet);
						suscriptor* sub = list_find(get_admin->suscriptores,mismoSocket);
						list_add(particionMenEnv->suscriptoresMensajeEnviado,sub);
						pthread_mutex_unlock(&mutexQueueGet);
						pthread_mutex_unlock(&mutexMemoria);

					}

					else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
					{
						pthread_mutex_lock(&mutexMemoria);
						particion_buddy_memoria* particionMenEnv = encontrarParticionBuddyPorID(mensaje.id_mensaje);
						pthread_mutex_lock(&mutexQueueGet);
						suscriptor* sub = list_find(get_admin->suscriptores,mismoSocket);
						list_add(particionMenEnv->suscriptoresMensajeEnviado,sub);
						pthread_mutex_unlock(&mutexQueueGet);
						pthread_mutex_unlock(&mutexMemoria);
					}
					indice++;
				}

				pthread_mutex_lock(&mutexQueueGet);
				uint32_t* idNuevo = crearElementoCola(mensaje.id_mensaje);
				list_add(get_admin->queue,idNuevo);
				pthread_mutex_unlock(&mutexQueueGet);
				free(mensaje.nombrePokemon);
				enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				break;
			}
			case LOCALIZED_POKEMON:{


				log_info(logger, "LOCALIZED POKEMON RECIBIDO");


				void *aux = message->content;
				localized_pokemon_enviar mensaje;

				memcpy(&mensaje.idCorrelativo,aux,sizeof(uint32_t));
				aux+=sizeof(uint32_t);
				memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
				memcpy(mensaje.nombrePokemon,aux,mensaje.sizeNombre);
				aux += mensaje.sizeNombre;
				memcpy(&mensaje.cantidadPosiciones,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				size_t tamanioLista = sizeof(uint32_t)*2*mensaje.cantidadPosiciones;
				mensaje.posiciones = malloc(tamanioLista);
				memcpy(mensaje.posiciones,aux,tamanioLista);

				pthread_mutex_lock(&mutexId);
				mensaje.id_mensaje = ID_INICIAL;
				ID_INICIAL ++;
				pthread_mutex_unlock(&mutexId);



				pthread_mutex_lock(&mutexQueueLocalized);
				if(buscarIdCorrelativo(idsCorrelativosLocalized,mensaje.idCorrelativo)==NULL){
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
					uint32_t* idNuevo = crearElementoCola(mensaje.id_mensaje);
					list_add(localized_admin->queue,idNuevo);
					int sizeCola = list_size(localized_admin->suscriptores);
					int indice = 0;
					while(indice<sizeCola){
						t_message_envio* mensajeAEnviar = malloc(sizeof(t_message_envio));
						mensajeAEnviar->mensajeEnvio = malloc(sizeof(t_message));
						void* envio = malloc(message->size + sizeof(uint32_t));
						int desplazamientoEnvio = 0;
						memcpy(envio + desplazamientoEnvio,&mensaje.id_mensaje,sizeof(uint32_t));
						desplazamientoEnvio += sizeof(uint32_t);
						memcpy(envio + desplazamientoEnvio,message->content,message->size);
						mensajeAEnviar->mensajeEnvio->head = message->head;
						mensajeAEnviar->mensajeEnvio->content = envio;
						mensajeAEnviar->mensajeEnvio->size = message->size + sizeof(uint32_t);

						pthread_mutex_lock(&mutexSuscriptoresLocalized);
						suscriptor* suscriptorEnviar = list_get(localized_admin->suscriptores,indice);

						mensajeAEnviar->socketEnvio = suscriptorEnviar->socket;
						mensajeAEnviar->idSuscriptor = suscriptorEnviar->idSuscriptor;
						pthread_mutex_unlock(&mutexSuscriptoresLocalized);

						pthread_t envioMensajes;
						pthread_create(&envioMensajes, NULL, handler_envio_mensajes,(void*) (mensajeAEnviar));
						pthread_detach(envioMensajes);
						bool mismoSocket(void* sus){
							suscriptor* suscriptorCasteado = sus;
							return suscriptorCasteado->socket == suscriptorEnviar->socket;;
						}

						if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
						{
							pthread_mutex_lock(&mutexMemoria);
							particion_dinamica_memoria* particionMenEnv = encontrarParticionDinamicaPorID(mensaje.id_mensaje);

							suscriptor* sub = list_find(localized_admin->suscriptores,mismoSocket);
							list_add(particionMenEnv->suscriptoresMensajeEnviado,sub);

							pthread_mutex_unlock(&mutexMemoria);

						}

						else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
						{
							pthread_mutex_lock(&mutexMemoria);
							particion_buddy_memoria* particionMenEnv = encontrarParticionBuddyPorID(mensaje.id_mensaje);

							suscriptor* sub = list_find(localized_admin->suscriptores,mismoSocket);
							list_add(particionMenEnv->suscriptoresMensajeEnviado,sub);

							pthread_mutex_unlock(&mutexMemoria);
						}
						indice++;
					}
				}
				pthread_mutex_unlock(&mutexQueueLocalized);
				free(mensaje.nombrePokemon);
				free(mensaje.posiciones);

				enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				break;
			}
			case SUSCRIPCION:{
				suscripcion mensajeSuscripcion;
				void *aux = message->content;
				memcpy(&mensajeSuscripcion.idCola,aux,sizeof(uint32_t));
				aux +=sizeof(uint32_t);
				memcpy(&mensajeSuscripcion.idSuscriptor,aux,sizeof(pid_t));
				char* nombreCola = obtenerNombreCola(mensajeSuscripcion.idCola);
				log_info(logger,"NUEVA SUSCRIPCION DEL PROCESO %d a la cola %s",mensajeSuscripcion.idSuscriptor,nombreCola);
				agregarSuscripcion(mensajeSuscripcion,broker_sock);
				pthread_mutex_lock(&mutexMemoria);
				enviarUltimosMensajesRecibidos(mensajeSuscripcion,broker_sock);
				pthread_mutex_unlock(&mutexMemoria);

				break;
			}

			case CONFIRMACION:{
				mensajeACK mensajeConf;
				void *aux = message->content;
				memcpy(&mensajeConf.id_mensaje,aux,sizeof(uint32_t));
				aux +=sizeof(uint32_t);
				memcpy(&mensajeConf.idSuscriptor,aux,sizeof(pid_t));
				log_info(logger,"ACK RECIBIDO DEL SUSCRIPTOR %d REFERENCIANDO AL MENSAJE %d",mensajeConf.idSuscriptor,mensajeConf.id_mensaje);

				pthread_mutex_lock(&mutexMemoria);
				if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES")){
					particion_dinamica_memoria* particionConfirmacion = encontrarParticionDinamicaPorID(mensajeConf.id_mensaje);
					if(particionConfirmacion != NULL){
						bool igualSuscriptor(void* suscriptorMensajeEnviado){
							suscriptor* suscriptorMensajeEnviadoCasteado = suscriptorMensajeEnviado;
							return suscriptorMensajeEnviadoCasteado->idSuscriptor == mensajeConf.idSuscriptor;
						}
						suscriptor* suscriptorEncontrado = list_find(particionConfirmacion->suscriptoresMensajeEnviado,igualSuscriptor);
						if(suscriptorEncontrado != NULL)
							list_add(particionConfirmacion->suscriptoresACK,suscriptorEncontrado);

					}

				}
				else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS")){
					particion_buddy_memoria* particionConfirmacion = encontrarParticionBuddyPorID(mensajeConf.id_mensaje);
					if(particionConfirmacion != NULL){
						bool igualSuscriptor(void* suscriptorMensajeEnviado){
							suscriptor* suscriptorMensajeEnviadoCasteado = suscriptorMensajeEnviado;
							return suscriptorMensajeEnviadoCasteado->idSuscriptor == mensajeConf.idSuscriptor;
						}
						suscriptor* suscriptorEncontrado = list_find(particionConfirmacion->suscriptoresMensajeEnviado,igualSuscriptor);
						if(suscriptorEncontrado != NULL)
							list_add(particionConfirmacion->suscriptoresACK,suscriptorEncontrado);
					}

				}
				pthread_mutex_unlock(&mutexMemoria);

				break;
			}

			case NO_CONNECTION:
				//log_info(logger, "CLIENTE DESCONECTADO");

				free_t_message(message);
				pthread_exit(NULL);
				return NULL;
				break;
			case ERROR_RECV:
				free_t_message(message);
				//log_info(logger, "ERROR COMUNICACIÓN");
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

void* handler_envio_mensajes(void* mensajeAEnviar){
	t_message_envio* mensaje = (t_message_envio*) mensajeAEnviar;

	int resultadoEnvio = send_message(mensaje->socketEnvio, mensaje->mensajeEnvio->head,mensaje->mensajeEnvio->content,mensaje->mensajeEnvio->size);

	if(resultadoEnvio != (-errno)){
		char* nombreCola = obtenerNombreColaHeader(mensaje->mensajeEnvio->head);
		void* contenido = mensaje->mensajeEnvio->content;
		int idLog;
		memcpy(&idLog,contenido,sizeof(uint32_t));
		log_info(logger,"MENSAJE %s CON ID %d ENVIADO AL SUSCRIPTOR %d ",nombreCola,idLog,mensaje->idSuscriptor);

	}
	free(mensaje->mensajeEnvio->content);
	free(mensaje->mensajeEnvio);
	free(mensaje);
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
		pthread_mutex_lock(&mutexSuscriptoresNew);
		suscriptorEncontrado = buscarSuscriptor(new_admin,mensajeSuscripcion.idSuscriptor);
		if(suscriptorEncontrado == NULL)
		{
			suscriptor suscriptorNuevo;
			suscriptorNuevo.socket = broker_sock;
			suscriptorNuevo.idSuscriptor = mensajeSuscripcion.idSuscriptor;
			suscriptor* suscriptorNuevoCreado = crearSuscriptor(suscriptorNuevo);
			list_add(new_admin->suscriptores,suscriptorNuevoCreado);
		}
		else{
			suscriptorEncontrado->socket = broker_sock;
		}
		pthread_mutex_unlock(&mutexSuscriptoresNew);
		break;
	}
	case APPEARED:{
		pthread_mutex_lock(&mutexSuscriptoresAppeared);
		suscriptor* suscriptorEncontrado = buscarSuscriptor(appeared_admin,mensajeSuscripcion.idSuscriptor);
		if(suscriptorEncontrado == NULL)
		{
			suscriptor suscriptorNuevo;
			suscriptorNuevo.socket = broker_sock;
			suscriptorNuevo.idSuscriptor = mensajeSuscripcion.idSuscriptor;
			suscriptor* suscriptorNuevoCreado = crearSuscriptor(suscriptorNuevo);
			list_add(appeared_admin->suscriptores,suscriptorNuevoCreado);
		}
		else{
			suscriptorEncontrado->socket = broker_sock;
		}
		pthread_mutex_unlock(&mutexSuscriptoresAppeared);
		break;
	}
	case GET:{
		pthread_mutex_lock(&mutexSuscriptoresGet);
		suscriptor* suscriptorEncontrado = buscarSuscriptor(get_admin,mensajeSuscripcion.idSuscriptor);
		if(suscriptorEncontrado == NULL)
		{
			suscriptor suscriptorNuevo;
			suscriptorNuevo.socket = broker_sock;
			suscriptorNuevo.idSuscriptor = mensajeSuscripcion.idSuscriptor;
			suscriptor* suscriptorNuevoCreado = crearSuscriptor(suscriptorNuevo);
			list_add(get_admin->suscriptores,suscriptorNuevoCreado);
		}
		else{
			suscriptorEncontrado->socket = broker_sock;
		}
		pthread_mutex_unlock(&mutexSuscriptoresGet);
		break;
	}

	case LOCALIZED:{
		pthread_mutex_lock(&mutexSuscriptoresLocalized);
		suscriptor* suscriptorEncontrado = buscarSuscriptor(localized_admin,mensajeSuscripcion.idSuscriptor);
		if(suscriptorEncontrado == NULL)
		{
			suscriptor suscriptorNuevo;
			suscriptorNuevo.socket = broker_sock;
			suscriptorNuevo.idSuscriptor = mensajeSuscripcion.idSuscriptor;
			suscriptor* suscriptorNuevoCreado = crearSuscriptor(suscriptorNuevo);
			list_add(localized_admin->suscriptores,suscriptorNuevoCreado);
		}
		else{
			suscriptorEncontrado->socket = broker_sock;
		}
		pthread_mutex_unlock(&mutexSuscriptoresLocalized);
		break;
	}
	case CATCH:{
		pthread_mutex_lock(&mutexSuscriptoresCatch);
		suscriptor* suscriptorEncontrado = buscarSuscriptor(catch_admin,mensajeSuscripcion.idSuscriptor);
		if(suscriptorEncontrado == NULL)
		{
			suscriptor suscriptorNuevo;
			suscriptorNuevo.socket = broker_sock;
			suscriptorNuevo.idSuscriptor = mensajeSuscripcion.idSuscriptor;
			suscriptor* suscriptorNuevoCreado = crearSuscriptor(suscriptorNuevo);
			list_add(catch_admin->suscriptores,suscriptorNuevoCreado);
		}
		else{
			suscriptorEncontrado->socket = broker_sock;
		}
		pthread_mutex_unlock(&mutexSuscriptoresCatch);
		break;
	}
	case CAUGHT:{
		pthread_mutex_lock(&mutexSuscriptoresCaught);
		suscriptor* suscriptorEncontrado = buscarSuscriptor(caught_admin,mensajeSuscripcion.idSuscriptor);
		if(suscriptorEncontrado == NULL)
		{
			suscriptor suscriptorNuevo;
			suscriptorNuevo.socket = broker_sock;
			suscriptorNuevo.idSuscriptor = mensajeSuscripcion.idSuscriptor;
			suscriptor* suscriptorNuevoCreado = crearSuscriptor(suscriptorNuevo);
			list_add(caught_admin->suscriptores,suscriptorNuevoCreado);
		}
		else{
			suscriptorEncontrado->socket = broker_sock;
		}
		pthread_mutex_unlock(&mutexSuscriptoresCaught);
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
		pthread_mutex_lock(&mutexSuscriptoresNew);
		pthread_mutex_lock(&mutexQueueNew);
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
						new_pokemon_enviar npEnviar;
						size_t sizeNew = sizeof(npEnviar.id_mensaje) + particion->tamanioMensaje + 1;//+1 porque agrego el barra cero
						void* content = malloc(sizeNew);
						int bytesEscritos = 0;
						int bytesLeidosMemoria = particion->posicionParticion;

						memcpy(content+bytesEscritos,idMensaje,sizeof(npEnviar.id_mensaje));
						bytesEscritos+= sizeof(npEnviar.id_mensaje);
						memcpy(&npEnviar.sizeNombre,principioMemoria+bytesLeidosMemoria,sizeof(npEnviar.sizeNombre));
						npEnviar.sizeNombre++;
						memcpy(content+bytesEscritos,&npEnviar.sizeNombre,sizeof(npEnviar.sizeNombre));
						bytesEscritos+=sizeof(npEnviar.sizeNombre);
						bytesLeidosMemoria+=sizeof(npEnviar.sizeNombre);
						memcpy(content+bytesEscritos,principioMemoria+bytesLeidosMemoria,(npEnviar.sizeNombre-1));
						bytesEscritos+=(npEnviar.sizeNombre-1);
						bytesLeidosMemoria+=(npEnviar.sizeNombre-1);
						char caracterNulo= '\0';
						memcpy(content+bytesEscritos,&caracterNulo,sizeof(caracterNulo));
						bytesEscritos+=sizeof(caracterNulo);
						memcpy(content+bytesEscritos,principioMemoria+bytesLeidosMemoria,3*sizeof(uint32_t));
						CONTADORLRU++;
						particion->contadorLRU = CONTADORLRU;

						send_message(socket, NEW_POKEMON,content,sizeNew);
						log_info(logger,"MENSAJE NEW CON ID %d ENVIADO AL SUSCRIPTOR %d",*idMensaje ,suscripcion.idSuscriptor);
						suscriptor* suscriptorEncontradoEnviado = list_find(particion->suscriptoresMensajeEnviado,igualSuscriptor);
						if(suscriptorEncontradoEnviado == NULL){
							suscriptor* suscriptorEncontradoCola = list_find(new_admin->suscriptores,igualSuscriptor);
//							suscriptor subAux;
//							subAux.idSuscriptor = suscriptorEncontradoCola->idSuscriptor;
//							subAux.socket = suscriptorEncontradoEnviado->socket;
//							suscriptor* subAuxCreado = crearSuscriptor(subAux);
							list_add(particion->suscriptoresMensajeEnviado,suscriptorEncontradoCola);
						}
						free(content);

					}
				}
				index++;
			}
		}

			else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
			{
				estructuraAuxiliar = new_admin;
				int sizeCola = list_size(estructuraAuxiliar->queue);
				int index = 0;
				while(index<sizeCola){
					uint32_t* idMensaje = list_get(estructuraAuxiliar->queue,index);
					particion_buddy_memoria* particion = encontrarParticionBuddyPorID(*idMensaje);

					if(particion != NULL){
						bool igualSuscriptor(void* suscriptorACK){
							suscriptor* suscriptorACKCasteado = suscriptorACK;
							return suscriptorACKCasteado->idSuscriptor == suscripcion.idSuscriptor;
						}
						suscriptor* suscriptorEncontrado = list_find(particion->suscriptoresACK,igualSuscriptor);

						if (suscriptorEncontrado == NULL){
							//ENVIAR
							new_pokemon_enviar npEnviar;
							size_t sizeNew = sizeof(npEnviar.id_mensaje) + particion->tamanioMensaje + 1;//+1 porque agrego el barra cero
							void* content = malloc(sizeNew);
							int bytesEscritos = 0;
							int bytesLeidosMemoria = particion->posicionParticion;

							memcpy(content+bytesEscritos,idMensaje,sizeof(npEnviar.id_mensaje));
							bytesEscritos+= sizeof(npEnviar.id_mensaje);
							memcpy(&npEnviar.sizeNombre,principioMemoriaBuddy+bytesLeidosMemoria,sizeof(npEnviar.sizeNombre));
							npEnviar.sizeNombre++;
							memcpy(content+bytesEscritos,&npEnviar.sizeNombre,sizeof(npEnviar.sizeNombre));
							bytesEscritos+=sizeof(npEnviar.sizeNombre);
							bytesLeidosMemoria+=sizeof(npEnviar.sizeNombre);
							memcpy(content+bytesEscritos,principioMemoriaBuddy+bytesLeidosMemoria,(npEnviar.sizeNombre-1));
							bytesEscritos+=(npEnviar.sizeNombre-1);
							bytesLeidosMemoria+=(npEnviar.sizeNombre-1);
							char caracterNulo= '\0';
							memcpy(content+bytesEscritos,&caracterNulo,sizeof(caracterNulo));
							bytesEscritos+=sizeof(caracterNulo);
							memcpy(content+bytesEscritos,principioMemoriaBuddy+bytesLeidosMemoria,3*sizeof(uint32_t));
							CONTADORLRUBUDDY++;
							particion->contadorLRU = CONTADORLRUBUDDY;

							send_message(socket, NEW_POKEMON,content,sizeNew);
							log_info(logger,"MENSAJE NEW CON ID %d ENVIADO AL SUSCRIPTOR %d",*idMensaje ,suscripcion.idSuscriptor);
							suscriptor* suscriptorEncontradoEnviado = list_find(particion->suscriptoresMensajeEnviado,igualSuscriptor);
							if(suscriptorEncontradoEnviado == NULL){
								suscriptor* suscriptorEncontradoCola = list_find(new_admin->suscriptores,igualSuscriptor);
								list_add(particion->suscriptoresMensajeEnviado,suscriptorEncontradoCola);
							}
							free(content);
						}

					}
					index++;
				}
			}
		pthread_mutex_unlock(&mutexSuscriptoresNew);
		pthread_mutex_unlock(&mutexQueueNew);
		break;
		}

	case APPEARED:{
		pthread_mutex_lock(&mutexSuscriptoresAppeared);
		pthread_mutex_lock(&mutexQueueAppeared);
		if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
		{
			estructuraAuxiliar = appeared_admin;
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
						appeared_pokemon_enviar apEnviar;
						size_t sizeAppeared = sizeof(apEnviar.id_mensaje)+sizeof(apEnviar.idCorrelativo) + particion->tamanioMensaje + 1;//+1 porque agrego el barra cero
						void* content = malloc(sizeAppeared);
						int bytesEscritos = 0;
						int bytesLeidosMemoria = particion->posicionParticion;

						memcpy(content+bytesEscritos,idMensaje,sizeof(apEnviar.id_mensaje));
						bytesEscritos+= sizeof(apEnviar.id_mensaje);
						apEnviar.idCorrelativo = particion->idCorrelativo;
						memcpy(content+bytesEscritos,&apEnviar.idCorrelativo,sizeof(apEnviar.idCorrelativo));
						bytesEscritos+= sizeof(apEnviar.idCorrelativo);
						memcpy(&apEnviar.sizeNombre,principioMemoria+bytesLeidosMemoria,sizeof(apEnviar.sizeNombre));
						apEnviar.sizeNombre++;
						memcpy(content+bytesEscritos,&apEnviar.sizeNombre,sizeof(apEnviar.sizeNombre));
						bytesEscritos+=sizeof(apEnviar.sizeNombre);
						bytesLeidosMemoria+=sizeof(apEnviar.sizeNombre);
						memcpy(content+bytesEscritos,principioMemoria+bytesLeidosMemoria,(apEnviar.sizeNombre-1));
						bytesEscritos+=(apEnviar.sizeNombre-1);
						bytesLeidosMemoria+=(apEnviar.sizeNombre-1);
						char caracterNulo= '\0';
						memcpy(content+bytesEscritos,&caracterNulo,sizeof(caracterNulo));
						bytesEscritos+=sizeof(caracterNulo);
						memcpy(content+bytesEscritos,principioMemoria+bytesLeidosMemoria,2*sizeof(uint32_t));
						CONTADORLRU++;
						particion->contadorLRU = CONTADORLRU;


						send_message(socket, APPEARED_POKEMON,content,sizeAppeared);
						log_info(logger,"MENSAJE APPEARED CON ID %d ENVIADO AL SUSCRIPTOR %d",*idMensaje ,suscripcion.idSuscriptor);
						suscriptor* suscriptorEncontradoEnviado = list_find(particion->suscriptoresMensajeEnviado,igualSuscriptor);
						if(suscriptorEncontradoEnviado == NULL){
							suscriptor* suscriptorEncontradoCola = list_find(appeared_admin->suscriptores,igualSuscriptor);
							list_add(particion->suscriptoresMensajeEnviado,suscriptorEncontradoCola);
						}
						free(content);

					}

				}
				index++;
			}
		}

			else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
			{
				estructuraAuxiliar = appeared_admin;
				int sizeCola = list_size(estructuraAuxiliar->queue);
				int index = 0;
				while(index<sizeCola){
					uint32_t* idMensaje = list_get(estructuraAuxiliar->queue,index);
					particion_buddy_memoria* particion = encontrarParticionBuddyPorID(*idMensaje);

					if(particion != NULL){
						bool igualSuscriptor(void* suscriptorACK){
							suscriptor* suscriptorACKCasteado = suscriptorACK;
							return suscriptorACKCasteado->idSuscriptor == suscripcion.idSuscriptor;
						}
						suscriptor* suscriptorEncontrado = list_find(particion->suscriptoresACK,igualSuscriptor);

						if (suscriptorEncontrado == NULL){
							//ENVIAR
							appeared_pokemon_enviar apEnviar;
							size_t sizeAppeared = sizeof(apEnviar.id_mensaje)+sizeof(apEnviar.idCorrelativo) + particion->tamanioMensaje + 1;//+1 porque agrego el barra cero
							void* content = malloc(sizeAppeared);
							int bytesEscritos = 0;
							int bytesLeidosMemoria = particion->posicionParticion;

							memcpy(content+bytesEscritos,idMensaje,sizeof(apEnviar.id_mensaje));
							bytesEscritos+= sizeof(apEnviar.id_mensaje);
							apEnviar.idCorrelativo = particion->idCorrelativo;
							memcpy(content+bytesEscritos,&apEnviar.idCorrelativo,sizeof(apEnviar.idCorrelativo));
							bytesEscritos+= sizeof(apEnviar.idCorrelativo);
							memcpy(&apEnviar.sizeNombre,principioMemoriaBuddy+bytesLeidosMemoria,sizeof(apEnviar.sizeNombre));
							apEnviar.sizeNombre++;
							memcpy(content+bytesEscritos,&apEnviar.sizeNombre,sizeof(apEnviar.sizeNombre));
							bytesEscritos+=sizeof(apEnviar.sizeNombre);
							bytesLeidosMemoria+=sizeof(apEnviar.sizeNombre);
							memcpy(content+bytesEscritos,principioMemoriaBuddy+bytesLeidosMemoria,(apEnviar.sizeNombre-1));
							bytesEscritos+=(apEnviar.sizeNombre-1);
							bytesLeidosMemoria+=(apEnviar.sizeNombre-1);
							char caracterNulo= '\0';
							memcpy(content+bytesEscritos,&caracterNulo,sizeof(caracterNulo));
							bytesEscritos+=sizeof(caracterNulo);
							memcpy(content+bytesEscritos,principioMemoriaBuddy+bytesLeidosMemoria,2*sizeof(uint32_t));
							CONTADORLRUBUDDY++;
							particion->contadorLRU = CONTADORLRUBUDDY;
							send_message(socket, APPEARED_POKEMON,content,sizeAppeared);
							log_info(logger,"MENSAJE APPEARED CON ID %d ENVIADO AL SUSCRIPTOR %d",*idMensaje ,suscripcion.idSuscriptor);
							suscriptor* suscriptorEncontradoEnviado = list_find(particion->suscriptoresMensajeEnviado,igualSuscriptor);
							if(suscriptorEncontradoEnviado == NULL){
								suscriptor* suscriptorEncontradoCola = list_find(appeared_admin->suscriptores,igualSuscriptor);
								list_add(particion->suscriptoresMensajeEnviado,suscriptorEncontradoCola);
							}
							free(content);
						}

					}
					index++;
				}
			}
		pthread_mutex_unlock(&mutexSuscriptoresAppeared);
		pthread_mutex_unlock(&mutexQueueAppeared);
		break;
		}

	case GET:{
		pthread_mutex_lock(&mutexSuscriptoresGet);
		pthread_mutex_lock(&mutexQueueGet);
		if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
		{
			estructuraAuxiliar = get_admin;
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
						get_pokemon_enviar gpEnviar;
						size_t sizeGet = sizeof(gpEnviar.id_mensaje) + particion->tamanioMensaje + 1;//+1 porque agrego el barra cero
						void* content = malloc(sizeGet);
						int bytesEscritos = 0;
						int bytesLeidosMemoria = particion->posicionParticion;

						memcpy(content+bytesEscritos,idMensaje,sizeof(gpEnviar.id_mensaje));
						bytesEscritos+= sizeof(gpEnviar.id_mensaje);
						memcpy(&gpEnviar.sizeNombre,principioMemoria+bytesLeidosMemoria,sizeof(gpEnviar.sizeNombre));
						gpEnviar.sizeNombre++;
						memcpy(content+bytesEscritos,&gpEnviar.sizeNombre,sizeof(gpEnviar.sizeNombre));
						bytesEscritos+=sizeof(gpEnviar.sizeNombre);
						bytesLeidosMemoria+=sizeof(gpEnviar.sizeNombre);
						memcpy(content+bytesEscritos,principioMemoria+bytesLeidosMemoria,(gpEnviar.sizeNombre-1));
						bytesEscritos+=(gpEnviar.sizeNombre-1);
						bytesLeidosMemoria+=(gpEnviar.sizeNombre-1);
						char caracterNulo= '\0';
						memcpy(content+bytesEscritos,&caracterNulo,sizeof(caracterNulo));
						bytesEscritos+=sizeof(caracterNulo);
						CONTADORLRU++;
						particion->contadorLRU = CONTADORLRU;

						send_message(socket, GET_POKEMON,content,sizeGet);
						log_info(logger,"MENSAJE GET CON ID %d ENVIADO AL SUSCRIPTOR %d",*idMensaje ,suscripcion.idSuscriptor);
						suscriptor* suscriptorEncontradoEnviado = list_find(particion->suscriptoresMensajeEnviado,igualSuscriptor);
						if(suscriptorEncontradoEnviado == NULL){
							suscriptor* suscriptorEncontradoCola = list_find(get_admin->suscriptores,igualSuscriptor);
							list_add(particion->suscriptoresMensajeEnviado,suscriptorEncontradoCola);
						}
						free(content);

					}

				}
				index++;
			}
		}

			else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
			{
				estructuraAuxiliar = get_admin;
				int sizeCola = list_size(estructuraAuxiliar->queue);
				int index = 0;
				while(index<sizeCola){
					uint32_t* idMensaje = list_get(estructuraAuxiliar->queue,index);
					particion_buddy_memoria* particion = encontrarParticionBuddyPorID(*idMensaje);

					if(particion != NULL){
						bool igualSuscriptor(void* suscriptorACK){
							suscriptor* suscriptorACKCasteado = suscriptorACK;
							return suscriptorACKCasteado->idSuscriptor == suscripcion.idSuscriptor;
						}
						suscriptor* suscriptorEncontrado = list_find(particion->suscriptoresACK,igualSuscriptor);

						if (suscriptorEncontrado == NULL){
							//ENVIAR
							get_pokemon_enviar gpEnviar;
							size_t sizeGet = sizeof(gpEnviar.id_mensaje) + particion->tamanioMensaje + 1;//+1 porque agrego el barra cero
							void* content = malloc(sizeGet);
							int bytesEscritos = 0;
							int bytesLeidosMemoria = particion->posicionParticion;

							memcpy(content+bytesEscritos,idMensaje,sizeof(gpEnviar.id_mensaje));
							bytesEscritos+= sizeof(gpEnviar.id_mensaje);
							memcpy(&gpEnviar.sizeNombre,principioMemoriaBuddy+bytesLeidosMemoria,sizeof(gpEnviar.sizeNombre));
							gpEnviar.sizeNombre++;
							memcpy(content+bytesEscritos,&gpEnviar.sizeNombre,sizeof(gpEnviar.sizeNombre));
							bytesEscritos+=sizeof(gpEnviar.sizeNombre);
							bytesLeidosMemoria+=sizeof(gpEnviar.sizeNombre);
							memcpy(content+bytesEscritos,principioMemoriaBuddy+bytesLeidosMemoria,(gpEnviar.sizeNombre-1));
							bytesEscritos+=(gpEnviar.sizeNombre-1);
							bytesLeidosMemoria+=(gpEnviar.sizeNombre-1);
							char caracterNulo= '\0';
							memcpy(content+bytesEscritos,&caracterNulo,sizeof(caracterNulo));
							bytesEscritos+=sizeof(caracterNulo);
							CONTADORLRUBUDDY++;
							particion->contadorLRU = CONTADORLRUBUDDY;
							send_message(socket, GET_POKEMON,content,sizeGet);
							log_info(logger,"MENSAJE GET CON ID %d ENVIADO AL SUSCRIPTOR %d",*idMensaje ,suscripcion.idSuscriptor);
							suscriptor* suscriptorEncontradoEnviado = list_find(particion->suscriptoresMensajeEnviado,igualSuscriptor);
							if(suscriptorEncontradoEnviado == NULL){
								suscriptor* suscriptorEncontradoCola = list_find(get_admin->suscriptores,igualSuscriptor);
								list_add(particion->suscriptoresMensajeEnviado,suscriptorEncontradoCola);
							}
							free(content);
						}

					}
					index++;
				}
			}
		pthread_mutex_unlock(&mutexSuscriptoresGet);
		pthread_mutex_unlock(&mutexQueueGet);
		break;
		}


	case LOCALIZED:{
		pthread_mutex_lock(&mutexSuscriptoresLocalized);
		pthread_mutex_lock(&mutexQueueLocalized);
		if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
		{
			estructuraAuxiliar = localized_admin;
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
						localized_pokemon_enviar lpEnviar;
						size_t sizeLocalized = sizeof(lpEnviar.id_mensaje)+sizeof(lpEnviar.idCorrelativo) + particion->tamanioMensaje + 1;//+1 porque agrego el barra cero
						void* content = malloc(sizeLocalized);
						int bytesEscritos = 0;
						int bytesLeidosMemoria = particion->posicionParticion;

						memcpy(content+bytesEscritos,idMensaje,sizeof(lpEnviar.id_mensaje));
						bytesEscritos+= sizeof(lpEnviar.id_mensaje);
						lpEnviar.idCorrelativo = particion->idCorrelativo;
						memcpy(content+bytesEscritos,&lpEnviar.idCorrelativo,sizeof(lpEnviar.idCorrelativo));
						bytesEscritos+= sizeof(lpEnviar.idCorrelativo);
						memcpy(&lpEnviar.sizeNombre,principioMemoria+bytesLeidosMemoria,sizeof(lpEnviar.sizeNombre));
						lpEnviar.sizeNombre++;
						memcpy(content+bytesEscritos,&lpEnviar.sizeNombre,sizeof(lpEnviar.sizeNombre));
						bytesEscritos+=sizeof(lpEnviar.sizeNombre);
						bytesLeidosMemoria+=sizeof(lpEnviar.sizeNombre);
						memcpy(content+bytesEscritos,principioMemoria+bytesLeidosMemoria,(lpEnviar.sizeNombre-1));
						bytesEscritos+=(lpEnviar.sizeNombre-1);
						bytesLeidosMemoria+=(lpEnviar.sizeNombre-1);
						char caracterNulo= '\0';
						memcpy(content+bytesEscritos,&caracterNulo,sizeof(caracterNulo));
						bytesEscritos+=sizeof(caracterNulo);
						memcpy(content+bytesEscritos,principioMemoria+bytesLeidosMemoria,sizeof(lpEnviar.cantidadPosiciones));
						memcpy(&lpEnviar.cantidadPosiciones,principioMemoria+bytesLeidosMemoria,sizeof(lpEnviar.cantidadPosiciones));
						bytesEscritos+=sizeof(lpEnviar.posiciones);
						bytesLeidosMemoria+=sizeof(lpEnviar.posiciones);
						memcpy(content+bytesEscritos,principioMemoria+bytesLeidosMemoria,sizeof(uint32_t)*2*lpEnviar.cantidadPosiciones);
						CONTADORLRU++;
						particion->contadorLRU = CONTADORLRU;
						send_message(socket, LOCALIZED_POKEMON,content,sizeLocalized);
						log_info(logger,"MENSAJE LOCALIZED CON ID %d ENVIADO AL SUSCRIPTOR %d",*idMensaje ,suscripcion.idSuscriptor);
						suscriptor* suscriptorEncontradoEnviado = list_find(particion->suscriptoresMensajeEnviado,igualSuscriptor);
						if(suscriptorEncontradoEnviado == NULL){
							suscriptor* suscriptorEncontradoCola = list_find(localized_admin->suscriptores,igualSuscriptor);
							list_add(particion->suscriptoresMensajeEnviado,suscriptorEncontradoCola);
						}
						free(content);
					}

				}
				index++;
			}
		}

			else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
			{
				estructuraAuxiliar = localized_admin;
				int sizeCola = list_size(estructuraAuxiliar->queue);
				int index = 0;
				while(index<sizeCola){
					uint32_t* idMensaje = list_get(estructuraAuxiliar->queue,index);
					particion_buddy_memoria* particion = encontrarParticionBuddyPorID(*idMensaje);

					if(particion != NULL){
						bool igualSuscriptor(void* suscriptorACK){
							suscriptor* suscriptorACKCasteado = suscriptorACK;
							return suscriptorACKCasteado->idSuscriptor == suscripcion.idSuscriptor;
						}
						suscriptor* suscriptorEncontrado = list_find(particion->suscriptoresACK,igualSuscriptor);

						if (suscriptorEncontrado == NULL){
							//ENVIAR
							localized_pokemon_enviar lpEnviar;
							size_t sizeLocalized = sizeof(lpEnviar.id_mensaje)+sizeof(lpEnviar.idCorrelativo) + particion->tamanioMensaje + 1;//+1 porque agrego el barra cero
							void* content = malloc(sizeLocalized);
							int bytesEscritos = 0;
							int bytesLeidosMemoria = particion->posicionParticion;

							memcpy(content+bytesEscritos,idMensaje,sizeof(lpEnviar.id_mensaje));
							bytesEscritos+= sizeof(lpEnviar.id_mensaje);
							lpEnviar.idCorrelativo = particion->idCorrelativo;
							memcpy(content+bytesEscritos,&lpEnviar.idCorrelativo,sizeof(lpEnviar.idCorrelativo));
							bytesEscritos+= sizeof(lpEnviar.idCorrelativo);
							memcpy(&lpEnviar.sizeNombre,principioMemoriaBuddy+bytesLeidosMemoria,sizeof(lpEnviar.sizeNombre));
							lpEnviar.sizeNombre++;
							memcpy(content+bytesEscritos,&lpEnviar.sizeNombre,sizeof(lpEnviar.sizeNombre));
							bytesEscritos+=sizeof(lpEnviar.sizeNombre);
							bytesLeidosMemoria+=sizeof(lpEnviar.sizeNombre);
							memcpy(content+bytesEscritos,principioMemoriaBuddy+bytesLeidosMemoria,(lpEnviar.sizeNombre-1));
							bytesEscritos+=(lpEnviar.sizeNombre-1);
							bytesLeidosMemoria+=(lpEnviar.sizeNombre-1);
							char caracterNulo= '\0';
							memcpy(content+bytesEscritos,&caracterNulo,sizeof(caracterNulo));
							bytesEscritos+=sizeof(caracterNulo);
							memcpy(content+bytesEscritos,principioMemoriaBuddy+bytesLeidosMemoria,sizeof(lpEnviar.cantidadPosiciones));
							memcpy(&lpEnviar.cantidadPosiciones,principioMemoriaBuddy+bytesLeidosMemoria,sizeof(lpEnviar.cantidadPosiciones));
							bytesEscritos+=sizeof(lpEnviar.posiciones);
							bytesLeidosMemoria+=sizeof(lpEnviar.posiciones);
							memcpy(content+bytesEscritos,principioMemoriaBuddy+bytesLeidosMemoria,sizeof(uint32_t)*2*lpEnviar.cantidadPosiciones);
							CONTADORLRUBUDDY++;
							particion->contadorLRU = CONTADORLRUBUDDY;
							send_message(socket, LOCALIZED_POKEMON,content,sizeLocalized);
							log_info(logger,"MENSAJE LOCALIZED CON ID %d ENVIADO AL SUSCRIPTOR %d",*idMensaje ,suscripcion.idSuscriptor);
							suscriptor* suscriptorEncontradoEnviado = list_find(particion->suscriptoresMensajeEnviado,igualSuscriptor);
							if(suscriptorEncontradoEnviado == NULL){
								suscriptor* suscriptorEncontradoCola = list_find(localized_admin->suscriptores,igualSuscriptor);
								list_add(particion->suscriptoresMensajeEnviado,suscriptorEncontradoCola);
							}
							free(content);
						}

					}
					index++;
				}
			}
		pthread_mutex_unlock(&mutexSuscriptoresLocalized);
		pthread_mutex_unlock(&mutexQueueLocalized);
		break;
		}

	case CATCH:{
		pthread_mutex_lock(&mutexSuscriptoresCatch);
		pthread_mutex_lock(&mutexQueueCatch);
		if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
		{
			estructuraAuxiliar = catch_admin;
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
						catch_pokemon_enviar cpEnviar;
						size_t sizeCatch = sizeof(cpEnviar.id_mensaje) + particion->tamanioMensaje + 1;//+1 porque agrego el barra cero
						void* content = malloc(sizeCatch);
						int bytesEscritos = 0;
						int bytesLeidosMemoria = particion->posicionParticion;

						memcpy(content+bytesEscritos,idMensaje,sizeof(cpEnviar.id_mensaje));
						bytesEscritos+= sizeof(cpEnviar.id_mensaje);
						memcpy(&cpEnviar.sizeNombre,principioMemoria+bytesLeidosMemoria,sizeof(cpEnviar.sizeNombre));
						cpEnviar.sizeNombre++;
						memcpy(content+bytesEscritos,&cpEnviar.sizeNombre,sizeof(cpEnviar.sizeNombre));
						bytesEscritos+=sizeof(cpEnviar.sizeNombre);
						bytesLeidosMemoria+=sizeof(cpEnviar.sizeNombre);
						memcpy(content+bytesEscritos,principioMemoria+bytesLeidosMemoria,(cpEnviar.sizeNombre-1));
						bytesEscritos+=(cpEnviar.sizeNombre-1);
						bytesLeidosMemoria+=(cpEnviar.sizeNombre-1);
						char caracterNulo= '\0';
						memcpy(content+bytesEscritos,&caracterNulo,sizeof(caracterNulo));
						bytesEscritos+=sizeof(caracterNulo);
						memcpy(content+bytesEscritos,principioMemoria+bytesLeidosMemoria,2*sizeof(uint32_t));
						CONTADORLRU++;
						particion->contadorLRU = CONTADORLRU;
						send_message(socket, CATCH_POKEMON,content,sizeCatch);
						log_info(logger,"MENSAJE CATCH CON ID %d ENVIADO AL SUSCRIPTOR %d",*idMensaje ,suscripcion.idSuscriptor);
						suscriptor* suscriptorEncontradoEnviado = list_find(particion->suscriptoresMensajeEnviado,igualSuscriptor);
						if(suscriptorEncontradoEnviado == NULL){
							suscriptor* suscriptorEncontradoCola = list_find(catch_admin->suscriptores,igualSuscriptor);
							list_add(particion->suscriptoresMensajeEnviado,suscriptorEncontradoCola);
						}
						free(content);
					}

				}
				index++;
			}
		}

			else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
			{
				estructuraAuxiliar = catch_admin;
				int sizeCola = list_size(estructuraAuxiliar->queue);
				int index = 0;
				while(index<sizeCola){
					uint32_t* idMensaje = list_get(estructuraAuxiliar->queue,index);
					particion_buddy_memoria* particion = encontrarParticionBuddyPorID(*idMensaje);

					if(particion != NULL){
						bool igualSuscriptor(void* suscriptorACK){
							suscriptor* suscriptorACKCasteado = suscriptorACK;
							return suscriptorACKCasteado->idSuscriptor == suscripcion.idSuscriptor;
						}
						suscriptor* suscriptorEncontrado = list_find(particion->suscriptoresACK,igualSuscriptor);

						if (suscriptorEncontrado == NULL){
							//ENVIAR
							catch_pokemon_enviar cpEnviar;
							size_t sizeCatch = sizeof(cpEnviar.id_mensaje) + particion->tamanioMensaje + 1;//+1 porque agrego el barra cero
							void* content = malloc(sizeCatch);
							int bytesEscritos = 0;
							int bytesLeidosMemoria = particion->posicionParticion;

							memcpy(content+bytesEscritos,idMensaje,sizeof(cpEnviar.id_mensaje));
							bytesEscritos+= sizeof(cpEnviar.id_mensaje);
							memcpy(&cpEnviar.sizeNombre,principioMemoriaBuddy+bytesLeidosMemoria,sizeof(cpEnviar.sizeNombre));
							cpEnviar.sizeNombre++;
							memcpy(content+bytesEscritos,&cpEnviar.sizeNombre,sizeof(cpEnviar.sizeNombre));
							bytesEscritos+=sizeof(cpEnviar.sizeNombre);
							bytesLeidosMemoria+=sizeof(cpEnviar.sizeNombre);
							memcpy(content+bytesEscritos,principioMemoriaBuddy+bytesLeidosMemoria,(cpEnviar.sizeNombre-1));
							bytesEscritos+=(cpEnviar.sizeNombre-1);
							bytesLeidosMemoria+=(cpEnviar.sizeNombre-1);
							char caracterNulo= '\0';
							memcpy(content+bytesEscritos,&caracterNulo,sizeof(caracterNulo));
							bytesEscritos+=sizeof(caracterNulo);
							memcpy(content+bytesEscritos,principioMemoriaBuddy+bytesLeidosMemoria,2*sizeof(uint32_t));
							CONTADORLRUBUDDY++;
							particion->contadorLRU = CONTADORLRUBUDDY;
							send_message(socket, CATCH_POKEMON,content,sizeCatch);
							log_info(logger,"MENSAJE CATCH CON ID %d ENVIADO AL SUSCRIPTOR %d",*idMensaje ,suscripcion.idSuscriptor);
							suscriptor* suscriptorEncontradoEnviado = list_find(particion->suscriptoresMensajeEnviado,igualSuscriptor);
							if(suscriptorEncontradoEnviado == NULL){
								suscriptor* suscriptorEncontradoCola = list_find(catch_admin->suscriptores,igualSuscriptor);
								list_add(particion->suscriptoresMensajeEnviado,suscriptorEncontradoCola);
							}
							free(content);
						}

					}
					index++;
				}
			}
		pthread_mutex_unlock(&mutexSuscriptoresCatch);
		pthread_mutex_unlock(&mutexQueueCatch);
		break;
		}


	case CAUGHT:{
		pthread_mutex_lock(&mutexSuscriptoresCaught);
		pthread_mutex_lock(&mutexQueueCaught);
		if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES"))
		{
			estructuraAuxiliar = caught_admin;
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
						caught_pokemon_enviar cpEnviar;
						size_t sizeCaught = sizeof(cpEnviar.id_mensaje)+sizeof(cpEnviar.idCorrelativo) + particion->tamanioMensaje + 1;//+1 porque agrego el barra cero
						void* content = malloc(sizeCaught);
						int bytesEscritos = 0;
						int bytesLeidosMemoria = particion->posicionParticion;

						memcpy(content+bytesEscritos,idMensaje,sizeof(cpEnviar.id_mensaje));
						bytesEscritos+= sizeof(cpEnviar.id_mensaje);
						cpEnviar.idCorrelativo = particion->idCorrelativo;
						memcpy(content+bytesEscritos,&cpEnviar.idCorrelativo,sizeof(cpEnviar.idCorrelativo));
						bytesEscritos+= sizeof(cpEnviar.idCorrelativo);
						memcpy(content+bytesEscritos,principioMemoria+bytesLeidosMemoria,sizeof(uint32_t));
						CONTADORLRU++;
						particion->contadorLRU = CONTADORLRU;
						send_message(socket, CAUGHT_POKEMON,content,sizeCaught);
						log_info(logger,"MENSAJE CAUGHT CON ID %d ENVIADO AL SUSCRIPTOR %d",*idMensaje ,suscripcion.idSuscriptor);
						suscriptor* suscriptorEncontradoEnviado = list_find(particion->suscriptoresMensajeEnviado,igualSuscriptor);
						if(suscriptorEncontradoEnviado == NULL){
							suscriptor* suscriptorEncontradoCola = list_find(caught_admin->suscriptores,igualSuscriptor);
							list_add(particion->suscriptoresMensajeEnviado,suscriptorEncontradoCola);
						}
						free(content);
					}

				}
				index++;
			}
		}

			else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS"))
			{
				estructuraAuxiliar = caught_admin;
				int sizeCola = list_size(estructuraAuxiliar->queue);
				int index = 0;
				while(index<sizeCola){
					uint32_t* idMensaje = list_get(estructuraAuxiliar->queue,index);
					particion_buddy_memoria* particion = encontrarParticionBuddyPorID(*idMensaje);

					if(particion != NULL){
						bool igualSuscriptor(void* suscriptorACK){
							suscriptor* suscriptorACKCasteado = suscriptorACK;
							return suscriptorACKCasteado->idSuscriptor == suscripcion.idSuscriptor;
						}
						suscriptor* suscriptorEncontrado = list_find(particion->suscriptoresACK,igualSuscriptor);

						if (suscriptorEncontrado == NULL){
							//ENVIAR
							caught_pokemon_enviar cpEnviar;
							size_t sizeCaught = sizeof(cpEnviar.id_mensaje)+sizeof(cpEnviar.idCorrelativo) + particion->tamanioMensaje + 1;//+1 porque agrego el barra cero
							void* content = malloc(sizeCaught);
							int bytesEscritos = 0;
							int bytesLeidosMemoria = particion->posicionParticion;

							memcpy(content+bytesEscritos,idMensaje,sizeof(cpEnviar.id_mensaje));
							bytesEscritos+= sizeof(cpEnviar.id_mensaje);
							cpEnviar.idCorrelativo = particion->idCorrelativo;
							memcpy(content+bytesEscritos,&cpEnviar.idCorrelativo,sizeof(cpEnviar.idCorrelativo));
							bytesEscritos+= sizeof(cpEnviar.idCorrelativo);
							memcpy(content+bytesEscritos,principioMemoriaBuddy+bytesLeidosMemoria,sizeof(uint32_t));
							CONTADORLRUBUDDY++;
							particion->contadorLRU = CONTADORLRUBUDDY;
							send_message(socket, CAUGHT_POKEMON,content,sizeCaught);
							log_info(logger,"MENSAJE CAUGHT CON ID %d ENVIADO AL SUSCRIPTOR %d",*idMensaje ,suscripcion.idSuscriptor);
							suscriptor* suscriptorEncontradoEnviado = list_find(particion->suscriptoresMensajeEnviado,igualSuscriptor);
							if(suscriptorEncontradoEnviado == NULL){
								suscriptor* suscriptorEncontradoCola = list_find(caught_admin->suscriptores,igualSuscriptor);
								list_add(particion->suscriptoresMensajeEnviado,suscriptorEncontradoCola);
							}
							free(content);
						}

					}
					index++;
				}
			}
		pthread_mutex_unlock(&mutexSuscriptoresCaught);
		pthread_mutex_unlock(&mutexQueueCaught);
		break;
		}
	}

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

//Será requerimiento del motor de administración de memoria que éste pueda depositar en un archivo
//el estado actual de la memoria en la caché según el esquema seleccionado. Para solicitar dicho dump,
//se enviará una señal SIGUSR1 que deberá ser manejada e inicializada.
//No se pretende ver el contenido de la información almacenada, sino las particiones asignadas/libres,
//indicando su dirección de comienzo y fin, su tamaño en bytes, tiempos de LRU, el tipo de cola de
//mensajes que pertenece y su identificador.

//-----------------------------------------------------------------------------------------------------------------------------
//Dump: 14/07/2012 10:11:12
//Partición 1: 0x000 - 0x3FF.	[X]	Size: 1024b	LRU:<VALOR>	Cola:<COLA>   ID:<ID>
//Partición 2: 0x400 - 0x409. 	[L]	Size: 9b
//Partición 3: 0x40A - 0x40B. 	[L] 	Size: 1b
//-----------------------------------------------------------------------------------------------------------------------------


void imprimirEstadoActualMemoria(int senial){
	if(senial == SIGUSR1)
	{
		FILE* archivoTxt = txt_open_for_append("./EstadoMemoria.txt");
		txt_write_in_file(archivoTxt, "-----------------------------------------------------------------------------------------------------------------------------\n");
		char* fecha = temporal_get_string_time();
		txt_write_in_file(archivoTxt,fecha);
		txt_write_in_file(archivoTxt,"\n");
		if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES")){
			ordenarParticionesPorPosicion();
			int index = 0;
			int sizeLista = list_size(particionesEnMemoria);
			while(index<sizeLista){
				particion_dinamica_memoria* particion = list_get(particionesEnMemoria,index);
				int numeroParticion = index+1;
				fprintf(archivoTxt, "Particion: %d ", numeroParticion);
				fflush(archivoTxt);

				void* direccionDeComienzo = principioMemoria + particion->posicionParticion;
				void* direccionDeFin = direccionDeComienzo + particion->tamanio - 1;
				fprintf(archivoTxt, "%p - %p   ",direccionDeComienzo,direccionDeFin);
				fflush(archivoTxt);
				int libre = particion->libre;
				int tamanio = particion->tamanio;

				if(libre){
					fprintf(archivoTxt,"   [L] Size: %db",tamanio);
					fflush(archivoTxt);
				}
				else{
					fprintf(archivoTxt,"   [X] Size: %db",tamanio);
					fflush(archivoTxt);
					int lru = particion->contadorLRU;
					char* nombreCola = obtenerNombreCola(particion->cola);
					int id = particion->idMensaje;
					fprintf(archivoTxt,"   LRU: %d   Cola: %s   ID: %d",lru,nombreCola,id);
					fflush(archivoTxt);
				}
				fprintf(archivoTxt,"\n");
				fflush(archivoTxt);
				index++;
			}
		}

		else if(string_equals_ignore_case(ALGORITMO_MEMORIA,"BS")){
			ordenarParticionesPorPosicionBuddy();
			int index = 0;
			int sizeLista = list_size(particionesEnMemoriaBuddy);
			while(index<sizeLista){
				{
					particion_buddy_memoria* particion = list_get(particionesEnMemoriaBuddy,index);
					int numeroParticion = index+1;
					fprintf(archivoTxt, "Particion: %d ", numeroParticion);
					fflush(archivoTxt);

					void* direccionDeComienzo = principioMemoriaBuddy + particion->posicionParticion;
					void* direccionDeFin = direccionDeComienzo + particion->tamanio - 1;
					fprintf(archivoTxt, "%p - %p   ",direccionDeComienzo,direccionDeFin);
					fflush(archivoTxt);
					int libre = particion->libre;
					int tamanio = particion->tamanio;

					if(libre){
						fprintf(archivoTxt,"   [L] Size: %db",tamanio);
						fflush(archivoTxt);
					}
					else{
						fprintf(archivoTxt,"   [X] Size: %db",tamanio);
						fflush(archivoTxt);
						int lru = particion->contadorLRU;
						char* nombreCola = obtenerNombreCola(particion->cola);
						int id = particion->idMensaje;
						fprintf(archivoTxt,"   LRU: %d   Cola: %s   ID: %d",lru,nombreCola,id);
						fflush(archivoTxt);
					}
					fprintf(archivoTxt,"\n");
					fflush(archivoTxt);
					index++;
				}
			}
		}
		txt_write_in_file(archivoTxt, "-----------------------------------------------------------------------------------------------------------------------------\n");
		txt_close_file(archivoTxt);
	}
}

char* obtenerNombreCola(id_cola id){
	switch(id){
	case NEW:{
		return "NEW";
	}
	case APPEARED:{
		return "APPEARED";
	}
	case GET:{
		return "GET";
	}
	case LOCALIZED:{
		return "LOCALIZED";
	}
	case CATCH:{
		return "CATCH";
	}
	case CAUGHT:{
		return "CAUGHT";
	}
	default:{
		return "NULL";
	}
	}
}

char* obtenerNombreColaHeader(t_header id){
	switch(id){
	case NEW_POKEMON:{
		return "NEW";
	}
	case APPEARED_POKEMON:{
		return "APPEARED";
	}
	case GET_POKEMON:{
		return "GET";
	}
	case LOCALIZED_POKEMON:{
		return "LOCALIZED";
	}
	case CATCH_POKEMON:{
		return "CATCH";
	}
	case CAUGHT_POKEMON:{
		return "CAUGHT";
	}
	default:{
		return "NULL";
	}
	}
}
