#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>

#include <stdlib.h>
#include<commons/string.h>
#include "config.h"
#include "entrenadores.h"

void cargar_objetivos(t_entrenador* entrenador) {
	t_config * config = config_create("a.config");
	if (config == NULL) {
		printf("no se pudo leer el archivo de configuracion \n");
		exit(1);
	}

	char**array_objetivos = config_get_array_value(config,
			"OBJETIVOS_ENTRENADORES");
	//t_list * list_valores = list_create();
	t_queue * valores_pokemones =queue_create();
	separar_valores(&array_objetivos,list_valores);
	while (queue_is_empty(valores_pokemones)==0){
		entrenador->pokemonesBuscados->especie = queue_pop(valores_pokemones);
		entrenador->pokemonesBuscados=entrenador->pokemonesBuscados->sgte ;
	}
	config_destroy(config);

}
int separar_array(char**array) {
	return 0;
}
void separar_valores(char*string_valores, t_list * list_valores) { //divide los valores de un arreglo del config
	int long_string = string_length(string_valores);
	int i = 0;
	char*str = "";
	char c;
	while (i != long_string) {

		while (c != '|') {
			c = string_valores[i];
			string_append(str, c); //a la palabra currente le sigo metiendo letras// ver que lo haga bien
			string_valores++;
		}
		list_add(list_valores, str);
		str = "";

		i++;
	}
}
