#include "Gamecard.h"

pthread_mutex_t mutexLogger;
t_log* log;

int main(void)
{
	iniciar_config_logger();

	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

	pthread_t new_thread;
	pthread_t catch_thread;
	pthread_t get_thread;

	/*t_queue new_queue;
	t_queue catch_queue;
	t_queue get_queue;*/


	log_info(log, "Se crea un hilo para subscribirse a NEW_POKEMON del BROKER");
	pthread_create(&new_thread, NULL, (void*) reintentar_conexion, (uint32_t) (NEW));
	pthread_detach(new_thread);
	usleep(600000);

	log_info(log, "Se crea un hilo para subscribirse a CATCH_POKEMON del BROKER");
	pthread_create(&catch_thread, NULL, (void*) reintentar_conexion, (uint32_t) (CATCH));
	pthread_detach(catch_thread);
	usleep(600000);

	log_info(log, "Se crea un hilo para subscribirse a GET_POKEMON del BROKER");
	pthread_create(&get_thread, NULL, (void*) reintentar_conexion, (uint32_t) (GET));
	pthread_detach(get_thread);
	usleep(600000);

	log_info(log, "Creando un hilo para poner al GAMECARD en modo Servidor");
	gm_init();
	usleep(500000);

	for(;;);

	gm_exit();
	return EXIT_SUCCESS;
}

void reintentar_conexion(void* arg)
{
	//t_queue retryConn = arg;
	void * retryConn = arg;
	while (true)
	{
		is_conn = false;
		handler_suscripciones (retryConn);
		utils_delay(tiempoReintentoConexion);
	}
}

void iniciar_config_logger()

{
		log =  log_create("GameCard.log", "GameCard", 1, LOG_LEVEL_INFO);

		t_config * config = config_create("GameCard.config");

		tiempoReintentoConexion = config_get_string_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
		tiempoReintentoOperacion = config_get_string_value(config, "TIEMPO_DE_REINTENTO_OPERACION");
		tiempoRetardoOperacion = config_get_string_value(config, "TIEMPO_RETARDO_OPERACION");
		puntoMontaje = config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS");
		ipBroker = config_get_string_value(config, "IP_BROKER");
		puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
}

void* handler_suscripciones(uint32_t cola_subs)
{
	int socketBroker = connect_to_server(ipBroker, puertoBroker, NULL);
	t_message* message;
	void* content;
	int salida = 0;
	while(1){
		if(socketBroker != -errno)
		{
			pthread_mutex_lock(&mutexLogger);
			log_info(log, "CONEXION EXITOSA CON EL BROKER");
			pthread_mutex_unlock(&mutexLogger);

			t_subscribe* sub_snd = malloc(sizeof(t_subscribe));

			log_info(log, "Proceso Gamecard");
			sub_snd->ip = string_duplicate(LOCAL_IP);
			sub_snd->puerto = LOCAL_PORT;
			sub_snd->cola = cola_subs;
			utils_serialize_and_send(socketBroker, SUSCRIPCION, sub_snd);
			recibir_msgs_gamecard(socketBroker, 0);
			is_conn = true;
		}
		socketBroker = connect_to_server(ipBroker, puertoBroker, NULL);
	}
	return NULL;
}

//--------------------------------GAMECARD iniciando como SERVER-------------------------------------------------//
void gm_init()
{
	int gm_socket = socket_create_listener(LOCAL_IP, LOCAL_PORT);
	if (gm_socket < 0)
	{
		log_info(log, "Error al levantar GAMECARD server");
	}

	log_info(log, "SERVER creado correctamente. Esperando conexión del GAMEBOY");

	struct sockaddr_in client_info;
	socklen_t addrlen = sizeof client_info;
	pthread_attr_t attrs;

	pthread_attr_init(&attrs);
	pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

	int accepted_fd;

	for (;;)
	{
		pthread_t thread;
		if ((accepted_fd = accept(gm_socket, (struct sockaddr *) &client_info, &addrlen)) != -1)
		{
			t_handle_connection* conn = malloc(sizeof(t_handle_connection));
			conn->fd = accepted_fd;
			conn->valor = 1;
			pthread_create(&thread, NULL, (void*) handle_conexion_server, (void*) conn);
			pthread_detach(thread);
			log_info("Creando un hilo para atender una conexión en el socket %d", accepted_fd);
			usleep(500000);
		}
		else
		{
			log_error("Error al conectar con un cliente", log);
		}
	}
}

