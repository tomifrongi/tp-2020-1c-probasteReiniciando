#include "ModoSuscriptor.h"


void ejecutarModoSuscriptor()
{
	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
	int socketGame = connect_to_server(ipBroker, puertoBroker, NULL);

	if(socketGame != -errno)
		log_info(logger, "CONEXION EXITOSA CON EL BROKER");

	id_cola id = obtenerID();
	t_message mensajeSuscripcion = obtenerMensajeSuscripcion(id);
	int duracion = obtenerTiempo();

	send_message(socketGame, mensajeSuscripcion.head,mensajeSuscripcion.content, mensajeSuscripcion.size);
	char* nombreCola= obtenerNombreCola(id);
	log_info(logger, "SUSCRIPCION EXITOSA A LA COLA %s\n",nombreCola);

	pthread_t gameListen;
	pthread_create(&gameListen, NULL, handler_envios,(void*) (socketGame));
	//pthread_detach(gameListen);
	sleep(duracion);

}

void* handler_envios(void* socket){
	int socketGame = (int) (socket);
	while(1){
		t_message* mensajeRecibido = recv_message(socketGame);
		//log_info(logger, "MENSAJE %s RECIBIDO",mensajeRecibido->head);
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


