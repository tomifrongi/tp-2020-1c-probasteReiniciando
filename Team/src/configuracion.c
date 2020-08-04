#include "configuracion.h"
#define CANTIDAD_TEAMS 5




char** separar_valores_de_string(char*string_valores) { //divide los pokemones de un string campo del arreglo del config y lo pone en un array

	char**arreglo_palabras = string_split(string_valores, "|");
	return arreglo_palabras;

}
int largo_array(char**array) {
	int largo = 0;
	while (array[largo] != NULL) {
		largo++;
	}
	return largo;
}




t_config * leer_config() {//saco el parametro team

	t_config * config = config_create("./src/a.config"); //TODO CAMBIAR DE CARPETA EL CONFIG
														//TODO CAMBIAR EL LOG DE CARPETA
	if (config == NULL) {
		printf("no se pudo leer el archivo de configuracion \n");
		exit(1);
	}
	return config;
}

char** leer_pokemones(t_team* team, int tipo) { //todos los del config tipo 0 para los que tiene 1 para los objetivos
	char**array_pokemones;
	switch (tipo) {
	case 0:
		array_pokemones = POKEMON_ENTRENADORES;
		break;
	case 1:
		array_pokemones = OBJETIVOS_ENTRENADORES;
		break;
	default:
		break;
	}

	return array_pokemones;
}
char** leer_posicion(t_team* team, int num_entrenador) {
	char**array_posiciones = POSICIONES_ENTRENADORES;
	char**posicion = separar_valores_de_string(array_posiciones[num_entrenador]); // separa el "1|2" en posicion[1,2]

	return posicion;
}

int cantidad_entrenadores(t_team*team) {
	return largo_array(POSICIONES_ENTRENADORES);
}

void cargar_configuracion(){

	config_team = leer_config();

	POSICIONES_ENTRENADORES = config_get_array_value(config_team,"POSICIONES_ENTRENADORES");
	POKEMON_ENTRENADORES = config_get_array_value(config_team,"POKEMON_ENTRENADORES");
	OBJETIVOS_ENTRENADORES = config_get_array_value(config_team,"OBJETIVOS_ENTRENADORES");

	TIEMPO_RECONEXION = config_get_int_value(config_team, "TIEMPO_RECONEXION");
	RETARDO_CICLO_CPU = config_get_int_value(config_team, "RETARDO_CICLO_CPU");
	ALGORITMO_PLANIFICACION = config_get_string_value(config_team,"ALGORITMO_PLANIFICACION");
	QUANTUM = config_get_int_value(config_team, "QUANTUM");
	ALPHA = config_get_int_value(config_team, "ALPHA");
	IP_BROKER = config_get_string_value(config_team,"IP_BROKER");
	ESTIMACION_INICIAL = config_get_int_value(config_team, "ESTIMACION_INICIAL");
	PUERTO_BROKER = config_get_int_value(config_team, "PUERTO_BROKER");
	LOG_FILE = config_get_string_value(config_team,"LOG_FILE");

	IP_TEAM = config_get_string_value(config_team,"IP_TEAM");
	PUERTO_TEAM = config_get_int_value(config_team, "PUERTO_TEAM");
}

void inicializar_logger(){
	log_team_oficial =  log_create(LOG_FILE, "Team", 1, LOG_LEVEL_INFO);
}
