#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "team.h"
#include  "entrenadores.h"


void agregar_pokemon(t_entrenador* entrenador,t_pokemon* pokemon){
	t_pokemon* p = entrenador->pokemonesBuscados;
	if(p == NULL){
		entrenador->pokemonesBuscados = pokemon;
	}
	else{
		while(p->sgte != NULL){
			p = p->sgte;
		}
		p->sgte = pokemon;
	}

}

void mostrar_pokemones(t_entrenador* entrenador){
	t_pokemon* p=entrenador->pokemonesBuscados;
	while(p != NULL){
		printf("%s",p->especie);
		p=p->sgte;
	}
}

int cantidad_por_especie(t_team* team,t_pokemon* pokemon){
	int i=0;
	t_entrenador* entrenador = team->entrenadores;
	while(entrenador != NULL){
		i += cantidad_pokemones_especie(entrenador,pokemon);
	}
	return i;
}

t_objetivo* objetivo_global(t_team* team){
	t_objetivo* objetivo = malloc(sizeof(t_objetivo));
	t_objetivo* aux = malloc(sizeof(t_objetivo));
	t_entrenador* entrenador = malloc(sizeof(t_entrenador));
	t_pokemon* pokemon = malloc(sizeof(t_pokemon));
	objetivo->pokemon = entrenador->pokemonesBuscados;
	objetivo->cantidad = cantidad_por_especie(team,objetivo->pokemon);
	objetivo->sgte = aux;
 	while(entrenador != NULL){
 		pokemon = entrenador->pokemonesBuscados;
 		while(pokemon != NULL){
 			aux->pokemon=pokemon;
 			aux->cantidad=cantidad_por_especie(team,aux->pokemon);
 			pokemon = pokemon->sgte;
 			aux = aux->sgte;
 		}
 	}

	return objetivo;
}


int main(void) {
	t_entrenador* ash = malloc(sizeof(t_entrenador));
	t_pokemon* pikachu = malloc(sizeof(t_pokemon));
	t_pokemon* squirtle = malloc(sizeof(t_pokemon));
	ash->nombre="Ash";
	ash->estado=NEW;
	ash->id = 0;
	pikachu->especie = "pikachu";
	squirtle->especie = "squirtle";
	agregar_pokemon(ash,pikachu);
	agregar_pokemon(ash,squirtle);
	mostrar_pokemones(ash);
	//printf("%s",pikachu->especie);
	//t_entrenador* mapa[5][5];
	return 0;
}
