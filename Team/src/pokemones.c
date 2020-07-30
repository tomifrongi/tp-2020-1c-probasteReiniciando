#include "pokemones.h"

/*------------------------------------------------FUNCIONES QUE LLAMAN LOS ENTRENADORES------------------------------------------------*/

t_list* intersect_listas_pokemones(t_list*list1, t_list*list2)
{
for (int i = 0; i < list_size(list1); i++) {
	t_pokemon*poke_busc = list_get(list1, i);

	for (int j = 0; j < list_size(list2); j++) {
		t_pokemon*poke_cap = list_get(list2, j);
		if (misma_especie(poke_busc, poke_cap)==true ){
			list_remove(list1, i);
			list_remove(list2,j);

		}
	}
}
return list1;

}
t_list *get_pokemones(t_team*team, int id_entrenador, int tipo) { //0 para capturados 1 para buscados//HACER UN ENUM PARA ESTO
	char**pokemones = leer_pokemones(team, tipo);
	return listar_pokemones(pokemones[id_entrenador]);
}

bool  pokemones_es_misma_especie(t_pokemon*p1,t_pokemon*p2){   //SE USA ALFINAL?? parece que  si
	return string_equals_ignore_case(p1->especie, p2->especie);

}
int cantidad_pokemones_especie(t_list* pokemones, char*mi_especie) {

	bool _es_especie(t_pokemon * p) {
		char* especie = p->especie;
		return string_equals_ignore_case(mi_especie, (char*) especie);
	}
	return list_size(list_filter(pokemones, (t_pokemon *) _es_especie));

}

t_list* pokemones_misma_especie(t_list* pokemones, char*mi_especie) {

	bool _es_especie(t_pokemon * p) {
		char* especie = p->especie;
		return string_equals_ignore_case(mi_especie, (char*) especie);
	}
	return list_filter(pokemones, (t_pokemon *) _es_especie);

}
/*
/*------------------------------------------------CARGA E INICIALIZACION DE LOS POKEMONES------------------------------------------------*/

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




/*------------------------------------------------MUESTRA POR PANTALLA------------------------------------------------*/

void mostrar_pokemon(t_pokemon*pokemon) { //closure

	printf("especie :%s\n", pokemon->especie);
	//printf("posicion :[%d,%d]\n", pokemon->posicion[0], pokemon->posicion[1]);

}
void mostrar_pokemones(t_list*pokemones) {

	list_iterate(pokemones, (void*) mostrar_pokemon);
}
/*------------------------------------------------VERIFICACION DE POKEMONES UTILES A OBJETIVOS------------------------------------------------*/

int pokemon_util_a_entrenador(t_entrenador* entrenador, t_pokemon *pokemon) { //medio spagetti pero creo que la voy a usar otra vez
	bool foo(t_pokemon*p){
		return misma_especie(p,pokemon);
	}
	list_any_satisfy(objetivo_personal(entrenador),foo);
	return 0;
}


bool pokemon_util_a_team(t_team*team,t_pokemon*pokemon){
	bool es_util(t_entrenador*entrenador){
		return pokemon_util_a_entrenador(entrenador,pokemon);
	}
	//t_list*entrenadores=team->entrenadores;       //no se por que rompe la verga
	return list_any_satisfy(team->entrenadores,es_util);
}



