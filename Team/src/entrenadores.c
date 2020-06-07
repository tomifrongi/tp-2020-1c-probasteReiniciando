#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "team.h"
#include "config.h"
#include "pokemones.h"
int cambiar_estado(t_entrenador* entrenador, int estado) {
	entrenador->estado = estado;
	return 0;
}

void bloquear_entrenador(t_entrenador *entrenador) {
	cambiar_estado(entrenador, BLOCK);
}



t_entrenador *iniciar_entrenador(int id, int posicion_x, int posicion_y,
		t_list*pokemones_capturados, t_list*pokemones_buscados) {
	t_entrenador*entrenador = malloc(sizeof(t_entrenador));
	entrenador->id = id;
	entrenador->posicion[0] = posicion_x;
	entrenador->posicion[1] = posicion_y;

	entrenador->estado = NEW;
	entrenador->pokemones_capturados = pokemones_capturados;
	entrenador->pokemones_buscados = pokemones_buscados;

	return entrenador;
}

t_list * inicializar_entrenadores(t_team *team) {
	int cantidad = cantidad_entrenadores(team);


	t_list *entrenadores = list_create();
	int posicion_x, posicion_y;
	char**posicion;

	int id;
	for (int i = 0; i < cantidad; i++) {
		id = i;

		posicion = leer_posicion(team, i); // coordenadas posicion, ejemplo  [1,2]
		posicion_x = atoi(posicion[0]);
		posicion_y = atoi(posicion[1]);

		t_entrenador *entrenador = iniciar_entrenador(id, posicion_x,posicion_y,get_pokemones(team,i,0),get_pokemones(team,i,1)); //el estado lo hago sin pasar parametro por ahora

		list_add(entrenadores, entrenador);
		printf("se cargo el entrenador: %d \n\n", i);

	}

	return entrenadores;
}



void mostrar_entrenador(t_entrenador*entrenador) { //closure
	printf("id: %d\n", entrenador->id);
	printf("estado: %d\n", entrenador->estado);
	printf("posicion x: %d\n", entrenador->posicion[0]);
	printf("posicion y: %d\n", entrenador->posicion[1]);
	printf("\npokemones capturados:\n\n");

	mostrar_pokemones(entrenador->pokemones_capturados);
	printf("\npokemones buscados:\n\n");

	mostrar_pokemones(entrenador->pokemones_buscados);

	printf("\n\n--fin entrenador\n\n");
}
void mostrar_entrenadores(t_list*entrenadores) {
	list_iterate(entrenadores, (void*) mostrar_entrenador);

}


t_list* intersect_listas_pokemones(t_list*list1, t_list*list2) //creo que no hace falta duplicar lista por que solo paso el valor
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
t_list * objetivo_personal(t_entrenador*entrenador) { //los que le faltan por encontrar a cada entrenador
t_list*objetivos = list_create();
return objetivos = intersect_listas_pokemones(entrenador->pokemones_buscados,
		entrenador->pokemones_capturados);
}


t_list* pokemones_sobrantes(t_entrenador*entrenador){
	t_list*lista=entrenador->pokemones_capturados;//a los capturados les saco los que son el objetivo personal y me da los que no necesito...?mmm
	intersect_listas_pokemones (lista,objetivo_personal(entrenador));

		return lista;
}

/*
int main() {
	printf("-----------------main de entrenadores.c\n\n");
	printf("-----------------carga de entrenadores.c\n\n");

	t_team *team = malloc(sizeof(t_team));
	team->entrenadores = inicializar_entrenadores(team);

	printf("-----------------muestra de entrenadores.c\n\n");
	mostrar_entrenadores(team->entrenadores);

	printf("----------------fin main de entrenadores.c\n");



	return 0;

}*/

