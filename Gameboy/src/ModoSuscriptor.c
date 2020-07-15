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

	time_t time1,time2;
	double diferencia;
	time1 = time(&time1);
	//TODO sleep y proceso hijo
	while(diferencia<duracion){
		t_message* mensajeRecibido = recv_message(socketGame);
		log_info(logger, "MENSAJE %s RECIBIDO",nombreCola);
		deserializarMensaje(mensajeRecibido);
		free_t_message(mensajeRecibido);
		time2 = time(&time2);
		diferencia = difftime(time2,time1);

		}
}


t_message obtenerMensajeSuscripcion(id_cola id){

	t_message mensaje;
	suscripcion content;
	content.idSuscriptor= getppid();
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


