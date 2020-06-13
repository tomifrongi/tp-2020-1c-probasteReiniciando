#include "ModoSuscriptor.h"


void ejecutarModoSuscriptor()
{
	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");;
	int socketGame = connect_to_server(ipBroker, puertoBroker, NULL);

	if(socketGame != -errno)
		log_info(logger, "CONEXION EXITOSA CON EL BROKER");

	id_cola id = obtenerID();
	t_message mensajeSuscripcion = obtenerMensajeSuscripcion(id);
	int duracion = obtenerTiempo();

	send_message(socketGame, mensajeSuscripcion.head,mensajeSuscripcion.content, mensajeSuscripcion.size);
	char* nombreCola= obtenerNombreCola(id);
	log_info(logger, "SUSCRIPCION EXITOSA A LA COLA %s\n",nombreCola);
	char tiempoInicio = temporal_getemporal_get_string_time();
/*		while(compararTiempo(duracion,tiempoInicio))
			escuchar()
			if(llegoAlgo())
				logearMensaje()
*/
}


t_message obtenerMensajeSuscripcion(id_cola id){

	t_message mensaje;
	mensaje.content = serializarSuscripcionContent(id);
	mensaje.head = CONFIRMACION;
	mensaje.size = sizeof(suscripcion);

	return mensaje;

}

int obtenerTiempo(){
	printf("Escriba el tiempo en minutos:\n");
	int tiempo;
	scanf("%d",&tiempo);

	return tiempo;
}

bool compararTiempo(int duracion,char*tiempoInicio){
	//formato: hh:mm:ss:mmmm

	char * tiempoactual = temporal_get_string_time();
	//TODO compararTiempos
}
