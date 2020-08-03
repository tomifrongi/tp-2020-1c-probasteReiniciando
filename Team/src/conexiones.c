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
		//perror("ERROR CONECTAR SERVIDOR");
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


void* handler_broker(void *administracion){
	administracion_cola* administracion_casteada = administracion;
	while(1){
		int socketTeam = connect_to_server(IP_BROKER, PUERTO_BROKER, NULL);
		if(socketTeam != -errno){
			administracion_casteada->team->conectado_al_broker = true;
			log_info(log_team_oficial,"CONEXION EXITOSA CON EL BROKER");
			t_message mensaje;
			suscripcion content;
			content.idSuscriptor= getpid();
			content.idCola = administracion_casteada->cola_suscriptor;
			mensaje.content = serializarSuscripcion(content);
			mensaje.head = SUSCRIPCION;
			mensaje.size = sizeof(suscripcion);
			int res = send_message(socketTeam, mensaje.head,mensaje.content, mensaje.size);
			free(mensaje.content);
			if(res != -errno)
				handler_suscripciones(socketTeam,administracion_casteada->cola_mensajes,administracion_casteada->semaforo_contador_cola,administracion_casteada->mutex_cola);
			administracion_casteada->team->conectado_al_broker = false;
		}
		else{
			log_info(log_team_oficial,"ERROR AL SUSCRIBIRSE A LA COLA");
		}
		sleep(TIEMPO_RECONEXION);
		log_info(log_team_oficial, "INCIO DEL PROCESO DE REINTENTO DE COMUNICACION CON EL BROKER");
	}
	return NULL;
}

void* escuchar_mensajes_gameboy(void* administracion){
	administracion_gameboy* administracion_casteada = administracion;
	log_info(log_team_oficial, "Servidor levantado! Escuchando en %i",PUERTO_TEAM);
		struct sockaddr team_cli;
		socklen_t len = sizeof(team_cli);
		do {
			int team_sock = accept(administracion_casteada->listener_socket, &team_cli, &len);
			if (team_sock > 0) {
				administracion_gameboy_cliente* administracion_cliente = malloc(sizeof(administracion_gameboy_cliente));
				administracion_cliente->cola_mensajes = administracion_casteada->cola_mensajes;
				administracion_cliente->listener_socket = administracion_casteada->listener_socket;
				administracion_cliente->mutex_cola = administracion_casteada->mutex_cola;
				administracion_cliente->semaforo_contador_cola = administracion_casteada->semaforo_contador_cola;
				administracion_cliente->socket_cliente = team_sock;
				log_info(log_team_oficial, "NUEVA CONEXIÓN");
				pthread_t team_cli_thread;
				pthread_create(&team_cli_thread, NULL, handler_appeared,(void*) administracion_cliente);
				pthread_detach(team_cli_thread);

			} else {
				log_error(log_team_oficial, "Error aceptando conexiones: %s", strerror(errno));
			}
		} while (1);

		return NULL;
}

