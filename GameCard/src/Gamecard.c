#include "Gamecard.h"

pthread_mutex_t mutexLogger;

/*
 * Inicio del programa
 * */
int main(void)
{
	iniciar_config_logger(); //Inicia log y config
	gm_structs_fs();//todo lo relacionado a File System

	//Se crean hilos para manejo de colas de mensajes
	pthread_t new_thread;
	pthread_t catch_thread;
	pthread_t get_thread;

	//Inicio en modo cliente
	pthread_create(&new_thread, NULL, (void*) handler_broker, (void*) NEW);
	pthread_detach(new_thread);
	pthread_create(&catch_thread, NULL, (void*) handler_broker, (void*) CATCH);
	pthread_detach(catch_thread);
	pthread_create(&get_thread, NULL, (void*) handler_broker, (void*) GET);
	pthread_detach(get_thread);

	//Inicio en modo servidor
	init_gamecard_server();

	gm_exit();
	return EXIT_SUCCESS;
}

/*
 * Se inicia el log y config
 * */


//Tanto init_gamecard_server() como handler_broker() pueden estar funcionando simultáneamente

/*
 * Modo servidor
 * */
void init_gamecard_server() { //Inicia el gamecard en modo server a la escucha de una o varias conexiones
	listener_socket = init_server(puertoGameCard);
	log_info(logger, "Servidor levantado! Escuchando en %i",puertoGameCard);
	struct sockaddr gamecard_cli;
	socklen_t len = sizeof(gamecard_cli);
	do {
		int gamecard_sock = accept(listener_socket, &gamecard_cli, &len);
		if (gamecard_sock > 0) {
			log_info(logger, "NUEVA CONEXIÓN");
			pthread_t gamecard_cli_thread;
			pthread_create(&gamecard_cli_thread, NULL, (void*) handler_suscripciones, (void*) (gamecard_sock));
			pthread_detach(gamecard_cli_thread);
		} else {
			log_error(logger, "Error aceptando conexiones: %s", strerror(errno));
		}
	} while (1);
}

/*
 * Conexion al Broker
 * */
void* handler_broker(void *cola) { //Inicia el gamecard conectandose al broker
	uint32_t colaSuscriptor = (uint32_t) cola;
	while(1){
		int socketGameCard = connect_to_server(ipBroker, puertoBroker, NULL);
		if(socketGameCard != -errno){
			log_info(logger, "CONEXION EXITOSA CON EL BROKER");
			t_message mensaje;
			suscripcion content;
			content.idSuscriptor= getpid();
			content.idCola = colaSuscriptor;

			int bytes_escritos = 0;
			mensaje.content = malloc(sizeof(suscripcion));
			memcpy(mensaje.content+bytes_escritos, &content.idCola, sizeof(id_cola));
			bytes_escritos += sizeof (uint32_t);
			memcpy(mensaje.content+bytes_escritos, &content.idSuscriptor, sizeof(pid_t));

			mensaje.head = SUSCRIPCION;
			mensaje.size = sizeof(suscripcion);
			send_message(socketGameCard, mensaje.head,mensaje.content, mensaje.size);
			free(mensaje.content);
			void* result = handler_suscripciones((void*) socketGameCard);
		}
		sleep(tiempoReintentoConexion);
	}
	return NULL;
}

/*
 * Envio de ACK al Broker
 * */


void enviar_ack(void* arg) {
	uint32_t id = (uint32_t) arg; //creo que no se esta usando
	pid_t processId = getpid();
	int client_fd = connect_to_server(ipBroker, puertoBroker, NULL);
	if (client_fd > 0)
	{
		void* content = malloc(sizeof(uint32_t) + sizeof(pid_t));
		size_t size = sizeof(uint32_t) + sizeof(pid_t);
		int bytes_escritos = 0;
		memcpy(content,&id,sizeof(uint32_t));
		bytes_escritos += sizeof (uint32_t);
		memcpy(content + bytes_escritos,&processId,sizeof(pid_t));
		send_message(client_fd, CONFIRMACION,content,size);
		free(content);
	}
	close(client_fd);
}

/*
 * Manejo de tipos de mensajes que llegan
 * */
