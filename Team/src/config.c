#include "config.h"


#define cantidad_claves_por_team 11
/*
 * Por ahora la idea es que el config devuelva los arreglos
de datos para luego procesarlos en otras funciones
*/
//infiero la cantidad de procesos team en base a que
int cantidad_teams(t_config*config){
	return  config_keys_amount(config)/cantidad_claves_por_team;
}


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


t_config * leer_config() {//sasco el parametro team

	t_config * config = config_create("/home/utnso/git/tp-2020-1c-probasteReiniciando/Team/src/a.config");
	//t_config * config = config_create("a.config"); //ver de que funcione asi //todo

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

