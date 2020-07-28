#include "conexiones.h"


//PROTOCOL----------------

int send_message(int socket, t_header head,const void* content, size_t size){
	t_message* message = create_t_message(head,size,content);
	int res = send(socket, &(message->size) , sizeof(size_t), 0);

	if(res >= 0){
		void* buffer = malloc(message->size);
		memcpy(buffer,&message->head,sizeof(t_header));
		memcpy(buffer + sizeof(t_header),message->content,size);
		res = send(socket,buffer,message->size,0);
		if(res <0){
			perror("ERROR ENVIANDO MENSAJE");
			res = -errno;
		}
		free(buffer);
	}else{
		perror("ERROR ENVIANDO MENSAJE");
		res = -errno;
	}
	free_t_message(message);

	return res;

}

void free_t_message(t_message* message){
	if(message != NULL){
		if(message->content != NULL){
			free(message->content);
		}
		free(message);
	}
}

t_message* recv_message(int socket){
	t_message * message = malloc(sizeof(t_message));

	int res = recv(socket,&message->size,sizeof(size_t),MSG_WAITALL);
	if (res <= 0 ){
		close(socket);
		free(message);
		return error(res);
	}

	void* buffer = malloc(message->size);
	res = recv(socket,buffer,message->size,MSG_WAITALL);


	if(res <= 0){
		close(socket);
		free(message);
		free(buffer);
		return error(res);
	}

	message->content = calloc(message->size - sizeof(t_header)+1,1);
	memcpy(&message->head, buffer, sizeof(t_header));
	memcpy(message->content,buffer + sizeof(t_header),message->size - sizeof(t_header));
	message->size = message->size - sizeof(t_header);

	free(buffer);
	return message;
}

t_message* create_t_message(t_header head, size_t size,const void* content){
	t_message* message = (t_message*)malloc(sizeof(t_message));
	message->head = head;
	message->content = malloc(size);
	message->size = size + sizeof(head);

	memset(message->content, 0, size);
	memcpy(message->content,content,size);

	return message;
}

t_message* error(int res){
	t_header header = res == 0? NO_CONNECTION : ERROR_RECV;
	int32_t err = -errno;
	return create_t_message(header,sizeof(err),&err);
}

int send_status(int sock,t_header head, int status){
	return send_message(sock,status,&status,sizeof(int32_t));
}

int get_status(t_message* message){
	return *((int*)message->content);
}

//------------------------

//NET---------------------

int connect_to_server(char* host,int port, void*(*callback)()) {

	int sock;

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(host);
	server_addr.sin_port = htons(port);

	sock = create_socket();

	if(connect(sock,(struct sockaddr *)&server_addr, sizeof(server_addr))< 0){
		perror("ERROR CONECTAR SERVIDOR");
		return -errno;
	}

	if(callback != NULL)
		callback();

	return sock;
}

