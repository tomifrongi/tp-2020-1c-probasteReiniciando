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













//ESTABAN ANTES ----------
int crear_servidor(int port) {
	struct socktadrr_in direccion_servidor;
	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_addr.s_addr = INADDR_ANY; // por alguna razon no me toma el .s_addr
	direccion_servidor.sin_port = htans(port);

	int servidor = socket(AF_INET,SOCK_STREAM,0);

	if(bind(servidor,(void*) &direccion_servidor,sizeof(direccion_servidor)) != 0){    // esto vuela en cualquier momento
		perror("ERROR AL CONECTARSE AL SERVIDOR");
		return 1;
	}
	listen(servidor,SOMAXCONN);
	return 0;
}

int aceptar_cliente(struct socktadrr_in servidor){
	struct socktadrr_in direccion_cliente;
	unsigned int tamaño_direccion;
	int cliente = accept(servidor,(void*) &direccion_cliente,&tamaño_direccion);
	return 0;
}

int enviar_mensaje(int socket,t_header head,const void* content,size_t size){
	t_message* mensaje = crear_mensaje(head,size,content);
	int res = send(socket,&(mensaje->size),sizeof(size_t),0);
	if(res < 0){
		perror("ERROR AL ENVIAR EL MENSAJE");
		res = -1;
	}
	else{
		void* buffer = malloc(mensaje->size);
		memcpy(buffer,&mensaje->head,sizeof(t_header));
		memcpy(buffer + sizeof(t_header),mensaje->content,size);
		res = send(socket,buffer,mensaje->size,0);
		free(buffer);
	}
	free(mensaje);
	return res;
}

t_message* crear_mensaje(t_header head,size_t size,void* content){
	t_message* mensaje=malloc(sizeof(t_message));
	mensaje->head = head;
	mensaje->size = size + sizeof(head);
	mensaje->content = malloc(size);

	memset(mensaje->content,0,size);
	memcpy(mensaje->content,content,size);
	return mensaje;
}


t_message recibir_mensaje(int socket){
	t_message* mensaje = (t_message*) malloc(sizeof(t_message));
	int res = recv(socket,&mensaje->size,sizeof(size_t),MSG_WAITALL);
	if(res <= 0){
		close(socket);
		free(mensaje);
		perror("ERROR AL ENVIAR EL MENSAJE");
	}
	void* buffer= malloc(mensaje->size);
	res = recv(socket,buffer,&mensaje->size,MSG_WAITALL);
	if(res <= 0){
			close(socket);
			free(mensaje);
			free(buffer);
			perror("ERROR AL ENVIAR EL MENSAJE");
		}
	mensaje->content = calloc(mensaje->size - sizeof(size_t)+1,1);
	memcpy(&mensaje->head,buffer,sizeof(t_header));
	memcpy(&mensaje->content,buffer,sizeof(t_header));
	mensaje->size -= sizeof(t_header);

	free(buffer);
	return  mensaje;
}

void *suscribirseBrokerLocalized(){
	int socketSuscripcion = crearConexion(teamConf->IP_BROKER,teamConf->PUERTO_BROKER,teamConf->TIEMPO_RECONEXION);

	suscribirseLocalized(teamConf->NOMBRE_PROCESO,0,socketSuscripcion);
	pthread_mutex_t mutexRecibir;
	pthread_mutex_init(&mutexRecibir,NULL);

	t_paquete *buffer;

	int flag = 1;
	while(flag){
		pthread_mutex_lock(&mutexRecibir);
		bufferLoco = recibirMensaje(socketSuscripcion);

		if(bufferLoco != NULL){
			pthread_mutex_lock(&mutex_bandeja);
			queue_push(bandejaDeMensajes,(void*)bufferLoco);
			pthread_mutex_unlock(&mutex_bandeja);
			pthread_mutex_unlock(&mutexRecibir);
			sem_post(&contadorBandeja);
		}
		else
		{
			socketSuscripcion = crearConexion(teamConf->IP_BROKER,teamConf->PUERTO_BROKER,teamConf->TIEMPO_RECONEXION);
			suscribirseLocalized(teamConf->NOMBRE_PROCESO,0,socketSuscripcion);
		}

	}
	return NULL;
}
