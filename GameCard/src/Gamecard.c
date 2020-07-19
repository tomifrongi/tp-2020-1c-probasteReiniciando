#include "Gamecard.h"

pthread_mutex_t mutexLogger;

int main(void)
{
	iniciar_config_logger();

	pthread_t new_thread;
	pthread_t catch_thread;
	pthread_t get_thread;

	log_info(logger, "Se crea un hilo para subscribirse a NEW_POKEMON del BROKER");
	pthread_create(&new_thread, NULL, (void*) reintentar_conexion, (uint32_t) (NEW));
	pthread_detach(new_thread);
	usleep(600000);

	log_info(logger, "Se crea un hilo para subscribirse a CATCH_POKEMON del BROKER");
	pthread_create(&catch_thread, NULL, (void*) reintentar_conexion, (uint32_t) (CATCH));
	pthread_detach(catch_thread);
	usleep(600000);

	log_info(logger, "Se crea un hilo para subscribirse a GET_POKEMON del BROKER");
	pthread_create(&get_thread, NULL, (void*) reintentar_conexion, (uint32_t) (GET));
	pthread_detach(get_thread);
	usleep(600000);

	log_info(logger, "Creando un hilo para poner al GAMECARD en modo Servidor");
	gm_init();
	usleep(600000);

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
		delay(tiempoReintentoConexion);//no se si es asi
	}
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
}

void* handler_suscripciones(uint32_t cola_subs)
{
	int socketBroker = connect_to_server(ipBroker, puertoBroker, NULL);
	t_message* message;
	void* content;
	int salida = 0;
	int is_server;//1 GB , 0 Broker

	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

	while(1){
		if(socketBroker != -errno)
		{
			pthread_mutex_lock(&mutexLogger);
			log_info(logger, "CONEXION EXITOSA CON EL BROKER");
			pthread_mutex_unlock(&mutexLogger);

			switch(cola_subs){

			case NEW:

				content = malloc(sizeof(uint32_t));
				uint32_t numeroNew = NEW;
				memcpy(content, &numeroNew, sizeof(uint32_t));
				send_message(socketBroker, SUSCRIPCION, content, sizeof(uint32_t));
				free(content);
				do {
					message = recv_message(socketBroker);
					if(message == -1 || message == 0) {
						salida = 1;
					} else {
						void*aux=message->content;

						new_pokemon_enviar mensaje;
						deserializarNew(mensaje);

						//ENVIO ACK AL BROKER
						send_message(socketBroker, CONFIRMACION, NULL, 0);

						//FILE SYSTEM
						createNewPokemon(&mensaje);
						if (is_server == 0) {
							pthread_t thread;
							pthread_create(&thread, NULL, (void*) enviar_ack, (void*) &mensaje.id_correlacional);
							pthread_detach(thread);
							}

						pthread_t thread2;
						pthread_create(&thread2, NULL, (void*) procesar_new_enviar_appeared,  (void*) mensaje);
						pthread_detach(thread2);
					}
				} while(salida != 1);
				salida = 0;
				free_t_message(message);
				break;

			case CATCH:

				content = malloc(sizeof(uint32_t));
				uint32_t numeroCatch = CATCH;
				memcpy (content, &numeroCatch, sizeof(uint32_t));
				send_message(socketBroker, SUSCRIPCION, content, sizeof(uint32_t));
				free(content);
				do
				{
					message = recv_message(socketBroker);
					if(message == -1 || message == 0)
					{
						salida = 1;
					} else {
						void *aux = message->content;

						catch_pokemon_enviar mensaje;
						deserializarCatch(mensaje);

						//Envio de ACK hay que agregarselo al broker
						send_message(socketBroker, CONFIRMACION, NULL, 0);

						//FILE SYSTEM
						catchAPokemon(&mensaje);
						usleep(50000);

						if (is_server == 0) {
							pthread_t thread5;
							pthread_create(&thread5, NULL, (void*) enviar_ack, (void*) &mensaje.id_correlacional);
							pthread_detach(thread5);
							}

							pthread_t thread6;
							pthread_create(&thread6, NULL, (void*) procesar_catch_enviar_caught, (void*) mensaje);
							pthread_detach(thread6);
					}
				}
				while(salida != 1);
				salida = 0;
				free_t_message(message);
				break;

			case GET:

				content = malloc(sizeof(uint32_t));
				uint32_t numeroGet = GET;
				memcpy (content, &numeroGet, sizeof(uint32_t));

				send_message(socketBroker, SUSCRIPCION, content, sizeof(uint32_t));
				free(content);
				do {
					message = recv_message(socketBroker);
					if(message == -1 || message == 0) {
						salida = 1;
						} else {
								void *aux = message->content;

								get_pokemon_enviar mensaje;
								deserializarGet(mensaje);

								//Envio de ACK hay que agregarselo al broker
								send_message(socketBroker, CONFIRMACION, NULL, 0);

								//FILE SYSTEM
								getAPokemon(&mensaje);

								usleep(50000);
								if (is_server == 0) {
									pthread_t thread3;
									pthread_create(&thread3, NULL, (void*) enviar_ack, (void*) &mensaje.id_correlacional);
									pthread_detach(thread3);
									}

									pthread_t thread4;
									pthread_create(&thread4, NULL, (void*) procesar_get_enviar_localized, (void*) mensaje);
									pthread_detach(thread4);
								}
					}
					while(salida != 1);
					salida = 0;
					free_t_message(message);
					break;
			}
		}
	}

	socketBroker = connect_to_server(ipBroker, puertoBroker, NULL);

	return NULL;
}

