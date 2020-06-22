#ifndef GAMECARD_H__
#define GAMECARD_H__

#include "commons/config.h"
#include "commons/log.h"
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include "ProtocoloDeMensajes.h"
#include <pthread.h>
#include "funcionesEnvio.h"
#include "time.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>


#define MAX_CLIENTS 128

t_log* log;

typedef struct{
	int cord_x;
	char guion;
	int cord_y;
	char igual;
	int cantidad;
}t_linea;


typedef struct{
	uint32_t block_size;
	uint32_t blocks;
	char* magic_number;
}t_header;


int tiempoReintentoConexion;
int tiempoReintentoOperacion;
int tiempoRetardoOperacion;
char* puntoMontaje;
char* ipBroker;
int puertoBroker;

void* handler_suscripciones(uint32_t cola);
void initConfigLogger();
int cantidad_pokemones(FILE* archivo_pokemon);
void existen_posiciones_pokemon_nuevo(FILE* archivo_pokemon,new_pokemon pokemon_nuevo);
void existen_posiciones_pokemon_atrapado(FILE* archivo_pokemon,catch_pokemon pokemon_atrapado);
void decrementar_cantidad(FILE* archivo_pokemon,catch_pokemon pokemon_atrapado);
void eliminarLinea(FILE* archivo_pokemon,t_linea linea_a_borrar);
t_list* obtener_posiciones_y_cantidades(FILE* archivo_pokemon);

// Chequear, lee: archivos, directorios

void chequear_metadata(char* root_directory);
void leer_metadata(char* root);
void leer_bitmap(char* root);
void chequear_existencia_directorio(char* path);
void chequear_existencia_archivo(char* path);
char* buscar_path(char *root, char *path);
void corroborar_existencia(void* resultado);
char* metadata_key_string(char* memoria, int size_memoria, char* key);
uint32_t metadata_key_int(char* memoria, int size_memoria, char* key);



#endif
/* GAMECARD_H_ */