static void *handle_conexion_server(void *arg)
{
	t_handle_connection* conexion = (t_handle_connection *) arg;
	int client_fd = conexion->fd;
	recibir_msgs_gamecard(client_fd, conexion->valor);
	return NULL;
}

//-----------------------------------------RECIBIR MENSAJE AL GAMECARD-----------------------------------------//
void *recibir_msgs_gamecard(int fd, int respond_to)
{
	int received_bytes;
	int client_fd = fd;
	t_header msg;

	// 1 = Recibido del GAMEBOY; 0 = Recibido del BROKER
	int is_server = respond_to;

	while (true) {
		received_bytes = recv(client_fd, &msg, sizeof(int), 0);

		if (received_bytes <= 0)
		{
			log_error(log, "Error al recibir mensaje");
			return NULL;
		}
		switch (msg)
		{

		// En los 3 casos de mensajes pueden llegar del BROKER o GAMEBOY
		case NEW_POKEMON: {
			log_info(log, "NEW received");
			new_pokemon_enviar *new_recv= utils_receive_and_deserialize(client_fd, msg);

			log_info("ID recibido: %d", new_recv->id_mensaje);
			log_info("ID Correlacional: %d", new_recv->id_correlacional);
			log_info("Cantidad: %d", new_recv->cantidad);
			log_info("Nombre Pokemon: %s", new_recv->nombrePokemon);
			log_info("Largo Nombre: %d", new_recv->sizeNombre);
			log_info("Posicion X: %d", new_recv->posicionEjeX);
			log_info("Posicion Y: %d", new_recv->posicionEjeY);

			//declarada en gm_filesystem.c
			createNewPokemon(new_recv);
			usleep(100000);

			pthread_attr_t attrs;
			pthread_attr_init(&attrs);
			pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

			if (is_server == 0) {
				pthread_t thread;
				pthread_create(&thread, NULL, (void*) enviar_ack, (void*) &new_recv->id_correlacional);
				pthread_detach(thread);
			}

			pthread_t thread2;
			pthread_create(&thread2, NULL, (void*) procesar_new_enviar_appeared, (void*) new_recv);
			pthread_detach(thread2);

			break;
		}

		case GET_POKEMON: {
			log_info(log, "GET received");
			get_pokemon_enviar *get_rcv = utils_receive_and_deserialize(client_fd, msg);

			log_info("ID correlacional: %d", get_rcv->id_correlacional);
			log_info("Nombre Pokemon: %s", get_rcv->nombrePokemon);
			log_info("Largo nombre: %d", get_rcv->sizeNombre);

			//declarada en gm_filesystem.c
			getAPokemon(get_rcv);
			usleep(50000);

			// To broker
			pthread_attr_t attrs;
			pthread_attr_init(&attrs);
			pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

			if (is_server == 0) {
				pthread_t thread3;
				pthread_create(&thread3, NULL, (void*) enviar_ack, (void*) &get_rcv->id_correlacional);
				pthread_detach(thread3);
			}

			pthread_t thread4;
			pthread_create(&thread4, NULL, (void*) procesar_get_enviar_localized, (void*) get_rcv);
			pthread_detach(thread4);

			break;
		}

		case CATCH_POKEMON: {
			log_info(log, "CATCH received");
			catch_pokemon_enviar *catch_rcv = utils_receive_and_deserialize(client_fd, msg);

			log_info("ID correlacional: %d", catch_rcv->id_correlacional);
			log_info("Nombre Pokemon: %s", catch_rcv->nombrePokemon);
			log_info("Largo nombre: %d", catch_rcv->sizeNombre);
			log_info("Posicion X: %d", catch_rcv->posicionEjeX);
			log_info("Posicion Y: %d", catch_rcv->posicionEjeY);

			//declarada en gm_filesystem.c
			catchAPokemon(catch_rcv);
			usleep(50000);

			// To Broker
			pthread_attr_t attrs;
			pthread_attr_init(&attrs);
			pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

			if (is_server == 0) {
				pthread_t thread5;
				pthread_create(&thread5, NULL, (void*) enviar_ack, (void*) &catch_rcv->id_correlacional);
				pthread_detach(thread5);
			}

			pthread_t thread6;

			pthread_create(&thread6, NULL, (void*) procesar_catch_enviar_caught, (void*) catch_rcv);
			pthread_detach(thread6);
			break;
		}

		default:
			break;
		}
	}
}