void* handler_appeared(void* administracion){
	administracion_gameboy_cliente* administracion_casteada = administracion;
	while(1){
		t_message* message = recv_message(administracion_casteada->socket_cliente);
		bool flag = false;
		switch(message->head){
		case APPEARED_POKEMON:{
			appeared_pokemon* mensaje = deserializarAppeared(message->content);
			log_info(log_team_oficial,"MENSAJE APPEARED RECIBIDO \nID MENSAJE: %d \nID CORRELATIVO: %d \nNOMBRE: %s \nPOSICION (%d,%d) \n",mensaje->id_mensaje,mensaje->idCorrelativo,mensaje->nombrePokemon,mensaje->posicionEjeX,mensaje->posicionEjeY);
			suscripcion mensajeACK;
			mensajeACK.idCola = APPEARED;
			mensajeACK.idSuscriptor = getpid();
			void* mensajeACKSerializado = serializarSuscripcion(mensajeACK);
			send_message(administracion_casteada->socket_cliente,CONFIRMACION,mensajeACKSerializado,sizeof(mensajeACK));
			free(mensajeACKSerializado);
			pthread_mutex_lock(administracion_casteada->mutex_cola);
			queue_push(administracion_casteada->cola_mensajes,mensaje);
			pthread_mutex_unlock(administracion_casteada->mutex_cola);
			sem_post(administracion_casteada->semaforo_contador_cola);
			break;
		}
		case ERROR_RECV:{
			log_info(log_team_oficial,"ERROR RECIBIENDO MENSAJE");
			break;
		}
		case NO_CONNECTION:{
			flag = true;
			break;
		}
		free_t_message(message);
	}
		if(flag)
			break;

}
	free(administracion_casteada);
	return NULL;
}
void handler_suscripciones(int socketTeam,t_queue*cola_mensajes,sem_t*semaforo_contador_cola,pthread_mutex_t* mutex_cola){
	int executing = 1;
	while(executing){
		t_message* message = recv_message(socketTeam);

		switch(message->head){
		case APPEARED_POKEMON:{
			appeared_pokemon* mensaje = deserializarAppeared(message->content);
			log_info(log_team_oficial,"MENSAJE APPEARED RECIBIDO \nID MENSAJE: %d \nID CORRELATIVO: %d \nNOMBRE: %s \nPOSICION (%d,%d) \n",mensaje->id_mensaje,mensaje->idCorrelativo,mensaje->nombrePokemon,mensaje->posicionEjeX,mensaje->posicionEjeY);
			suscripcion mensajeACK;
			mensajeACK.idCola = APPEARED;
			mensajeACK.idSuscriptor = getpid();
			void* mensajeACKSerializado = serializarSuscripcion(mensajeACK);
			int res = send_message(socketTeam,CONFIRMACION,mensajeACKSerializado,sizeof(mensajeACK));
			free(mensajeACKSerializado);
			pthread_mutex_lock(mutex_cola);
			queue_push(cola_mensajes,mensaje);
			pthread_mutex_unlock(mutex_cola);
			sem_post(semaforo_contador_cola);
			if(res == -errno)
				executing = 0;

			break;
		}

		case LOCALIZED_POKEMON:{
			localized_pokemon* mensaje = deserializarLocalized(message->content);
			char* posiciones = malloc(6*(mensaje->cantidadPosiciones)+1);
			strcpy(posiciones,"");
			t_list* coordenadas = mensaje->posiciones;
			int i = 0;
			while(i< mensaje->cantidadPosiciones){
				coordenada* p = list_get(coordenadas,i);
				char xy[6];
				sprintf(xy,"(%d,%d) ",p->posicionEjeX,p->posicionEjeY);
				strcat(posiciones, xy);
				i++;
			}
			log_info(log_team_oficial,"MENSAJE APPEARED RECIBIDO \nID MENSAJE: %d \nID CORRELATIVO: %d \nNOMBRE: %s \nPOSICIONES %s \n",mensaje->id_mensaje,mensaje->idCorrelativo,mensaje->nombrePokemon,posiciones);
			free(posiciones);
			suscripcion mensajeACK;
			mensajeACK.idCola = LOCALIZED;
			mensajeACK.idSuscriptor = getpid();
			void* mensajeACKSerializado = serializarSuscripcion(mensajeACK);
			int res = send_message(socketTeam,CONFIRMACION,mensajeACKSerializado,sizeof(mensajeACK));
			free(mensajeACKSerializado);
			pthread_mutex_lock(mutex_cola);
			queue_push(cola_mensajes,mensaje);
			pthread_mutex_unlock(mutex_cola);
			sem_post(semaforo_contador_cola);
			if(res == -errno)
				executing = 0;
			break;
		}

		case CAUGHT_POKEMON:{
			caught_pokemon* mensaje = deserializarCaught(message->content);
			if(mensaje->pokemonAtrapado == 1)
				log_info(log_team_oficial,"MENSAJE CAUGHT RECIBIDO \nID MENSAJE: %d \nID CORRELATIVO: %d \n EL POKEMON FUE ATRAPADO \n",mensaje->id_mensaje,mensaje->idCorrelativo);
			else
				log_info(log_team_oficial,"MENSAJE CAUGHT RECIBIDO \nID MENSAJE: %d \nID CORRELATIVO: %d \n EL POKEMON NO FUE ATRAPADO \n",mensaje->id_mensaje,mensaje->idCorrelativo);
			suscripcion mensajeACK;
			mensajeACK.idCola = CAUGHT;
			mensajeACK.idSuscriptor = getpid();
			void* mensajeACKSerializado = serializarSuscripcion(mensajeACK);
			int res = send_message(socketTeam,CONFIRMACION,mensajeACKSerializado,sizeof(mensajeACK));
			free(mensajeACKSerializado);
			pthread_mutex_lock(mutex_cola);
			queue_push(cola_mensajes,mensaje);
			pthread_mutex_unlock(mutex_cola);
			sem_post(semaforo_contador_cola);
			if(res == -errno)
				executing = 0;
			break;
		}

		case NO_CONNECTION:{
			executing = 0;
			break;
		}

		default:{
			break;
		}

		}
		free_t_message(message);
	}
}

