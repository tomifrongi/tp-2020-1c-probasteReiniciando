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
	t_linea linea=malloc(sizeof(t_linea));;
	while(archivo_pokemon){
		fscanf(archivo_pokemon,"%d%c%d%c%d%\n",&linea.cord_x,linea.guion,&linea.cord_y,linea.igual,&linea.cantidad);
		if(linea.cord_x == pokemon_atrapado.posicionEjeX && linea.cord_y == pokemon_atrapado.posicionEjeY)
			break;
		}
	printf("posicion no encontrada");
	exit(1);
}

void decrementar_cantidad(FILE* archivo_pokemon,catch_pokemon pokemon_atrapado){
	t_linea linea=malloc(sizeof(t_linea));;
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

// funciones get_pokemon.

t_list* obtener_posiciones_y_cantidades(FILE* archivo_pokemon){
	t_linea linea = malloc(sizeof(t_linea));
	t_list lista = list_create();
	while(archivo_pokemon){
		fscanf(archivo_pokemon,"%d%c%d%c%d%\n",&linea.cord_x,linea.guion,&linea.cord_y,linea.igual,&linea.cantidad);
		list_add(lista,linea);
	}

	 return lista;
}

// soy un comentario



int main(void) {

	t_config * config = config_create("GameCard.config");
	t_log* log =  log_create("GameCard.log", "GameCard", 1, LOG_LEVEL_INFO);

	//primero hay que intentar asociarnos a las colas de mensajes del broker
	char* ipBroker = config_get_string_value(config, "IP_BROKER");
	int puertoBroker = config_get_int_value(config, "PUERTO_BROKER");
	int socketGame = connect_to_server(ipBroker, puertoBroker, NULL);
	if(socketGame != -errno){
		log_info(log, "CONEXION EXITOSA CON EL BROKER");
		t_message mensajeBroker;
		void* content = malloc(sizeof(uint32_t));
		uint32_t numero = 1; // es el new pero no se por que no funciona
		memcpy (content, &numero, sizeof(uint32_t));
		mensajeBroker.head = 7; // esto es lo mismo pero para suscripcion no se por que funciona mal eso
		mensajeBroker.content = content;
		mensajeBroker.size = sizeof(uint32_t);
		send_message(socketGame, mensajeBroker.head,mensajeBroker.content, mensajeBroker.size);
	}
	FILE* archivo_pokemon = fopen("patch","wb+");//TODO

}
