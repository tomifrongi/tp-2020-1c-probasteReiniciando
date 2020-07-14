#include "Gamecard.h"

pthread_mutex_t mutexLogger;
t_log* log;

int main(void)
{

	initConfigLogger();

	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

	pthread_t new_thread;
	pthread_t catch_thread;
	pthread_t get_thread;

	/*t_queue new_queue;
	t_queue catch_queue;
	t_queue get_queue;*/


	printf("Se crea un hilo para subscribirse a NEW_POKEMON del BROKER");
	pthread_create(&new_thread, NULL, (void*) gm_retry_connect, (uint32_t) (NEW));
	pthread_detach(new_thread);
	usleep(600000);

	printf("Se crea un hilo para subscribirse a CATCH_POKEMON del BROKER");
	pthread_create(&catch_thread, NULL, (void*) gm_retry_connect, (uint32_t) (CATCH));
	pthread_detach(catch_thread);
	usleep(600000);

	printf("Se crea un hilo para subscribirse a GET_POKEMON del BROKER");
	pthread_create(&get_thread, NULL, (void*) gm_retry_connect, (uint32_t) (GET));
	pthread_detach(get_thread);
	usleep(600000);

	for(;;);

	/*gamecard_exit();
	return EXIT_SUCCESS;*/
}

void gm_retry_connect(void* arg)
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

void initConfigLogger()

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

void* handler_suscripciones(uint32_t cola)
{
	int socketBroker = connect_to_server(ipBroker, puertoBroker, NULL);
	t_message* message;
	void* content;
	int salida = 0;
	while(1){
		if(socketBroker != -errno){
			pthread_mutex_lock(&mutexLogger);
			log_info(log, "CONEXION EXITOSA CON EL BROKER");
			pthread_mutex_unlock(&mutexLogger);
			switch(cola){
				case NEW:{
					content = malloc(sizeof(uint32_t));
					uint32_t numero = NEW;
					memcpy (content, &numero, sizeof(uint32_t));
					send_message(socketBroker, SUSCRIPCION, content, sizeof(uint32_t));
					free(content);
					do{
						message = recv_message(socketBroker);
						if(message == -1 || message == 0){
							salida = 1;
						}else{
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

							//Envio de ACK hay que agregarselo al broker
							//CONFIRMACION es t_header
							send_message(socketBroker, CONFIRMACION, NULL, 0);

							//Empezar revision de archivos en FS

						}
					}while(salida != 1);
					salida = 0;
					free_t_message(message);
					break;
				}
				case CATCH:{
					content = malloc(sizeof(uint32_t));
					uint32_t numero = CATCH;
					memcpy (content, &numero, sizeof(uint32_t));
					send_message(socketBroker, SUSCRIPCION, content, sizeof(uint32_t));
					free(content);
					do{
						message = recv_message(socketBroker);
						if(message == -1 || message == 0){
							salida = 1;
						}else{
							void *aux = message->content;
							catch_pokemon_enviar mensaje;
							memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
							aux += sizeof(uint32_t);
							memcpy(&mensaje.nombrePokemon,aux,mensaje.sizeNombre);
							aux += mensaje.sizeNombre;
							memcpy(&mensaje.posicionEjeX,aux,sizeof(uint32_t));
							aux += sizeof(uint32_t);
							memcpy(&mensaje.posicionEjeY,aux,sizeof(uint32_t));

							//Envio de ACK hay que agregarselo al broker
							send_message(socketBroker, CONFIRMACION, NULL, 0);

							//Empezar revision de archivos en FS

						}
					}while(salida != 1);
					salida = 0;
					free_t_message(message);
					break;
				}
				case GET:{
					content = malloc(sizeof(uint32_t));
					uint32_t numero = GET;
					memcpy (content, &numero, sizeof(uint32_t));
					send_message(socketBroker, SUSCRIPCION, content, sizeof(uint32_t));
					free(content);
					do{
						message = recv_message(socketBroker);
						if(message == -1 || message == 0){
							salida = 1;
						}else{
							void *aux = message->content;
							get_pokemon_enviar mensaje;
							memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
							aux += sizeof(uint32_t);
							memcpy(mensaje.nombrePokemon,aux,mensaje.sizeNombre);

							//Envio de ACK hay que agregarselo al broker
							send_message(socketBroker, CONFIRMACION, NULL, 0);

							//Empezar revision de archivos en FS

						}
					}while(salida != 1);
					salida = 0;
					free_t_message(message);
					break;
				}
			}
		}
		socketBroker = connect_to_server(ipBroker, puertoBroker, NULL);
	}
	return NULL;
}

