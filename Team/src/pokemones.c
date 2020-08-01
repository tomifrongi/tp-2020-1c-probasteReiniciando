#include "pokemones.h"

/*------------------------------------------------FUNCIONES QUE LLAMAN LOS ENTRENADORES------------------------------------------------*/

//TODO ACTUALIZAR EN TODOS LOS LADOS QUE USE INTERSECT_LISTAS_POKEMONES
t_list* intersect_listas_pokemones(t_list*list1, t_list*list2)
{
	int i = 0;
	while(i < list_size(list1)) {
		t_pokemon*poke_busc = list_get(list1, i);

		for (int j = 0; j < list_size(list2); j++) {
			t_pokemon*poke_cap = list_get(list2, j);
			if (strcmp(poke_busc->especie,poke_cap->especie)==0 ){
				list_remove(list1, i);
				list_remove(list2,j);
				i = -1;
				break;
			}
		}
		i++;
	}
	return list1;
}
t_list *get_pokemones(t_team*team, int id_entrenador, int tipo) { //0 para capturados 1 para buscados//HACER UN ENUM PARA ESTO
	char**pokemones = leer_pokemones(team, tipo);
	int ultima_posicion = largo_array(pokemones)-1;
	if(id_entrenador <= ultima_posicion)
		return listar_pokemones(pokemones[id_entrenador]);
	else
		return list_create();
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
		pokemon->posicion_x = -1;
		pokemon->posicion_y = -1;
		int longitud = strlen(pokemones[i])+1;
		pokemon->especie = malloc(longitud);
		strcpy(pokemon->especie,pokemones[i]);
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

//int pokemon_util_a_entrenador(t_entrenador* entrenador, t_pokemon *pokemon) { //medio spagetti pero creo que la voy a usar otra vez
//	bool foo(t_pokemon*p){
//		return misma_especie(p,pokemon);
//	}
//	list_any_satisfy(objetivo_personal(entrenador),foo);
//	return 0;
//}


//bool pokemon_util_a_team(t_team*team,t_pokemon*pokemon){
//	bool es_util(t_entrenador*entrenador){
//		return pokemon_util_a_entrenador(entrenador,pokemon);
//	}
//	//t_list*entrenadores=team->entrenadores;       //no se por que rompe la verga
//	return list_any_satisfy(team->entrenadores,es_util);
//}

void remover_pokemon(t_list* pokemones,t_pokemon* pokemon){
	bool mismo_nombre_p(void* p){
		t_pokemon* p_1 = p;
		return (strcmp(p_1->especie, pokemon->especie) == 0);
	}

	list_remove_by_condition(pokemones,mismo_nombre_p);
}

void remover_especie_y_destruir(t_list* especies,char* nombre_especie){
	bool mismo_nombre_pp(void* e){
		char* especie = e;
		return (strcmp(especie, nombre_especie) == 0);
	}
	char* especie_encontrada = list_remove_by_condition(especies,mismo_nombre_pp);
	free(especie_encontrada);
}

t_pokemon* crear_t_pokemon(t_pokemon pokemon){
	t_pokemon* pokemon_creado = malloc(sizeof(t_pokemon));
	pokemon_creado->especie = malloc(strlen(pokemon.especie)+1);
	pokemon_creado->posicion_x = pokemon.posicion_x;
	pokemon_creado->posicion_y = pokemon.posicion_y;
	strcpy(pokemon_creado->especie,pokemon.especie);
	return pokemon_creado;
}

void borrar_t_pokemon(t_pokemon* pokemon){
	free(pokemon->especie);
	free(pokemon);
}

int team_cantidad_maxima_especie(t_team*team, char*especie) { // team no puede atrapar mas de una especie de los requerido globalmente

	int cantidad = 0;
	void sumar_cantidad_por_entrenador(t_entrenador*entrenador) {
		cantidad += cantidad_pokemones_especie(entrenador->pokemones_buscados, especie);
	}
	list_iterate(team->entrenadores, sumar_cantidad_por_entrenador);

	return cantidad;
}
int team_cantidad_total_especie(t_team*team, char*especie) { //cantidad de especie que ttiene el team globalmente
	int cantidad = 0;
	void sumar_cantidad_por_entrenador(t_entrenador*entrenador) {
		cantidad += cantidad_pokemones_especie(entrenador->pokemones_capturados, especie);
	}
	list_iterate(team->entrenadores, sumar_cantidad_por_entrenador);

	return cantidad;
}
bool team_puede_capturar(t_team*team, t_pokemon*pokemon) {
	return team_cantidad_maxima_especie(team, pokemon->especie) > team_cantidad_total_especie(team, pokemon->especie);
}