//--------------------------------GAMECARD iniciando como SERVER-------------------------------------------------//
void gm_init()
{
	int gm_socket = init_server(LOCAL_PORT);
	log_info(logger, "SERVER creado correctamente. Esperando conexión del GAMEBOY");

	struct sockaddr_in client_info;
	socklen_t addrlen = sizeof client_info;

	do
	{
		int accept_gmcard = accept(gm_socket, &client_info, &addrlen);
		if (accept_gmcard > 0){
			log_info(logger, "Nueva conexión realizada!");
			pthread_t thread;
			pthread_create(&thread, NULL, (void*) handle_conexion_server, (void*) accept_gmcard);
			pthread_detach(thread);
		}
		else {
			log_error(logger, "Error aceptando conexiones: %s", strerror(errno));
		}
	} while(1);
}

void *handle_conexion_server(void *arg)
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
}

//----------------------------------------ENVIAR ACK------------------------------------------------------//
void enviar_ack(void* arg) {
	int id = *((int*) arg);
	t_header* ack_send;//seria SUSCRIPCION
	suscripcion susc; //contiene id_cola QUE ES EL TIPO DE MENSAJE(NEW, GET, CATCH)

	int client_fd = connect_to_server(ipBroker, puertoBroker, NULL);
	if (client_fd > 0)
	{
		serializarMensaje(susc);
		send_message(client_fd, SUSCRIPCION, NULL, 0);
		log_info(logger, "ACK enviado al BROKER");
	}
	usleep(500000);
	log_info(logger, "La conexión con el BROKER se cerrará");
	close_conection(client_fd);
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

	int client_fd = connect_to_server(ipBroker, puertoBroker, NULL);
	if (client_fd > 0)
	{
		serializarAppeared(appeared_snd);
		send_message(client_fd, APPEARED_POKEMON, NULL, 0);
		log_info(logger, "El mensaje APPEARED fué enviado al BROKER");
	}
	usleep(500000);
	log_info(logger, "Cerrando la conexión con el BROKER");
	close_conection(client_fd);
}

//----------------------------Procesar GET y enviar LOCALIZED------------------------------------------//
void procesar_get_enviar_localized(void* arg)
{
	get_pokemon_enviar* get_rcv = (get_pokemon*) arg; //recibo GET y t_header es CONFIRMACION, para tener el id_mensaje
	localized_pokemon* localized_snd = malloc(sizeof(localized_pokemon));
	t_list* posiciones_list = list_create();
	t_posiciones *posicion = malloc(sizeof(t_posiciones));
	t_posiciones *otraPosicion = malloc(sizeof(t_posiciones));
	t_header localized = LOCALIZED_POKEMON;

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
	close_conection(client_fd);
}

//----------------------------Procesar CATCH y enviar CAUGHT-----------------------------------------------//
void procesar_catch_enviar_caught(void* arg) {
	catch_pokemon_enviar* catch_rcv = (catch_pokemon*) arg;
	caught_pokemon* caught_snd = malloc(sizeof(caught_pokemon));
	t_message mensajeGamecard;
	t_header caught = CAUGHT_POKEMON;

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
	close_conection(client_fd);
}

//-----------------------------------FINALIZACION DEL GAMECARD------------------------------------//
void gm_exit() {
	close_conection(logger);
	//gcfsFreeBitmaps();
	free(logger);
	log_destroy(logger);
	free(struct_paths[METADATA]);
	free(struct_paths[FILES]);
	free(struct_paths[BLOCKS]);
	free(struct_paths[TALL_GRASS]);
}