int init_server(int port){
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR){}
	int  socket, val = 1;
	struct sockaddr_in servaddr;

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr =INADDR_ANY;
	servaddr.sin_port = htons(port);

	socket = create_socket();
	if (socket < 0) {
		char* error = strerror(errno);
		perror(error);
		free(error);
		return EXIT_FAILURE;
	}

	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	if (bind(socket,(struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
		return EXIT_FAILURE;
	}

	if (listen(socket, MAX_CLIENTS)< 0) {
		return EXIT_FAILURE;
	}
	return socket;

}

int create_socket(){
	return socket(AF_INET, SOCK_STREAM, 0);
}

//------------------------


//+CONEXIONES-------------


void* handler_broker(void *cola){
	uint32_t* colaSusc = cola;
	uint32_t colaSuscriptor = *colaSusc;
	while(1){
		int socketTeam = connect_to_server(IP_BROKER, PUERTO_BROKER, NULL);
		if(socketTeam != -errno){
			log_info(log_team_oficial, "CONEXION EXITOSA CON EL BROKER");
			t_message mensaje;
			suscripcion content;
			content.idSuscriptor= getpid();
			content.idCola = colaSuscriptor;
			mensaje.content = serializarSuscripcion(content);
			mensaje.head = SUSCRIPCION;
			mensaje.size = sizeof(suscripcion);
			send_message(socketTeam, mensaje.head,mensaje.content, mensaje.size);
			free(mensaje.content);
			handler_suscripciones(socketTeam);
		}
		sleep(TIEMPO_RECONEXION);
	}
	return NULL;
}

void escuchar_mensajes_gameboy(int listener_socket){
	log_info(log_team_oficial, "Servidor levantado! Escuchando en %i",PUERTO_TEAM);
		struct sockaddr team_cli;
		socklen_t len = sizeof(team_cli);
		do {
			int team_sock = accept(listener_socket, &team_cli, &len);
			if (team_sock > 0) {
				log_info(log_team_oficial, "NUEVA CONEXIÓN");
				pthread_t team_cli_thread;
				pthread_create(&team_cli_thread, NULL, handler_appeared,
						(void*) (team_sock));
				pthread_detach(team_cli_thread);
			} else {
				log_error(log_team_oficial, "Error aceptando conexiones: %s", strerror(errno));
			}
		} while (1);
}

void* handler_appeared(void* socket){
	int socketEnvio = (int) (socket);
	bool executing = true;
	while(executing){
		t_message* message = recv_message(socketEnvio);
		if(message->head == APPEARED_POKEMON){
			appeared_pokemon mensaje;
			mensaje = deserializarAppeared(message->content);
			suscripcion mensajeACK;
			mensajeACK.idCola = APPEARED;
			mensajeACK.idSuscriptor = getpid();
			void* mensajeACKSerializado = serializarSuscripcion(mensajeACK);
			send_message(socketEnvio,CONFIRMACION,mensajeACKSerializado,sizeof(mensajeACK));
			free(mensajeACKSerializado);

			//TODO hacer algo con este mensaje
			log_info(log_team_oficial,"MENSAJE APPEARED RECIBIDO DEL GAMEBOY");
			log_info(log_team_oficial,"ID CORRELATIVO: %d",mensaje.idCorrelativo);
			log_info(log_team_oficial,"ID MENSAJE: %d",mensaje.id_mensaje);
			log_info(log_team_oficial,"SIZE NOMBRE: %d",mensaje.sizeNombre);
			log_info(log_team_oficial,"NOMBRE: %s",mensaje.nombrePokemon);
			log_info(log_team_oficial,"POSICION EJE X: %d",mensaje.posicionEjeX);
			log_info(log_team_oficial,"POSICION EJE Y: %d",mensaje.posicionEjeY);



		}
		free_t_message(message);
	}
	return NULL;
}

void handler_suscripciones(int socketTeam){

	while(1){
		t_message* message = recv_message(socketTeam);

		switch(message->head){
		case APPEARED_POKEMON:{
			appeared_pokemon mensaje = deserializarAppeared(message->content);
			suscripcion mensajeACK;
			mensajeACK.idCola = APPEARED;
			mensajeACK.idSuscriptor = getpid();
			void* mensajeACKSerializado = serializarSuscripcion(mensajeACK);
			send_message(socketTeam,CONFIRMACION,mensajeACKSerializado,sizeof(mensajeACK));
			free(mensajeACKSerializado);
			//TODO hacer algo con el mensaje
			log_info(log_team_oficial,"MENSAJE APPEARED RECIBIDO DEL BROKER");
			log_info(log_team_oficial,"ID CORRELATIVO: %d",mensaje.idCorrelativo);
			log_info(log_team_oficial,"ID MENSAJE: %d",mensaje.id_mensaje);
			log_info(log_team_oficial,"SIZE NOMBRE: %d",mensaje.sizeNombre);
			log_info(log_team_oficial,"NOMBRE: %s",mensaje.nombrePokemon);
			log_info(log_team_oficial,"POSICION EJE X: %d",mensaje.posicionEjeX);
			log_info(log_team_oficial,"POSICION EJE Y: %d",mensaje.posicionEjeY);
			break;
		}

		case LOCALIZED_POKEMON:{
			localized_pokemon mensaje = deserializarLocalized(message->content);
			suscripcion mensajeACK;
			mensajeACK.idCola = LOCALIZED;
			mensajeACK.idSuscriptor = getpid();
			void* mensajeACKSerializado = serializarSuscripcion(mensajeACK);
			send_message(socketTeam,CONFIRMACION,mensajeACKSerializado,sizeof(mensajeACK));
			free(mensajeACKSerializado);
			//TODO hacer algo con el mensaje
			log_info(log_team_oficial,"MENSAJE LOCALIZED RECIBIDO DEL BROKER");
			log_info(log_team_oficial,"ID CORRELATIVO: %d",mensaje.idCorrelativo);
			log_info(log_team_oficial,"ID MENSAJE: %d",mensaje.id_mensaje);
			log_info(log_team_oficial,"SIZE NOMBRE: %d",mensaje.sizeNombre);
			log_info(log_team_oficial,"NOMBRE: %s",mensaje.nombrePokemon);
			log_info(log_team_oficial,"CANTIDAD DE POSICIONES: %d",mensaje.cantidadPosiciones);

			void logearPosiciones(void* posicion){
				coordenada* posicionCasteada = posicion;
				log_info(log_team_oficial,"POSICION EJE X: %d",posicionCasteada->posicionEjeX);
				log_info(log_team_oficial,"POSICION EJE Y: %d",posicionCasteada->posicionEjeY);
			}
			list_iterate(mensaje.posiciones,logearPosiciones);

			break;

		}

		case CAUGHT_POKEMON:{
			caught_pokemon mensaje = deserializarCaught(message->content);
			suscripcion mensajeACK;
			mensajeACK.idCola = CAUGHT;
			mensajeACK.idSuscriptor = getpid();
			void* mensajeACKSerializado = serializarSuscripcion(mensajeACK);
			send_message(socketTeam,CONFIRMACION,mensajeACKSerializado,sizeof(mensajeACK));
			free(mensajeACKSerializado);
			//TODO hacer algo con el mensaje
			log_info(log_team_oficial,"MENSAJE CAUGHT RECIBIDO DEL BROKER");
			log_info(log_team_oficial,"ID CORRELATIVO: %d",mensaje.idCorrelativo);
			log_info(log_team_oficial,"ID MENSAJE: %d",mensaje.id_mensaje);
			log_info(log_team_oficial,"POKEMON ATRAPADO: %d",mensaje.pokemonAtrapado);
			if(mensaje.pokemonAtrapado == 1)
				log_info(log_team_oficial,"POKEMON FUE ATRAPADO");
			else if (mensaje.pokemonAtrapado == 0)
				log_info(log_team_oficial,"POKEMON NOOO FUE ATRAPADO");
			break;
		}

		default:{
			break;
		}

		}
		free_t_message(message);
	}
}

//------------------------




