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

t_config* config;

int main(void) {

	//Inicio de logger y traer valores del config.

	initConfigLogger();

	char* root_directory = puntoMontaje; //config_get_string_value(config, "PUNTO_MONTAJE_TALLGRASS"); hace lo mismo que puntoMontaje

	chequear_metadata(root_directory);
	leer_metadata(root_directory);
	leer_bitmap(root_directory);

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

//-------Init of Connections------//

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
//-------End of Connections------//


void chequear_existencia_directorio(char* path) {

	int resultado = mkdir(path, 0700);
	if(resultado){
		if(errno == EEXIST){ //Si el error fue que encontro el directorio, continua
			log_debug(log, "Se encontró el directorio: \"%s\". !\n", path);
		} else { //Hubo otro error
			log_error(log, "Hubo un error al encontrar el directorio: \"%s\". !\n", path);
			exit(4);
		}
	} else { //No se creó, entonces hubo un error
		log_error(log, "No se encontró el directorio: \"%s\".\n", path);
		rmdir(path); //Como creé el directorio, lo borro para no dejarlo
		exit(3);
	}
}

void chequear_existencia_archivo(char* path) {

	if(access(path, F_OK) != -1) {
		log_debug(log, "Se encontró el archivo: \"%s\". !\n");
	} else{
		log_error(log, "Hubo un error al encontrar el archivo: \"%s\". !\n");
		exit(5);
	}
}

char *buscar_path(char *root, char *path){

	char *new_directory = string_new();
	string_append(&new_directory, root);
	string_append(&new_directory, path);

	return new_directory;
}

void chequear_metadata(char* root_directory){

	chequear_existencia_directorio(root_directory);

	char* metadata_directory = buscar_path(root_directory, "\Metadata");
	chequear_existencia_directorio(metadata_directory);

	char* metadata_metadata_archive = buscar_path(metadata_directory, "\Metadata.bin");
	chequear_existencia_archivo(metadata_metadata_archive);

	char* files_directory = buscar_path(root_directory, "\Files");
	chequear_existencia_directorio(files_directory);

	char* metadata_files_archive = buscar_path(files_directory, "\Metadata.bin");
	chequear_existencia_archivo(metadata_files_archive);

	char* blocks_directory = buscar_path(root_directory, "\Blocks");
	chequear_existencia_directorio(blocks_directory);

	log_info(log, "Se encontraron todos los archivos de Tall Grass.\n");
	printf("Se encontraron todos los archivos de Tall Grass");

}

void corroborar_existencia(void* resultado) {

	if(resultado == NULL) {
		log_error(log, "Error con resultado NULL");
		exit(7);
	}
}

char* metadata_key_string(char* memoria, int size_memoria, char* key) {

	char* resultado = malloc(sizeof(char*));
	int i, j;
	for(i = 0; i < size_memoria; i++){
		for(j = 0; memoria[i] == key[j]; j++){
			i++;
		}
		if(j == strlen(key)) {
			i++;
			int k;
			for (k = 0; (memoria[i] != '\n') && (memoria[i] != '\0') && (memoria[i] != EOF); k++) {
				resultado[k] = memoria[i];
				i++;
			}
			resultado[k] = '\0';
			return resultado;
		}
	}

	return NULL;
}

uint32_t metadata_key_int(char* memoria, int size_memoria, char* key) {

	char* buscado_en_string = metadata_key_string(memoria, size_memoria, key);
	corroborar_existencia(buscado_en_string);

	return atoi(buscado_en_string);
}

void leer_metadata(char* root) {

	char* metadata_archive_path = buscar_path(root, "\Metadata\Metadata.bin");
	struct stat fb;

	int fd = open(metadata_archive_path, O_RDONLY);
	fstat(fd, &fb);
	char* memoria = mmap(NULL, fb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	t_header* tall_grass = malloc(sizeof(t_header));

	tall_grass->block_size = metadata_key_int(memoria, fb.st_size, "BLOCK_SIZE");
	tall_grass->blocks = metadata_key_int(memoria, fb.st_size, "BLOCKS");
	tall_grass->magic_number = metadata_key_string(memoria, fb.st_size, "MAGIC_NUMBER");
	corroborar_existencia(tall_grass->block_size);

	log_info(log, "HEADER\nBLOCK SIZE = %d\nBLOCKS = %d\nMAGIC NUMBER = %s\n", tall_grass->block_size, tall_grass->blocks, tall_grass->magic_number);

	if(munmap(memoria, fb.st_size) == -1){
		printf("Hubo un error cerrando el mapeo de memoria");
		log_error(log, "Hubo un error cerrando el mapeo de memoria");
		exit(6);
	}
	close(fd);

}

void leer_bitmap(char* root) {

	char* metadata_bitmap = buscar_path(root, "\Metadata\Bitmap.bin");
	struct stat fb;

	int fd = open(metadata_bitmap, O_RDONLY);
	char* bitmap_mmap = mmap(NULL, fb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	if(bitmap_mmap == NULL){
		log_info(log, "El bitmap está vacío");
		//Chequear si es un error
	}

	t_bitarray* bitmap = bitarray_create(bitmap_mmap, fb.st_size);

	//Terminar
}



//-------Init of pokemons------//

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

//-------End of pokemons------//
