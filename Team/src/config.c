#include<commons/config.h>
#include<commons/collections/list.h>
#include "config.h"
#include<commons/string.h>
//tengo muchas dudas pero creo que se usa asi la lib de listas

t_list * list_valores = list_create(); //

int cargar_objetivos(t_entrenador* entrenador) {
	t_config * config;
	if (config = config_create("a.config") == NULL) {
		printf("no se pudo leer el archivo de configuracion \n");
		exit();
	}

	char**list_objetivos = config_get_array_value(config,
			"OBJETIVOS_ENTRENADORES");

	config_destroy(config);

}
int separar_array(char**array) {

}
void separa_valores(char*string_valores, t_list * ptr_valores) { //divide los valores de un arreglo
	long_string = string_length(string_valores);
	int i = 0;
	char*str = "";
	while (i != long_string) {

		while (c != '|') {
			c = string_valores[i];
			string_append(str, c); //a la palabra currente le sigo metiendo letras// ver que lo haga bien
			str++;
		}
		list_add(list_valores, &str);
		str = "";

		i++;
	}
}
