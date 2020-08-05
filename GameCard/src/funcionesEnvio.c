//FUNCIONES SOCKETS
//BIBLIOTECA NET

//Funciones de sockets ,logger y config

#include "funcionesEnvio.h"
#include "ProtocoloDeMensajes.h"


void inicializarLogger(char* path){
	char* nombre = string_new();
	string_append(&nombre,path);
	string_append(&nombre,".log");
	logger = log_create(nombre,"broker",1,LOG_LEVEL_TRACE);
	free(nombre);
}


//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------------------------------


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


void iniciar_config_logger() {
	logger =  log_create("GameCard.log", "GameCard", 1, LOG_LEVEL_INFO);
	t_config * config = config_create("./Debug/GameCard.config");

	tiempoReintentoConexion = config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
	tiempoReintentoOperacion = config_get_int_value(config, "TIEMPO_DE_REINTENTO_OPERACION");
	tiempoRetardoOperacion = config_get_int_value(config, "TIEMPO_RETARDO_OPERACION");
	puntoMontaje = config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS");
	ipBroker = config_get_string_value(config, "IP_BROKER");
	puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
	puertoGameCard = config_get_int_value(config, "PUERTO_GAME_CARD");
}
