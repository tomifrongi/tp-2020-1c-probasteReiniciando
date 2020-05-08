#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "team.h"


int cambiar_estado(t_entrenador* entrenador,int estado){
	entrenador->estado = estado;
}

void bloquear_entrenador(t_entrenador entrenador){
	cambiar_estado(entrenador,BLOCK);
}

int cantidad_objetivos(t_entrenador entrenador){
	int i=0;
	t_pokemon* p=entrenador->pokemonesBuscados;
	while(p != NULL){
		p = p->sgte;
		i++;
	}
	return i;
}

int cantidad_pokemones_especie(t_entrenador* entrenador,t_pokemon* pokemon){
	int i=0;
	t_pokemon* p = entrenador->pokemonesBuscados;
	while(p->sgte != NULL){
		if(p == pokemon)
			i++;
		p = p->sgte;
	}
	return i;
}

int necesita_pokemon(t_entrenador* entrenador,t_pokemon pokemon){
	t_pokemon* p = pokemon;
	while(p != NULL){
		if(p==pokemon)
			return 1;
	}
	return 0;
}

void capturarPokemon(t_entrenador* entrenador,t_pokemon* pokemon){
	t_pokemon* p = entrenador->pokemonesBuscados;
	t_pokemon* q = NULL;
	while(pokemon != p->sgte)
		p=p->sgte;
	q = p->sgte
	p->sgte = p->sgte->sgte;
	free(q);
}



int main(){
	return 0;
}