void* handler_suscripciones(void* socket)
{
	int socketEnvio = (int) (socket);
	bool executing = true;

	while(executing){
		t_message* message = recv_message(socketEnvio);
		switch(message->head){
			case NEW:{
				void*aux=message->content;
				new_pokemon_suscripcion* mensaje = deserializarNew(aux);
				uint32_t idMensaje = mensaje->id_mensaje;

				//FILE SYSTEM
				new_pokemon* new_pokemon = malloc(sizeof(new_pokemon));
				new_pokemon->nombrePokemon = malloc(mensaje->sizeNombre);
				new_pokemon->sizeNombre = mensaje->sizeNombre;
				strcpy(new_pokemon->nombrePokemon,mensaje->nombrePokemon);
				new_pokemon->cantidad = mensaje->cantidad;
				new_pokemon->posicionEjeX = mensaje->posicionEjeX;
				new_pokemon->posicionEjeY = mensaje->posicionEjeY;

				procesar_new_enviar_appeared(new_pokemon,idMensaje);

				pthread_t thread;
				pthread_create(&thread, NULL, (void*) enviar_ack, (void*) idMensaje);
				pthread_detach(thread);

				free_t_message(message);
				//free(mensaje->nombrePokemon);
				free(mensaje);
				free(new_pokemon->nombrePokemon);
				//free(new_pokemon);
				break;
			}
			case CATCH:{
				void*aux=message->content;
				catch_pokemon_suscripcion* mensaje = deserializarCatch(aux);
				uint32_t idMensaje = mensaje->id_mensaje;

				//FILE SYSTEM
				catch_pokemon* catch_pokemon = malloc(sizeof(catch_pokemon));
				catch_pokemon->nombrePokemon = malloc(mensaje->sizeNombre);
				catch_pokemon->sizeNombre = mensaje->sizeNombre;
				strcpy(catch_pokemon->nombrePokemon,mensaje->nombrePokemon);
				catch_pokemon->posicionEjeX = mensaje->posicionEjeX;
				catch_pokemon->posicionEjeY = mensaje->posicionEjeY;

				//Esto capaz hay que pasarlo a un hilo para que no trabe la cola
				procesar_catch_enviar_caught(catch_pokemon,idMensaje);

				pthread_t thread;
				pthread_create(&thread, NULL, (void*) enviar_ack, (void*) idMensaje);
				pthread_detach(thread);

				free_t_message(message);
				//free(mensaje->nombrePokemon);
				free(mensaje);
				free(catch_pokemon->nombrePokemon);
				//free(catch_pokemon);
				break;

			}
			case GET:{
				void*aux=message->content;
				get_pokemon_suscripcion* mensaje = deserializarGet(aux);
				uint32_t idMensaje = mensaje->id_mensaje;

				//FILE SYSTEM
				get_pokemon* get_pokemon = malloc(sizeof(catch_pokemon));
				get_pokemon->nombrePokemon = malloc(mensaje->sizeNombre);
				get_pokemon->sizeNombre = mensaje->sizeNombre;
				strcpy(get_pokemon->nombrePokemon,mensaje->nombrePokemon);

				//Esto capaz hay que pasarlo a un hilo para que no trabe la cola
				procesar_get_enviar_localized(get_pokemon,idMensaje);

				pthread_t thread;
				pthread_create(&thread, NULL, (void*) enviar_ack, (void*) idMensaje);
				pthread_detach(thread);

				free_t_message(message);
				//free(mensaje->nombrePokemon);
				free(mensaje);
				free(get_pokemon->nombrePokemon);
				//free(get_pokemon);
				break;

			}
			case 100:{ //caso de error para el enumerado, se podria agregar en el idcola
				free_t_message(message);
				//pthread_exit(NULL);
				return NULL;
				break;
			}
		}
	}
	return NULL;
}

/*
 * Procesar mensaje New y Enviar mensaje Appeared
 * */
