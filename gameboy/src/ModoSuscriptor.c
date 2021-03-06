#include "ModoSuscriptor.h"


void ejecutarModoSuscriptor(t_list* argumentos)
{
	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
	int socketGame = connect_to_server(ipBroker, puertoBroker, NULL);

	if(socketGame != -errno){
		log_info(logger, "CONEXION EXITOSA CON EL BROKER");


	id_cola id;
	if(strcmp(list_get(argumentos,2),"NEW_POKEMON") == 0)
		id = NEW;
	if(strcmp(list_get(argumentos,2),"APPEARED_POKEMON") == 0)
		id = APPEARED;
	if(strcmp(list_get(argumentos,2),"CATCH_POKEMON") == 0)
		id = CATCH;
	if(strcmp(list_get(argumentos,2),"CAUGHT_POKEMON") == 0)
		id = CAUGHT;
	if(strcmp(list_get(argumentos,2),"GET_POKEMON") == 0)
		id = GET;
	if(strcmp(list_get(argumentos,2),"LOCALIZED_POKEMON") == 0)
		id = LOCALIZED;

	t_message mensajeSuscripcion = obtenerMensajeSuscripcion(id);

	int duracion;
	duracion = atoi(list_get(argumentos,3));

	send_message(socketGame, mensajeSuscripcion.head,mensajeSuscripcion.content, mensajeSuscripcion.size);
	char* nombreCola= obtenerNombreCola(id);
	log_info(logger, "SUSCRIPCION EXITOSA A LA COLA %s\n",nombreCola);

	pthread_t gameListen;
	pthread_create(&gameListen, NULL, handler_envios,(void*) (socketGame));
	//pthread_detach(gameListen);
	sleep(duracion);
	shutdown(socketGame,SHUT_RDWR);
	}
	else
		log_info(logger,"ERROR AL CONECTARSE CON EL BROKER");
}

void* handler_envios(void* socket){
	int socketGame = (int) (socket);
	while(1){
		t_message* mensajeRecibido = recv_message(socketGame);
		deserializarMensaje(mensajeRecibido);
		enviarACK(mensajeRecibido,socketGame);
		free_t_message(mensajeRecibido);
	}
}

t_message obtenerMensajeSuscripcion(id_cola id){

	t_message mensaje;
	suscripcion content;
	content.idSuscriptor= getpid();
	content.idCola = id;

	mensaje.content = serializarSuscripcionContent(content);
	mensaje.head = SUSCRIPCION;
	mensaje.size = sizeof(suscripcion);

	return mensaje;

}

int obtenerTiempo(){
	printf("Escriba el tiempo en segundos:\n");
	int tiempo;
	scanf("%d",&tiempo);

	return tiempo;
}

void enviarACK(t_message* mensajeRecibido,int socket){
	void* content = mensajeRecibido->content;
	void* contentEnviar = malloc(sizeof(mensajeACK));
	int bytesEscritos = 0;
	memcpy(contentEnviar+bytesEscritos,content,sizeof(uint32_t));
	bytesEscritos+=sizeof(uint32_t);
	pid_t id = getpid();
	memcpy(contentEnviar+bytesEscritos,&id,sizeof(pid_t));
	send_message(socket,CONFIRMACION,contentEnviar,sizeof(mensajeACK));
}


