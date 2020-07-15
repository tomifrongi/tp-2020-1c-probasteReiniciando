#include "conexiones.h"


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