bool ya_esta_en_la_lista(t_list* sin_repetidos,char* especie){
	bool mismo_nombre(void* n){
		char* nombre = n;
		return (strcmp(nombre,especie) == 0);
	}
	return (list_find(sin_repetidos,mismo_nombre)!= NULL);
}

t_list*eliminar_repetidos(t_list* objetivo_pokemones_restantes){
	t_list* sin_repetidos = list_create();
	list_add(sin_repetidos,list_get(objetivo_pokemones_restantes,0));
	int i = 1;
	int size = list_size(objetivo_pokemones_restantes);

	while(i<size){
		char* especie = list_get(objetivo_pokemones_restantes,i);
		if(!ya_esta_en_la_lista(sin_repetidos,especie)){
			list_add(sin_repetidos,especie);
		}
		i++;
	}
	return sin_repetidos;
}

void enviar_gets(t_list* objetivo_pokemones_restantes,t_list* idsGett,pthread_mutex_t* mutex_idsGett){

	t_list* objetivos_pokemones_restantes_sin_repetidos = eliminar_repetidos(objetivo_pokemones_restantes);
	int size = list_size(objetivos_pokemones_restantes_sin_repetidos);
	int i = 0;
	while(i<size){
		int socket_get = connect_to_server(IP_BROKER, PUERTO_BROKER, NULL);
		if(socket_get != -errno){
		char* especie = list_get(objetivos_pokemones_restantes_sin_repetidos,i);
		get_pokemon mensaje;
		mensaje.sizeNombre = strlen(especie)+1;
		mensaje.nombrePokemon = malloc(mensaje.sizeNombre);
		strcpy(mensaje.nombrePokemon,especie);
		void* content = serializarGet(mensaje);
		free(mensaje.nombrePokemon);

		pthread_mutex_lock(mutex_idsGett);
		send_message(socket_get, GET_POKEMON,content, sizeof(uint32_t)+mensaje.sizeNombre);
		free(content);

		t_message* mensajeConfirmacionID =recv_message(socket_get);
		uint32_t idConf;
		memcpy(&idConf,mensajeConfirmacionID->content,sizeof(uint32_t));


		int* id_lista = malloc(sizeof(int));
		*id_lista = idConf;

		list_add(idsGett,id_lista);
		pthread_mutex_unlock(mutex_idsGett);
		free_t_message(mensajeConfirmacionID);
		shutdown(socket_get,SHUT_RDWR);
		}
		else{
			log_info(log_team_oficial,"ERROR AL ENVIAR MENSAJE GET");
		}

		i++;
	}





	list_destroy(objetivos_pokemones_restantes_sin_repetidos);
}

//------------------------

int enviar_catch(char* especie,int posicion_x,int posicion_y,t_list* idsCatchh,pthread_mutex_t* mutex_idsCatchh){
	int id = -1;
	catch_pokemon mensaje;
	mensaje.nombrePokemon = especie;
	mensaje.sizeNombre = strlen(especie)+1;
	mensaje.posicionEjeX = posicion_x;
	mensaje.posicionEjeY = posicion_y;
	void* content = serializarCatch(mensaje);

	int socket_catch = connect_to_server(IP_BROKER, PUERTO_BROKER, NULL);
	if(socket_catch != -errno){
		pthread_mutex_lock(mutex_idsCatchh);
		send_message(socket_catch, CATCH_POKEMON,content, sizeof(uint32_t)*3 + mensaje.sizeNombre);
		free(content);

		t_message* mensajeConfirmacionID =recv_message(socket_catch);
		uint32_t idConf;
		memcpy(&idConf,mensajeConfirmacionID->content,sizeof(uint32_t));

		int* id_lista = malloc(sizeof(int));
		*id_lista = idConf;
		id = *id_lista;
		list_add(idsCatchh,id_lista);
		pthread_mutex_unlock(mutex_idsCatchh);
		free_t_message(mensajeConfirmacionID);
		shutdown(socket_catch,SHUT_RDWR);
	}
	return id;
}



