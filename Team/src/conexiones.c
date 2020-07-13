#include "conexiones.h"


int crear_servidor(int port) {
	struct socktadrr_in direccion_servidor;
	direccion_servidor.sin_family = AF_INET;
	direccion_servidor.sin_addr.s_addr = INADDR_ANY; // por alguna razon no me toma el .s_addr
	direccion_servidor.sin_port = htans(port);

	int servidor = socket(AF_INET,SOCK_STREAM,0);

	if(bind(servidor,(void*) &direccion_servidor,sizeof(direccion_servidor)) != 0){
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

int enviar_mensaje(struct socktadrr_in cliente,t_message mensaje){
	aceptar_cliente(cliente);
	send(cliente,mensaje.content, mensaje.size,0);
	return 0;
}

int recibir_mensaje(struct socktadrr_in cliente){
	unsigned int tamaño_paquete;
	char* buffer = malloc(tamaño_paquete);

	int bytes_recibidos = recv(cliente,buffer,tamaño_paquete,0);

	if(bytes_recibidos < 0){
		perror("El cliente  se ha desconectado");
		return 1;
	}
	t_message mensaje;
	mensaje.content = buffer;
	return  0;
}
