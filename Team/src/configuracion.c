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


void cargar_teams(){
	for (int i=0;i<CANTIDAD_TEAMS;i++){//todo testear!
		char*path="team";
		char*tipo=".config";

		t_team *team=malloc(sizeof(t_team));
		team->id=i;
		path[4]=i;
		team->path_config=strcat(path,tipo);

		list_add(lista_teams,team);
	}
}

t_config * leer_config() {//sasco el parametro team

	t_config * config = config_create("./src/a.config");
	//t_config * config = config_create(team->path_config); //ver de que funcione asi //todo

	if (config == NULL) {
		printf("no se pudo leer el archivo de configuracion \n");
		exit(1);
	}
	return config;
}

char** leer_pokemones(t_team* team, int tipo) { //todos los del config tipo 0 para los que tiene 1 para los objetivos
	t_config *config = leer_config(team);
	char**array_pokemones;
	switch (tipo) {
	case 0:
		array_pokemones = config_get_array_value(config,
				"POKEMON_ENTRENADORES");
		break;
	case 1:
		array_pokemones = config_get_array_value(config,
				"OBJETIVOS_ENTRENADORES");
		break;
	default:
		printf("tipo de lectura erroneo"), exit(1);
	}

	return array_pokemones;
}
char** leer_posicion(t_team* team, int num_entrenador) {
	t_config *config = leer_config(team);
	char**array_posiciones = config_get_array_value(config,
			"POSICIONES_ENTRENADORES");

	char**posicion = separar_valores_de_string(
			array_posiciones[num_entrenador]); // separa el "1|2" en posicion[1,2]

	return posicion;
}

int cantidad_entrenadores(t_team*team) { //al.h
	t_config *config = leer_config(team);
		char**array_posiciones = config_get_array_value(config,
				"POSICIONES_ENTRENADORES");
	return largo_array(array_posiciones);
}

void cargar_configuracion(){

	config_team = leer_config();//TODO solo se lee una vez, vi que arriba leen varias veces, estarian creando varios config.

	//TODO FALTAN 3 config_get


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