//--------------------------------GAMECARD iniciando como SERVER-------------------------------------------------//
void gm_init() {
	int gm_socket = socket_create_listener(LOCAL_IP, LOCAL_PORT);
	if (gm_socket < 0)
	{
		printf("Error al levantar GAMECARD server");
	}

	printf("Server creado correctamente!! Esperando conexion del GAMEBOY");

	struct sockaddr_in client_info;
	socklen_t addrlen = sizeof client_info;
	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);
	int accepted_fd;
	for (;;) {
		pthread_t tid;
		if ((accepted_fd = accept(gm_socket, (struct sockaddr *) &client_info, &addrlen)) != -1)
		{
			t_handle_connection* connection_handler = malloc(
					sizeof(t_handle_connection));
			connection_handler->fd = accepted_fd;
			connection_handler->bool_val = 1;
			pthread_create(&tid, NULL, (void*) handle_connection, (void*) connection_handler);
			pthread_detach(tid);
			log_info("Creando un hilo para atender una conexión en el socket %d", accepted_fd);
			usleep(500000);
		}
		else
		{
			log_error("Error al conectar con un cliente", accepted_fd);
		}
	}
}
//-------------------------------------------------------------------------------------------------------------//

//-----------------------------------------RECIBIR MENSAJE AL GAMECARD-----------------------------------------//
void *recv_gamecard(int fd, int respond_to) {
	int received_bytes;
	int client_fd = fd;
	t_header msg;
	// 1 = Receives from GB; 0 = Receives from Broker
	int is_server = respond_to;

	while (true) {
		received_bytes = recv(client_fd, &msg, sizeof(int), 0);

		if (received_bytes <= 0) {
			printf/*log_error*/("Error al recibir mensaje");
			return NULL;
		}
		switch (msg) {
		//switch (log)
		//y que el log contenta el t_header

		// From Broker or GB
		case NEW_POKEMON: {
			printf("NEW received");
			new_pokemon_enviar *new_recv= utils_receive_and_deserialize(client_fd, msg);

			log_info("ID recibido: %d", new_recv->id_mensaje);
			log_info("ID Correlacional: %d", new_recv->id_correlacional);
			log_info("Cantidad: %d", new_recv->cantidad);
			log_info("Nombre Pokemon: %s", new_recv->nombrePokemon);
			log_info("Largo Nombre: %d", new_recv->sizeNombre);
			log_info("Posicion X: %d", new_recv->posicionEjeX);
			log_info("Posicion Y: %d", new_recv->posicionEjeY);

			createNewPokemon(new_recv);
			usleep(100000);


			pthread_attr_t attrs;
			pthread_attr_init(&attrs);
			pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

			if (is_server == 0) {
				pthread_t tid;
				pthread_create(&tid, NULL, (void*) enviar_ack, (void*) &new_recv->id_correlacional);
				pthread_detach(tid);
			}

			pthread_t tid1;
			pthread_create(&tid1, NULL, (void*) process_new_and_send_appeared,
					(void*) new_recv);
			pthread_detach(tid1);

			break;
		}

			// From broker or GB
		case GET_POKEMON: {
			printf("GET received");
			get_pokemon_enviar *get_rcv = utils_receive_and_deserialize(client_fd, msg);

			log_info("ID correlacional: %d", get_rcv->id_correlacional);
			log_info("Nombre Pokemon: %s", get_rcv->nombrePokemon);
			log_info("Largo nombre: %d", get_rcv->sizeNombre);
			getAPokemon(get_rcv);
			usleep(50000);

			// To broker
			pthread_attr_t attrs;
			pthread_attr_init(&attrs);
			pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

			if (is_server == 0) {
				pthread_t tid2;
				pthread_create(&tid2, NULL, (void*) enviar_ack, (void*) &get_rcv->id_correlacional);
				pthread_detach(tid2);
			}

			pthread_t tid3;
			pthread_create(&tid3, NULL, (void*) process_get_and_send_localized,
					(void*) get_rcv);
			pthread_detach(tid3);

			break;
		}

			// From broker or GB
		case CATCH_POKEMON: {
			printf("CATCH received");
			catch_pokemon_enviar *catch_rcv = utils_receive_and_deserialize(client_fd, msg);

			log_info("ID correlacional: %d", catch_rcv->id_correlacional);
			log_info("Nombre Pokemon: %s", catch_rcv->nombrePokemon);
			log_info("Largo nombre: %d",catch_rcv->sizeNombre);
			log_info("Posicion X: %d", catch_rcv->posicionEjeX);
			log_info("Posicion Y: %d", catch_rcv->posicionEjeY);

			catchAPokemon(catch_rcv);
			usleep(50000);

			// To Broker
			pthread_attr_t attrs;
			pthread_attr_init(&attrs);
			pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE);

			if (is_server == 0) {
				pthread_t tid4;
				pthread_create(&tid4, NULL, (void*) enviar_ack, (void*) &catch_rcv->idCorrelativo);
				pthread_detach(tid4);
			}

			pthread_t tid5;

			pthread_create(&tid5, NULL, (void*) process_catch_and_send_caught,
					(void*) catch_rcv);
			pthread_detach(tid5);
			break;
		}

		default:
			break;
		}
	}
}

