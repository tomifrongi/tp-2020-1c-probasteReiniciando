#include<commons/config.h>
#include<commons/string.h>
#include<commons/collections/list.h>
#include "config.h"
#include "pokemones.h"
#include "team.h"
#include "entrenadores.h"
#include <stdbool.h>
//

t_list *get_pokemones(t_team*team, int id_entrenador, int tipo) { //0 para capturados 1 para buscados
	char**pokemones = leer_pokemones(team, tipo);
	return listar_pokemones(pokemones[id_entrenador]);
}
bool  misma_especie(t_pokemon*p1,t_pokemon*p2){
	return string_equals_ignore_case(p1->especie, p2->especie);

}
t_list*listar_pokemones(char**array) { //el string p1|p2|p3 lo pone cada uno en lista

	t_list * lista = list_create();
	char**pokemones = separar_valores_de_string(array);
	int largo = largo_array(pokemones);
	for (int i = 0; i < largo; i++) {
		t_pokemon * pokemon = malloc(sizeof(t_pokemon));
		pokemon->especie = pokemones[i];
		list_add(lista, pokemon);
	}
	return lista;
}

int cantidad_objetivos(t_entrenador *entrenador) {

	return list_size(entrenador->pokemones_buscados);
}

int cantidad_pokemones_especie(t_list* pokemones, char*mi_especie) {

	bool _es_especie(t_pokemon * p) {
		char* especie = p->especie;
		return string_equals_ignore_case(mi_especie, (char*) especie);
	}
	return list_size(list_filter(pokemones, (t_pokemon *) _es_especie));

}

int pokemon_es_util(t_entrenador* entrenador, t_pokemon *pokemon) { //medio spagetti pero creo que la voy a usar otra vez
	bool foo(t_pokemon*p){
		return misma_especie(p,pokemon);
	}
	list_any_satisfy(objetivo_personal(entrenador),foo);
	return 0;
}

void mostrar_pokemon(t_pokemon*pokemon) { //closure

	printf("especie :%s\n", pokemon->especie);
	//printf("posicion :[%d,%d]\n", pokemon->posicion[0], pokemon->posicion[1]);

}
void mostrar_pokemones(t_list*pokemones) {

	list_iterate(pokemones, (void*) mostrar_pokemon);

}
bool pokemon_es_util(t_team*team,t_pokemon*pokemon){
	bool es_util(t_entrenador*entrenador){
		return pokemon_es_util(entrenador,pokemon);
	}
	//t_list*entrenadores=team->entrenadores;       //no se por que rompe la verga
	return list_any_satisfy(team->entrenadores,es_util);
}