void procesar_new_enviar_appeared(new_pokemon* arg, uint32_t idMensaje)
{
	new_pokemon* new_recv = arg;
	createNewPokemon(new_recv);

	appeared_pokemon* appeared_snd = malloc(sizeof(appeared_pokemon));
	appeared_snd->nombrePokemon = malloc(new_recv->sizeNombre);
	strcpy(appeared_snd->nombrePokemon,new_recv->nombrePokemon);
	appeared_snd->sizeNombre = new_recv->sizeNombre;
	appeared_snd->idCorrelativo = idMensaje;
	appeared_snd->posicionEjeX = new_recv->posicionEjeX;
	appeared_snd->posicionEjeY = new_recv->posicionEjeY;
	appeared_snd->cantidad = new_recv->cantidad;

	int client_fd = connect_to_server(ipBroker, puertoBroker, NULL);
	if (client_fd > 0)
	{
		void* contenido = serializarAppeared(appeared_snd);
		send_message(client_fd, APPEARED_POKEMON, contenido, sizeof(uint32_t)*4+appeared_snd->sizeNombre);
		log_info(logger, "El mensaje APPEARED fué enviado al BROKER");
		free(contenido);

		//Falta recepcion del ack del broker en un hilo aparte
		t_message* message = recv_message(client_fd);
		free(message);
		//no se hace nada con la recepcion del ACK
	}
	free(appeared_snd);
	free(appeared_snd->nombrePokemon);
	close(client_fd);
}

/*
 * Procesar mensaje Get y Enviar mensaje Localized
 * */
void procesar_get_enviar_localized(get_pokemon* arg, uint32_t idMensaje)
{
	get_pokemon* get_rcv = arg;
	t_list* response = getAPokemon(get_rcv);

	localized_pokemon* localized_snd = malloc(sizeof(localized_pokemon));
	localized_snd->idCorrelativo = idMensaje;
	localized_snd->nombrePokemon = malloc(get_rcv->sizeNombre);
	localized_snd->nombrePokemon = get_rcv->nombrePokemon;
	localized_snd->sizeNombre = get_rcv->sizeNombre;

	t_list* posiciones_list = list_create();

	for (int i = 0; i < list_size(response); i++) {
		t_posiciones* posicion_aux = list_get(response, i);
		t_envio_posicion* pos = malloc(sizeof(t_envio_posicion));
		pos->posicionEjeX = posicion_aux->posicionEjeX;
		pos->posicionEjeY = posicion_aux->posicionEjeY;
		list_add(posiciones_list, pos);
	}

	localized_snd->cantidadPosiciones = list_size(posiciones_list);
	localized_snd->posiciones = posiciones_list;

	int client_fd = connect_to_server(ipBroker, puertoBroker, NULL);
	if (client_fd > 0)
	{
		void* contenido = serializarLocalized(localized_snd);
		int tamanio_mensaje = sizeof(uint32_t)*4 + localized_snd->sizeNombre + 2*sizeof(uint32_t)*localized_snd->cantidadPosiciones;
		send_message(client_fd, LOCALIZED_POKEMON, contenido, tamanio_mensaje);
		log_info(logger, "El mensaje LOCALIZED fué enviado al BROKER");
		free(contenido);

		//Falta recepcion del ack del broker en un hilo aparte
		t_message* message = recv_message(client_fd);
		free(message);
		//no se hace nada con la recepcion del ACK
	}
	free(localized_snd);
	free(localized_snd->nombrePokemon);
	close(client_fd);
}

/*
 * Procesar mensaje Catch y Enviar mensaje Caught
 * */
void procesar_catch_enviar_caught(catch_pokemon* arg, uint32_t idMensaje) {
	catch_pokemon* catch_rcv = arg;
	int response = catchAPokemon(catch_rcv);

	caught_pokemon* caught_snd = malloc(sizeof(caught_pokemon));
	caught_snd->idCorrelativo = idMensaje;
	caught_snd->pokemonAtrapado = response;

	int client_fd = connect_to_server(ipBroker, puertoBroker, NULL);
	if (client_fd > 0)
	{
		void* content = serializarCaught(caught_snd);
		send_message(client_fd, CAUGHT_POKEMON, content, (2*sizeof(uint32_t)));
		log_info(logger, "El mensaje CAUGHT fue enviado al BROKER");
		free(content);

		//falta la recepcion del ack en hilo aparte
		t_message* message = recv_message(client_fd);
		free(message);
		//no se hace nada con la recepcion del ACK
	}
	free(caught_snd);
	close(client_fd);
}

/*
 * Fin del programa
 * */
void gm_exit() {

	close(listener_socket);

	log_destroy(logger);

	free(puntoMontaje);
	free(ipBroker);
	free(tiempoReintentoConexion);
	free(tiempoReintentoOperacion);
	free(tiempoRetardoOperacion);
	free(puertoBroker);

	//Libero metadata
	free(struct_paths[METADATA]);
	free(struct_paths[FILES]);
	free(struct_paths[BLOCKS]);
	free(struct_paths[TALL_GRASS]);
}
