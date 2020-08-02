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
void iniciar_config_logger() {
	logger =  log_create("GameCard.log", "GameCard", 1, LOG_LEVEL_INFO);
	t_config * config = config_create("GameCard.config");

	tiempoReintentoConexion = config_get_string_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
	tiempoReintentoOperacion = config_get_string_value(config, "TIEMPO_DE_REINTENTO_OPERACION");
	tiempoRetardoOperacion = config_get_string_value(config, "TIEMPO_RETARDO_OPERACION");
	puntoMontaje = config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS");
	ipBroker = config_get_string_value(config, "IP_BROKER");
	puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
	puertoGameCard = config_get_int_value(config, "PUERTO_GAME_CARD");
}

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
	int id = *((int*) arg);
	t_header* ack_send;//seria SUSCRIPCION
	t_suscripcion susc; //contiene id_cola QUE ES EL TIPO DE MENSAJE(NEW, GET, CATCH)
	susc.idSuscriptor = getpid();

	int client_fd = connect_to_server(ipBroker, puertoBroker, NULL);
	if (client_fd > 0)
	{
		serializarMensaje(susc);
		send_message(client_fd, SUSCRIPCION, NULL, 0);
		log_info(logger, "ACK enviado al BROKER");
	}
	usleep(500000);
	log_info(logger, "La conexión con el BROKER se cerrará");
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
				new_pokemon_enviar mensaje;
				deserializarNew(mensaje);

				//ENVIO ACK AL BROKER
				send_message(socketEnvio, CONFIRMACION, NULL, 0);

				//FILE SYSTEM
				new_pokemon_gamecard* new_pokemon;
				createNewPokemon(new_pokemon);
				pthread_t thread;
				pthread_create(&thread, NULL, (void*) enviar_ack, (void*) &mensaje.id_mensaje);
				pthread_detach(thread);
				free_t_message(message);
				break;
			}
			case CATCH:{
				void *aux = message->content;
				catch_pokemon_enviar mensaje;
				deserializarCatch(mensaje);

				//Envio de ACK hay que agregarselo al broker
				send_message(socketEnvio, CONFIRMACION, NULL, 0);

				//FILE SYSTEM
				catchAPokemon(&mensaje);
				pthread_t thread5;
				pthread_create(&thread5, NULL, (void*) enviar_ack, (void*) &mensaje.id_mensaje);
				pthread_detach(thread5);
				free_t_message(message);
				break;
			}
			case GET:{
				void *aux = message->content;
				get_pokemon_enviar mensaje;
				deserializarGet(mensaje);

				//Envio de ACK hay que agregarselo al broker
				send_message(socketEnvio, CONFIRMACION, NULL, 0);

				//FILE SYSTEM
				getAPokemon(&mensaje);
				pthread_t thread3;
				pthread_create(&thread3, NULL, (void*) enviar_ack, (void*) &mensaje.id_mensaje);
				pthread_detach(thread3);
				free_t_message(message);
				break;
			}
			case 100:{ //caso de error para el enumerado, se podria agregar en el idcola
				free_t_message(message);
				pthread_exit(NULL);
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
void procesar_new_enviar_appeared(void* arg)
{
	new_pokemon_enviar* new_recv = (new_pokemon*) arg;
	createNewPokemon(new_recv);

	appeared_pokemon* appeared_snd = malloc(sizeof(appeared_pokemon_enviar));
	appeared_snd->nombrePokemon = new_recv->nombrePokemon;
	appeared_snd->sizeNombre = new_recv->sizeNombre;
	appeared_snd->idCorrelativo = new_recv->id_mensaje;
	appeared_snd->posicionEjeX = new_recv->posicionEjeX;
	appeared_snd->posicionEjeY = new_recv->posicionEjeY;
	appeared_snd->cantidad = new_recv->cantidad;

	int client_fd = connect_to_server(ipBroker, puertoBroker, NULL);
	if (client_fd > 0)
	{
		serializarAppeared(appeared_snd);
		send_message(client_fd, APPEARED_POKEMON, NULL, 0);
		log_info(logger, "El mensaje APPEARED fué enviado al BROKER");
	}
	usleep(500000);
	log_info(logger, "Cerrando la conexión con el BROKER");
	close(client_fd);
}

/*
 * Procesar mensaje Get y Enviar mensaje Localized
 * */
void procesar_get_enviar_localized(void* arg)
{
	get_pokemon_enviar* get_rcv = (get_pokemon*) arg;
	t_list* response = getAPokemon(get_rcv);

	localized_pokemon* localized_snd = malloc(sizeof(localized_pokemon_suscripcion));
	localized_snd->idCorrelativo = get_rcv->id_mensaje;
	localized_snd->nombrePokemon = get_rcv->nombrePokemon;
	localized_snd->sizeNombre = strlen(localized_snd->nombrePokemon) + 1;

	t_list* posiciones_list = list_create();

	for (int i = 0; i < list_size(response); i++) {
		t_posiciones* posicion_aux = list_get(response, i);
		for(int j = 0; j < posicion_aux->cantidad; j++) {
			t_posiciones* pos = malloc(sizeof(t_posiciones));
			pos->posicionEjeX = posicion_aux->posicionEjeX;
			pos->posicionEjeY = posicion_aux->posicionEjeY;

			list_add(posiciones_list, pos);
		}
	}

	localized_snd->cantidadPosiciones = list_size(posiciones_list);
	localized_snd->posiciones = posiciones_list;

	int client_fd = connect_to_server(ipBroker, puertoBroker, NULL);
	if (client_fd > 0)
	{
		serializarLocalized(localized_snd);
		send_message(client_fd, LOCALIZED_POKEMON, NULL, 0);
		log_info(logger, "El mensaje LOCALIZED fué enviado al BROKER");
	}
	usleep(50000);
	close(client_fd);
}

/*
 * Procesar mensaje Catch y Enviar mensaje Caught
 * */
void procesar_catch_enviar_caught(void* arg) {
	catch_pokemon_enviar* catch_rcv = (catch_pokemon*) arg;
	int response = catchAPokemon(catch_rcv);

	caught_pokemon* caught_snd = malloc(sizeof(caught_pokemon));
	caught_snd->idCorrelativo = catch_rcv->id_mensaje;
	caught_snd->pokemonAtrapado = response;

	int client_fd = connect_to_server(ipBroker, puertoBroker, NULL);
	if (client_fd > 0)
	{
		serializarCaught(caught_snd);
		send_message(client_fd, CAUGHT_POKEMON, NULL, 0);
		log_info(logger, "El mensaje CAUGHT fue enviado al BROKER");
	}
	usleep(500000);
	close(client_fd);
}

/*
 * Fin del programa
 * */
void gm_exit() {
	//Libero todo lo relacionado a File System
	gcfsFreeBitmaps();

	//Libero y destruyo logger
	free(logger);
	log_destroy(logger);

	//Libero metadata
	free(struct_paths[METADATA]);
	free(struct_paths[FILES]);
	free(struct_paths[BLOCKS]);
	free(struct_paths[TALL_GRASS]);
}
