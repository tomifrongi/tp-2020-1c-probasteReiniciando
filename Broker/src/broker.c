/*
 ============================================================================
 Name        : Broker.c
 Author      : Tomas
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "broker.h"

/* COSAS A IR VIENDO
 *
 * Manejo de semaforos para los hilos, que se dara en el handler de clientes
 * Entender bien como carajo funciona pthread
 * Hacer una prueba de la funcion init_broker_server, corriendo un cliente basico
 * Revisar los header necesarios, se dejan en el .h los actuales, pero pueden cambiar
 * Que hacer dentro de cada caso para el manejo de las colas
 * Probar levantar los datos usando el config que ya se creo
 * Manejo del listado de procesos que se encuentran actualmente conectados
 * Revisar el codigo en general? Eso se puede? Alguien me puede reportar?
 *
 * MERCADOLIBRE ME PIDE QUE PONGA MAS CARACTERES LA VERDAD BUENISMO EL TP NO ENTIENDO MUCHO PERO GENIAL EL MONOPATIN SOLAR
 *
 */

int main(void) {
	inicializarLogger("./Debug"); //logea ok!!
	PUERTO = 8080;
	ID_INICIAL = 0;
	crearColas();
	init_broker_server();
	return EXIT_SUCCESS;
}

void init_broker_server() {
	listener_socket = init_server(PUERTO);
	log_info(logger, "Servidor levantado! Escuchando en %i",PUERTO);
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
	char* id_cliente; //Id del cliente actual
	bool executing = true;
	while(executing){
		t_message* message = recv_message(broker_sock);
		switch(message->head){
			case NEW_POKEMON:{
				log_info(logger, "NEW POKEMON RECIBIDO");
				void*aux=message->content;
				new_pokemon_message mensaje;
				memcpy(&mensaje.contenidoMensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(mensaje.contenidoMensaje.nombrePokemon,aux,mensaje.contenidoMensaje.sizeNombre);
				aux += mensaje.contenidoMensaje.sizeNombre;
				memcpy(&mensaje.contenidoMensaje.cantidad,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.contenidoMensaje.posicion.ejeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.contenidoMensaje.posicion.ejeY,aux,sizeof(uint32_t));
				mensaje.id = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(new_pokemon_queue, &mensaje);
				break;
			}
			case APPEARED_POKEMON:{
				log_info(logger, "APPEARED POKEMON RECIBIDO");
				void *aux = message->content;
				appeared_pokemon_message mensaje;
				memcpy(&mensaje.contenidoMensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(mensaje.contenidoMensaje.nombrePokemon,aux,mensaje.contenidoMensaje.sizeNombre);
				aux += mensaje.contenidoMensaje.sizeNombre;
				memcpy(&mensaje.contenidoMensaje.posicion.ejeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.contenidoMensaje.posicion.ejeY,aux,sizeof(uint32_t));
				mensaje.id = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(appeared_pokemon_queue, &mensaje);

				break;
			}
			case CATCH_POKEMON:{
				log_info(logger, "CATCH POKEMON RECIBIDO");
				void *aux = message->content;
				catch_pokemon_message mensaje;
				memcpy(&mensaje.contenidoMensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(mensaje.contenidoMensaje.nombrePokemon,aux,mensaje.contenidoMensaje.sizeNombre);
				aux += mensaje.contenidoMensaje.sizeNombre;
				memcpy(&mensaje.contenidoMensaje.posicion.ejeX,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.contenidoMensaje.posicion.ejeY,aux,sizeof(uint32_t));
				mensaje.id = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(catch_pokemon_queue, &mensaje);

				break;
			}
			case CAUGHT_POKEMON:{
				log_info(logger, "CAUGHT POKEMON RECIBIDO");
				void *aux = message->content;
				caught_pokemon_message mensaje;
				memcpy(&mensaje.idCorrelativo,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(&mensaje.contenidoMensaje.pokemonAtrapado,aux,sizeof(uint32_t));
				mensaje.id = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(caught_pokemon_queue, &mensaje);
				break;
			}
			case GET_POKEMON:{
				log_info(logger, "GET POKEMON RECIBIDO");
				void *aux = message->content;
				get_pokemon_message mensaje;
				memcpy(&mensaje.contenidoMensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(mensaje.contenidoMensaje.nombrePokemon,aux,mensaje.contenidoMensaje.sizeNombre);
				mensaje.id = ID_INICIAL;
				ID_INICIAL ++;
				queue_push(get_pokemon_queue, &mensaje);

				break;
			}
			case LOCALIZED_POKEMON:{
				log_info(logger, "LOCALIZED POKEMON RECIBIDO");
				void *aux = message->content;
				localized_pokemon_message mensaje;
				memcpy(&mensaje.contenidoMensaje.sizeNombre,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);
				memcpy(mensaje.contenidoMensaje.nombrePokemon,aux,mensaje.contenidoMensaje.sizeNombre);
				aux += mensaje.contenidoMensaje.sizeNombre;
				memcpy(&mensaje.contenidoMensaje.cantidadPosiciones,aux,sizeof(uint32_t));
				aux += sizeof(uint32_t);

				//TODO -  Faltaria ver de como guardar la lista de cordenadas que mandamos en el t_list

				mensaje.id = ID_INICIAL;
				ID_INICIAL ++;
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
				log_info(logger, "ERROR COMUNIACIÓN");
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

//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------

void inicializarLogger(char* path){
	char* nombre = string_new();
	string_append(&nombre,path);
	string_append(&nombre,".log");
	logger = log_create(nombre,"broker",1,LOG_LEVEL_TRACE);
	free(nombre);
}

void cargarConfiguracion(){ //Funcion para cargar el config que nos pasen, por ahora le mande solo un puerto y tamanio de memoria general
	t_config* config = config_create("./broker.config"); //calculo que se llamara broker.config xd
	PUERTO = config_get_int_value(config, "LISTEN_PORT");
	TAMANIO_MEMORIA = config_get_int_value(config, "MEMORY_SIZE");
}

//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------


//FUNCIONES SOCKETS
//BIBLIOTECA NET
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

//BIBLIOTECA PROTOCOL

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