void enviar_ack(void* arg) {
	int id = *((int*) arg);
	t_header* ack_send;//seria SUSCRIPCION
	suscripcion susc; //contiene id_cola QUE ES EL TIPO DE MENSAJE(NEW, GET, CATCH)

	int client_fd = socket_connect_to_server(ipBroker, puertoBroker);
	if (client_fd > 0)
	{
		utils_serialize_and_send(client_fd, ack_send, susc);
		printf("ACK enviado al BROKER");
	}
	printf("La conexión con el BROKER se cerrará");
	socket_close_conection(client_fd);
}

//----------------------------Procesar NEW y enviar APPEARED--------------------------------------------//
void process_new_and_send_appeared(void* arg) {
	new_pokemon* new_recv = (new_pokemon*) arg;

	appeared_pokemon* appeared_snd = malloc(sizeof(appeared_pokemon_enviar));

	t_header appeared = APPEARED_POKEMON;

	appeared_snd->nombrePokemon = new_recv->nombrePokemon;
	appeared_snd->sizeNombre = new_recv->sizeNombre;
	appeared_snd->idCorrelativo = new_recv->id_mensaje;
	appeared_snd->posicionEjeX = new_recv->posicionEjeX;
	appeared_snd->posicionEjeY = new_recv->posicionEjeY;
	appeared_snd->cantidad = new_recv->cantidad;
	int client_fd = socket_connect_to_server(ipBroker, puertoBroker);
	if (client_fd > 0) {
		utils_serialize_and_send(client_fd,/*appeared*/ APPEARED, appeared_snd);
		printf("El mensaje APPEARED fué enviado al BROKER");
	}
	usleep(500000);
	printf("Cerrando la conexión con el BROKER");
	socket_close_conection(client_fd);
}
//-----------------------------------------------------------------------------------------------------//

//----------------------------Procesar GET y enviar LOCALIZED------------------------------------------//
void process_get_and_send_localized(void* arg) {
	get_pokemon* get_rcv = (get_pokemon*) arg;
	localized_pokemon* localized_snd = malloc(sizeof(localized_pokemon));
	t_list* posiciones = list_create();
	t_posiciones *posicion = malloc(sizeof(t_posiciones));
	t_posiciones *otraPosicion = malloc(sizeof(t_posiciones));

	posicion->posicionEjeX = 21;
	posicion->posicionEjeY = 8;

	otraPosicion->posicionEjeX = 2;
	otraPosicion->posicionEjeY = 8;

	list_add(posiciones, posicion);
	list_add(posiciones, otraPosicion);

	// Process get and sent localize
	localized_snd->idCorrelativo = get_rcv->id_mensaje;
	localized_snd->nombrePokemon = get_rcv->nombrePokemon;
	localized_snd->sizeNombre = strlen(localized_snd->nombrePokemon) + 1;
	localized_snd->cantidadPosiciones = list_size(posiciones);
	localized_snd->posiciones = posiciones;

	t_header localized = LOCALIZED_POKEMON;

	int client_fd = socket_connect_to_server(ipBroker, puertoBroker);
	if (client_fd > 0) {
		utils_serialize_and_send(client_fd, localized, localized_snd);
		printf("El mensaje LOCALIZED fué enviado al BROKER");
	}
	usleep(50000);
	socket_close_conection(client_fd);
}
//----------------------------------------------------------------------------------------------------------//

//----------------------------Procesar CATCH y enviar CAUGHT-----------------------------------------------//
void process_catch_and_send_caught(void* arg) {
	catch_pokemon* catch_rcv = (catch_pokemon*) arg;
	caught_pokemon* caught_snd = malloc(sizeof(caught_pokemon));

	// Process Catch and send Caught to broker

	caught_snd->idCorrelativo = catch_rcv->id_mensaje;
	caught_snd->pokemonAtrapado = 1;
	t_header caught = CAUGHT_POKEMON;

	int client_fd = socket_connect_to_server(ipBroker, puertoBroker);
	if (client_fd > 0) {
		utils_serialize_and_send(client_fd, caught, caught_snd);
		printf("El mensaje CAUGHT fue enviado al BROKER");
	}
	usleep(500000);
	socket_close_conection(client_fd);
}
//---------------------------------------------------------------------------------------------------------//
