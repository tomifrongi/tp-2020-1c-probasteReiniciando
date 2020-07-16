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


//TODO Recibir ACK de los suscriptores
//TODO Enviar id/confirmacion cuando publisher envia un mensaje
//TODO poner bien todos los logs
//TODO hacer config
//TODO agregar semaforos donde falten

int main(void) {
	inicializarLogger("./Debug"); //logea ok!!
	PUERTO_BROKER = 8080;
	ID_INICIAL = 0;
	TAMANO_MEMORIA =2048;
	TAMANO_MINIMO_PARTICION = 4;
	ALGORITMO_MEMORIA = "BS";
	ALGORITMO_REEMPLAZO ="FIFO";
	ALGORITMO_PARTICION_LIBRE ="FF";
	FRECUENCIA_COMPACTACION = 1;

	crearEstructurasAdministrativas();
	iniciarMutexs();
	iniciarListasIds();
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

				pthread_mutex_lock(&mutexLogger);
				log_info(logger, "NEW POKEMON RECIBIDO");
				pthread_mutex_unlock(&mutexLogger);

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


				//TODO agregar sucriptor a suscriptoresMensajeEnviado????
				t_message_envio* mensajeAEnviar = malloc(sizeof(t_message_envio));
				mensajeAEnviar->mensajeEnvio = malloc(sizeof(t_message));
				void* envio = malloc(message->size + sizeof(uint32_t));
				memcpy(envio,&mensaje.id_mensaje,sizeof(uint32_t));
				envio += sizeof(uint32_t);
				memcpy(envio,message->content,message->size);
				mensajeAEnviar->mensajeEnvio->head = message->head;
				mensajeAEnviar->mensajeEnvio->content = envio;
				mensajeAEnviar->mensajeEnvio->size = message->size + sizeof(uint32_t);
				int sizeCola = list_size(new_admin->suscriptores);
				int indice = 0;
				while(indice<sizeCola){
					suscriptor* suscriptor = list_get(new_admin->suscriptores,indice);
					mensajeAEnviar->socketEnvio = suscriptor->socket;
					pthread_t envioMensajes;
					pthread_create(&envioMensajes, NULL, handler_envio_mensajes,(void*) (mensajeAEnviar));
					pthread_detach(envioMensajes);
					indice++;
				}


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

				pthread_mutex_lock(&mutexQueueNew);
				uint32_t* idNuevo = crearElementoCola(idMenCola);
				list_add(new_admin->queue,idNuevo);
				pthread_mutex_unlock(&mutexQueueNew);

				//enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				free(mensaje.nombrePokemon);
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

				pthread_mutex_lock(&mutexQueueAppeared);
				if(buscarIdCorrelativo(idsCorrelativosAppeared,mensaje.idCorrelativo)==NULL){


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
					uint32_t* idNuevo = crearElementoCola(mensaje.id_mensaje);
					list_add(appeared_admin->queue,idNuevo);
				}
				pthread_mutex_unlock(&mutexQueueAppeared);

				free(mensaje.nombrePokemon);

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
				uint32_t* idNuevo = crearElementoCola(mensaje.id_mensaje);
				list_add(catch_admin->queue,idNuevo);
				pthread_mutex_unlock(&mutexQueueCatch);
				//enviarConfirmacion(mensaje.id_mensaje,broker_sock);
				free(mensaje.nombrePokemon);
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
				uint32_t* idNuevo = crearElementoCola(mensaje.id_mensaje);
				list_add(get_admin->queue,idNuevo);
				pthread_mutex_unlock(&mutexQueueGet);
				free(mensaje.nombrePokemon);
				//enviarConfirmacion(mensaje.id_mensaje,broker_sock);

				break;
			}
			case LOCALIZED_POKEMON:{

				pthread_mutex_lock(&mutexLogger);
				log_info(logger, "LOCALIZED POKEMON RECIBIDO");
				pthread_mutex_unlock(&mutexLogger);

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
				}
				pthread_mutex_unlock(&mutexQueueLocalized);
				free(mensaje.nombrePokemon);
				free(mensaje.posiciones);

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
				log_info(logger,"NUEVA SUSCRIPCION");
				pthread_exit(NULL);
				return NULL;
				break;
			}

			case CONFIRMACION:{
				typedef struct {
					uint32_t id_mensaje;
					pid_t idSuscriptor;
				}mensajeACK;
				//t_header = CONFIRMACION

				mensajeACK mensajeConf;
				void *aux = message->content;
				memcpy(&mensajeConf.id_mensaje,aux,sizeof(uint32_t));
				aux +=sizeof(uint32_t);
				memcpy(&mensajeConf.idSuscriptor,aux,sizeof(pid_t));

				if(string_equals_ignore_case(ALGORITMO_MEMORIA,"PARTICIONES")){
					particion_dinamica_memoria* particionConfirmacion = encontrarParticionDinamicaPorID(mensajeConf.id_mensaje);
					if(particionConfirmacion != NULL){
						bool igualSuscriptor(void* suscriptorMensajeEnviado){
							suscriptor* suscriptorMensajeEnviadoCasteado = suscriptorMensajeEnviado;
							return suscriptorMensajeEnviadoCasteado->idSuscriptor == mensajeConf.idSuscriptor;
						}
						suscriptor* suscriptorEncontrado = list_find(particionConfirmacion->suscriptoresMensajeEnviado,igualSuscriptor);
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
						list_add(particionConfirmacion->suscriptoresACK,suscriptorEncontrado);
					}

				}


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

void* handler_envio_mensajes(void* mensajeAEnviar){
	t_message_envio* mensaje = (t_message_envio*) mensajeAEnviar;
	send_message(mensaje->socketEnvio, mensaje->mensajeEnvio->head,mensaje->mensajeEnvio->content,mensaje->mensajeEnvio->size);
	//TODO falta recibir el ACK
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



						send_message(socket, NEW_POKEMON,content,sizeNew);
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


//							new_pokemon_enviar imprimir;
//	//						uint32_t sizeNombre;
//	//							char* nombrePokemon;
//	//							uint32_t cantidad;
//	//							uint32_t posicionEjeX;
//	//							uint32_t posicionEjeY;
//							imprimir.id_mensaje = particion->idMensaje;
//							void* posImprimir = principioMemoriaBuddy + particion->posicionParticion;
//							int bytesImprimir = 0;
//							memcpy(&imprimir.sizeNombre,posImprimir+bytesImprimir,sizeof(uint32_t));
//							bytesImprimir+=sizeof(uint32_t);
//							uint32_t sizeImprimirAux = imprimir.sizeNombre;
//							imprimir.sizeNombre++;
//							imprimir.nombrePokemon = malloc(imprimir.sizeNombre);
//							memcpy(imprimir.nombrePokemon,posImprimir+bytesImprimir,sizeImprimirAux);
//							memcpy(imprimir.nombrePokemon+sizeImprimirAux,&caracterNulo,sizeof(char));
//							bytesImprimir+=sizeImprimirAux;
//							memcpy(&imprimir.cantidad,posImprimir+bytesImprimir,sizeof(uint32_t));
//							bytesImprimir+=sizeof(uint32_t);
//							memcpy(&imprimir.posicionEjeX,posImprimir+bytesImprimir,sizeof(uint32_t));
//							bytesImprimir+=sizeof(uint32_t);
//							memcpy(&imprimir.posicionEjeY,posImprimir+bytesImprimir,sizeof(uint32_t));
//							bytesImprimir+=sizeof(uint32_t);
//
//							log_info(logger,"MENSAJE RECIENTE MANDADO");
//							log_info(logger,"NOMBRE: %s",imprimir.nombrePokemon);
//							log_info(logger,"CANTIDAD: %d",imprimir.cantidad);
//							log_info(logger,"POSICION EJE X: %d", imprimir.posicionEjeX);
//							log_info(logger,"POSICION EJE Y: %d", imprimir.posicionEjeY);


							send_message(socket, NEW_POKEMON,content,sizeNew);
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
		break;
		}

	case APPEARED:{
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

						send_message(socket, APPEARED_POKEMON,content,sizeAppeared);
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

							send_message(socket, APPEARED_POKEMON,content,sizeAppeared);
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
		break;
		}

	case GET:{
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

						send_message(socket, GET_POKEMON,content,sizeGet);
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

							send_message(socket, GET_POKEMON,content,sizeGet);
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
		break;
		}


	case LOCALIZED:{
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

						send_message(socket, LOCALIZED_POKEMON,content,sizeLocalized);
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

							send_message(socket, LOCALIZED_POKEMON,content,sizeLocalized);
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
		break;
		}

	case CATCH:{
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

						send_message(socket, CATCH_POKEMON,content,sizeCatch);
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

							send_message(socket, CATCH_POKEMON,content,sizeCatch);
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
		break;
		}


	case CAUGHT:{
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

						send_message(socket, CAUGHT_POKEMON,content,sizeCaught);
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

							send_message(socket, CAUGHT_POKEMON,content,sizeCaught);
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
		break;
		}
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
	pthread_mutex_init(&mutexQueueLocalized,NULL);
	pthread_mutex_init(&mutexMemoria,NULL);


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

