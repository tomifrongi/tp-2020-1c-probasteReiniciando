#include "Gamecard.h"

pthread_mutex_t mutexLogger;

int main(void)
{
	iniciar_config_logger(); //Inicia log y config
	gm_structs_fs();//todo File System

	pthread_t new_thread;
	pthread_t catch_thread;
	pthread_t get_thread;
	pthread_create(&new_thread, NULL, (void*) handler_broker, (void*) NEW);
	pthread_detach(new_thread);
	pthread_create(&catch_thread, NULL, (void*) handler_broker, (void*) CATCH);
	pthread_detach(catch_thread);
	pthread_create(&get_thread, NULL, (void*) handler_broker, (void*) GET);
	pthread_detach(get_thread);
	init_gamecard_server();

	gm_exit();
	return EXIT_SUCCESS;
}

void init_gamecard_server() {
	listener_socket = init_server(puertoGameCard);
	log_info(logger, "Servidor levantado! Escuchando en %i",puertoGameCard);
	struct sockaddr gamecard_cli;
	socklen_t len = sizeof(gamecard_cli);
	do {
		int gamecard_sock = accept(listener_socket, &gamecard_cli, &len);
		if (gamecard_sock > 0) {
			log_info(logger, "NUEVA CONEXIÓN");
			pthread_t gamecard_cli_thread;
			pthread_create(&gamecard_cli_thread, NULL, handler_suscripciones,
					(void*) (gamecard_sock));
			pthread_detach(gamecard_cli_thread);
		} else {
			log_error(logger, "Error aceptando conexiones: %s", strerror(errno));
		}
	} while (1);
}

void* handler_broker(void *cola){
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
			memcpy (content + bytes_escritos, &content.idCola, sizeof(id_cola));
			bytes_escritos += sizeof (uint32_t);
			memcpy(content+bytes_escritos,&content.idSuscriptor,sizeof(pid_t));
			mensaje.head = SUSCRIPCION;
			mensaje.size = sizeof(suscripcion);
			send_message(socketGameCard, mensaje.head,mensaje.content, mensaje.size);
			free(mensaje.content);
			void* result = handler_suscripciones((void*)socketGameCard);
		}
		sleep(tiempoReintentoConexion);
	}
	return NULL;
}

void iniciar_config_logger()

{
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

//------------------------------------------GAMECARD INICIANDO COMO CLIENT ESPERANDO CONEXION CON BROKER (SERVER)--------------------------------------------------//
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

//----------------------------------------ENVIAR ACK------------------------------------------------------//
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

//----------------------------Procesar NEW y enviar APPEARED--------------------------------------------//
void procesar_new_enviar_appeared(void* arg)
{
	new_pokemon_enviar* new_recv = (new_pokemon*) arg;
	appeared_pokemon* appeared_snd = malloc(sizeof(appeared_pokemon_enviar));
	//t_header appeared = APPEARED_POKEMON;

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

//----------------------------Procesar GET y enviar LOCALIZED------------------------------------------//
void procesar_get_enviar_localized(void* arg)
{
	get_pokemon_enviar* get_rcv = (get_pokemon*) arg; //recibo GET y t_header es CONFIRMACION, para tener el id_mensaje
	localized_pokemon_suscripcion* localized_snd = malloc(sizeof(localized_pokemon_suscripcion));
	t_list* posiciones_list = list_create();
	t_posiciones *posicion = malloc(sizeof(t_posiciones));
	t_posiciones *otraPosicion = malloc(sizeof(t_posiciones));
	//t_header localized = LOCALIZED_POKEMON;

	posicion->posicionEjeX = 21;
	posicion->posicionEjeY = 8;

	otraPosicion->posicionEjeX = 2;
	otraPosicion->posicionEjeY = 8;

	list_add(posiciones_list, posicion);
	list_add(posiciones_list, otraPosicion);

	localized_snd->idCorrelativo = get_rcv->id_mensaje;
	localized_snd->nombrePokemon = get_rcv->nombrePokemon;
	localized_snd->sizeNombre = strlen(localized_snd->nombrePokemon) + 1;
	localized_snd->cantidadPosiciones = list_size(posiciones_list);
	/*localized_snd->posiciones = posiciones_list;*/

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

//----------------------------Procesar CATCH y enviar CAUGHT-----------------------------------------------//
void procesar_catch_enviar_caught(void* arg) {
	catch_pokemon_enviar* catch_rcv = (catch_pokemon*) arg;
	caught_pokemon* caught_snd = malloc(sizeof(caught_pokemon));
	//t_header caught = CAUGHT_POKEMON;

	caught_snd->idCorrelativo = catch_rcv->id_mensaje;
	caught_snd->pokemonAtrapado = 1;

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

//-----------------------------------FINALIZACION DEL GAMECARD------------------------------------//
void gm_exit() {
	gcfsFreeBitmaps();
	free(logger);
	log_destroy(logger);
	free(struct_paths[METADATA]);
	free(struct_paths[FILES]);
	free(struct_paths[BLOCKS]);
	free(struct_paths[TALL_GRASS]);
}
