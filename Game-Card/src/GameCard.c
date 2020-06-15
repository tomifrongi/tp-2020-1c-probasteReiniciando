/*
 ============================================================================
 Name        : Game-Card.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "GameCard.h"

pthread_mutex_t mutexLogger;

int main(void) {


	//Inicio de logger y traer valores del config.

	initConfigLogger();

	//Crear hilos para el manejo de las suscripciones.

	pthread_t cola_new_thread;
	pthread_t cola_catch_thread;
	pthread_t cola_get_thread;
	pthread_create(&cola_new_thread, NULL, handler_suscripciones, (uint32_t) (NEW));
	pthread_create(&cola_catch_thread, NULL, handler_suscripciones, (uint32_t) (CATCH));
	pthread_create(&cola_get_thread, NULL, handler_suscripciones, (uint32_t) (GET));
	pthread_detach(cola_new_thread);
	pthread_detach(cola_catch_thread);
	pthread_detach(cola_get_thread);

	FILE* archivo_pokemon = fopen("patch","wb+");//TODO

}


void initConfigLogger(){
	log =  log_create("GameCard.log", "GameCard", 1, LOG_LEVEL_INFO);
	t_config * config = config_create("GameCard.config");
	tiempoReintentoConexion = config_get_string_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
	tiempoReintentoOperacion = config_get_string_value(config, "TIEMPO_DE_REINTENTO_OPERACION");
	tiempoRetardoOperacion = config_get_string_value(config, "TIEMPO_RETARDO_OPERACION");
	puntoMontaje = config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS");
	ipBroker = config_get_string_value(config, "IP_BROKER");
	puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
}

void* handler_suscripciones(uint32_t cola){
	int socketBroker = connect_to_server(ipBroker, puertoBroker, NULL);
	t_message* message;
	void* content;
	int salida = 0;
	while(1){
		if(socketBroker != -errno){
			pthread_mutex_lock(&mutexLogger);
			log_info(log, "CONEXION EXITOSA CON EL BROKER");
			pthread_mutex_unlock(&mutexLogger);
			switch(cola){
				case NEW:{
					content = malloc(sizeof(uint32_t));
					uint32_t numero = NEW;
					memcpy (content, &numero, sizeof(uint32_t));
					send_message(socketBroker, SUSCRIPCION, content, sizeof(uint32_t));
					free(content);
					do{
						message = recv_message(socketBroker);
						if(message == -1 || message == 0){
							salida = 1;
						}else{
							void*aux=message->content;
							new_pokemon_enviar mensaje;
							memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
							aux += sizeof(uint32_t);
							memcpy(&mensaje.nombrePokemon,aux,mensaje.sizeNombre);
							aux += mensaje.sizeNombre;
							memcpy(&mensaje.cantidad,aux,sizeof(uint32_t));
							aux += sizeof(uint32_t);
							memcpy(&mensaje.posicionEjeX,aux,sizeof(uint32_t));
							aux += sizeof(uint32_t);
							memcpy(&mensaje.posicionEjeY,aux,sizeof(uint32_t));

							//Envio de ACK hay que agregarselo al broker
							send_message(socketBroker, CONFIRMACION, NULL, 0);

							//Empezar revision de archivos en FS

						}
					}while(salida != 1);
					salida = 0;
					free_t_message(message);
					break;
				}
				case CATCH:{
					content = malloc(sizeof(uint32_t));
					uint32_t numero = CATCH;
					memcpy (content, &numero, sizeof(uint32_t));
					send_message(socketBroker, SUSCRIPCION, content, sizeof(uint32_t));
					free(content);
					do{
						message = recv_message(socketBroker);
						if(message == -1 || message == 0){
							salida = 1;
						}else{
							void *aux = message->content;
							catch_pokemon_enviar mensaje;
							memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
							aux += sizeof(uint32_t);
							memcpy(&mensaje.nombrePokemon,aux,mensaje.sizeNombre);
							aux += mensaje.sizeNombre;
							memcpy(&mensaje.posicionEjeX,aux,sizeof(uint32_t));
							aux += sizeof(uint32_t);
							memcpy(&mensaje.posicionEjeY,aux,sizeof(uint32_t));

							//Envio de ACK hay que agregarselo al broker
							send_message(socketBroker, CONFIRMACION, NULL, 0);

							//Empezar revision de archivos en FS

						}
					}while(salida != 1);
					salida = 0;
					free_t_message(message);
					break;
				}
				case GET:{
					content = malloc(sizeof(uint32_t));
					uint32_t numero = GET;
					memcpy (content, &numero, sizeof(uint32_t));
					send_message(socketBroker, SUSCRIPCION, content, sizeof(uint32_t));
					free(content);
					do{
						message = recv_message(socketBroker);
						if(message == -1 || message == 0){
							salida = 1;
						}else{
							void *aux = message->content;
							get_pokemon_enviar mensaje;
							memcpy(&mensaje.sizeNombre,aux,sizeof(uint32_t));
							aux += sizeof(uint32_t);
							memcpy(mensaje.nombrePokemon,aux,mensaje.sizeNombre);

							//Envio de ACK hay que agregarselo al broker
							send_message(socketBroker, CONFIRMACION, NULL, 0);

							//Empezar revision de archivos en FS

						}
					}while(salida != 1);
					salida = 0;
					free_t_message(message);
					break;
				}
			}
		}
		socketBroker = connect_to_server(ipBroker, puertoBroker, NULL);
	}
	return NULL;
}
/* TODO te comento tu codigo para poder debugear xddddd

int cantidad_pokemones(FILE* archivo_pokemon){
	int cantidad=0;
	t_linea linea;
	while(archivo_pokemon){
		fscanf(archivo_pokemon,"%d%c%d%c%d%\n",&linea.cord_x,linea.guion,&linea.cord_y,linea.igual,&linea.cantidad);
		cantidad += linea.cantidad;
	}
	return cantidad;
}
// funciones new_pokemon
void existen_posiciones_pokemon_nuevo(FILE* archivo_pokemon,new_pokemon pokemon_nuevo){
	char* posicion = "";
	t_linea linea;

	while(archivo_pokemon){
		fscanf(archivo_pokemon,"%d%c%d%c%d%\n",&linea.cord_x,linea.guion,&linea.cord_y,linea.igual,&linea.cantidad);
		if(linea.cord_x == pokemon_nuevo.posicionEjeX && linea.cord_y == pokemon_nuevo.posicionEjeY){
			pokemon_nuevo.cantidad = linea.cantidad;
			break;
		}
	}
	strcat(posicion,pokemon_nuevo.posicionEjeX);
	strcat(posicion,"-");
	strcat(posicion,pokemon_nuevo.posicionEjeY);   // TODO averiguar como convertir enteros en Strings
	strcat(posicion,"=");
	strcat(posicion,pokemon_nuevo.cantidad);
	strcat(posicion,"\n");
	fwrite(posicion,1,sizeof(posicion),archivo_pokemon);
}


// funciones catch_pokemon

void existen_posiciones_pokemon_atrapado(FILE* archivo_pokemon,catch_pokemon pokemon_atrapado){
	t_linea linea = malloc(sizeof(t_linea));
	while(archivo_pokemon){
		fscanf(archivo_pokemon,"%d%c%d%c%d%\n",&linea.cord_x,linea.guion,&linea.cord_y,linea.igual,&linea.cantidad);
		if(linea.cord_x == pokemon_atrapado.posicionEjeX && linea.cord_y == pokemon_atrapado.posicionEjeY)
			break;
		}
	printf("posicion no encontrada");
	exit(1);
}

void decrementar_cantidad(FILE* archivo_pokemon,catch_pokemon pokemon_atrapado){
	t_linea linea=malloc(sizeof(t_linea));
	while(archivo_pokemon){
		fscanf(archivo_pokemon,"%d%c%d%c%d%\n",&linea.cord_x,linea.guion,&linea.cord_y,linea.igual,&linea.cantidad);
		if(linea.cord_x == pokemon_atrapado.posicionEjeX && linea.cord_y == pokemon_atrapado.posicionEjeY){
			if(linea.cantidad == 0)
				eliminarLinea(archivo_pokemon,linea);//TODO
			else
				linea.cantidad--;

			break;
		}

	}
}

void eliminarLinea(FILE* archivo_pokemon,t_linea linea_a_borrar){
	t_linea linea=malloc(sizeof(t_linea));
	while(archivo_pokemon){
		fscanf(archivo_pokemon,"%d%c%d%c%d%\n",&linea.cord_x,linea.guion,&linea.cord_y,linea.igual,&linea.cantidad);
		if(linea.cord_x == linea_a_borrar.cord_x && linea.cord_y == linea_a_borrar.cord_y){
			fwrite("",1,sizeof(""),archivo_pokemon);
			break;
		}

	}
}

// funciones get_pokemon

t_list* obtener_posiciones_y_cantidades(FILE* archivo_pokemon){
	t_linea linea = malloc(sizeof(t_linea));
	t_list lista = list_create();
	while(archivo_pokemon){
		fscanf(archivo_pokemon,"%d%c%d%c%d%\n",&linea.cord_x,linea.guion,&linea.cord_y,linea.igual,&linea.cantidad);
		list_add(lista,linea);
	}

	 return lista;
}

*/