//----------------------------------------ENVIAR ACK------------------------------------------------------//
void enviar_ack(void* arg) {
	int id = *((int*) arg);
	t_header* ack_send;//seria SUSCRIPCION
	suscripcion susc; //contiene id_cola QUE ES EL TIPO DE MENSAJE(NEW, GET, CATCH)

	int client_fd = socket_connect_to_server(ipBroker, puertoBroker);
	if (client_fd > 0)
	{
		utils_serialize_and_send(client_fd, ack_send, susc);
		log_info("ACK enviado al BROKER", log);
	}
	log_info(log, "La conexión con el BROKER se cerrará");
	socket_close_conection(client_fd);
}

//----------------------------Procesar NEW y enviar APPEARED--------------------------------------------//
void procesar_new_enviar_appeared(void* arg)
{
	new_pokemon_enviar* new_recv = (new_pokemon*) arg;

	appeared_pokemon* appeared_snd = malloc(sizeof(appeared_pokemon_enviar));

	t_header appeared = APPEARED_POKEMON;

	appeared_snd->nombrePokemon = new_recv->nombrePokemon;
	appeared_snd->sizeNombre = new_recv->sizeNombre;
	appeared_snd->idCorrelativo = new_recv->id_mensaje;
	appeared_snd->posicionEjeX = new_recv->posicionEjeX;
	appeared_snd->posicionEjeY = new_recv->posicionEjeY;
	appeared_snd->cantidad = new_recv->cantidad;

	int client_fd = socket_connect_to_server(ipBroker, puertoBroker);
	if (client_fd > 0)
	{
		utils_serialize_and_send(client_fd, APPEARED, appeared_snd);
		log_info(log, "El mensaje APPEARED fué enviado al BROKER");
	}
	usleep(500000);
	log_info(log, "Cerrando la conexión con el BROKER");
	socket_close_conection(client_fd);
}

//----------------------------Procesar GET y enviar LOCALIZED------------------------------------------//
void procesar_get_enviar_localized(void* arg)
{
	get_pokemon_enviar* get_rcv = (get_pokemon*) arg; //recibo GET y t_header es CONFIRMACION, para tener el id_mensaje
	localized_pokemon* localized_snd = malloc(sizeof(localized_pokemon));
	t_list* posiciones_list = list_create();
	t_posiciones *posicion = malloc(sizeof(t_posiciones));
	t_posiciones *otraPosicion = malloc(sizeof(t_posiciones));

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

	t_header localized = LOCALIZED_POKEMON;

	int client_fd = socket_connect_to_server(ipBroker, puertoBroker);
	if (client_fd > 0)
	{
		utils_serialize_and_send(client_fd, localized, localized_snd);
		log_info(log, "El mensaje LOCALIZED fué enviado al BROKER");
	}
	usleep(50000);
	socket_close_conection(client_fd);
}

//----------------------------Procesar CATCH y enviar CAUGHT-----------------------------------------------//
void procesar_catch_enviar_caught(void* arg) {
	catch_pokemon_enviar* catch_rcv = (catch_pokemon*) arg;
	caught_pokemon* caught_snd = malloc(sizeof(caught_pokemon));

	caught_snd->idCorrelativo = catch_rcv->id_mensaje;
	caught_snd->pokemonAtrapado = 1;
	t_header caught = CAUGHT_POKEMON;

	int client_fd = socket_connect_to_server(ipBroker, puertoBroker);
	if (client_fd > 0)
	{
		utils_serialize_and_send(client_fd, caught, caught_snd);
		log_info(log, "El mensaje CAUGHT fue enviado al BROKER");
	}
	usleep(500000);
	socket_close_conection(client_fd);
}

//-----------------------------------FINALIZACION DEL GAMECARD------------------------------------//
void gm_exit() {
	socket_close_conection(gamecard_fd);
	//gcfsFreeBitmaps();
	free(gamecard_fd);
	log_destroy(gamecard_fd);

	free(struct_paths[METADATA]);
	free(struct_paths[FILES]);
	free(struct_paths[BLOCKS]);
	free(struct_paths[TALL_GRASS]);
}
