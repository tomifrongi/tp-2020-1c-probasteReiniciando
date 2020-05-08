#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "team.h"



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
/*
int necesita_pokemon(t_entrenador* entrenador,t_pokemon* pokemon){
	char* pokemones = entrenador->pokemonesBuscados;
	for(int i=0;i<strlen(pokemones);i++){
		if(strcmp(pokemones[i],pokemon->nombre))
			return 1;
	}
	return 0;
}*/


int main(void) {
	t_entrenador* ash = malloc(sizeof(t_entrenador));
	t_pokemon* pikachu = malloc(sizeof(t_pokemon));
	t_pokemon* squirtle = malloc(sizeof(t_pokemon));
	ash->nombre="Ash";
	pikachu->especie = "pikachu";
	squirtle->especie = "squirtle";
	agregar_pokemon(ash,pikachu);
	agregar_pokemon(ash,squirtle);
	mostrar_pokemones(ash);
	//printf("%s",pikachu->especie);
	//t_entrenador* mapa[5][5];
	return 0;
}
