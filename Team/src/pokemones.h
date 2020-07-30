/*
 * pokemones.h
 *
 *  Created on: 4 jun. 2020
 *      Author: utnso
 */
#ifndef POKEMONES_H_
#define POKEMONES_H_

#include<commons/config.h>
#include<commons/string.h>
#include<commons/collections/list.h>
#include "configuracion.h"
#include "team.h"
#include "entrenadores.h"
#include <stdbool.h>

typedef struct{
	char* especie;
	int posicion_x;
	int posicion_y;
}t_pokemon;


//-----------------FUNCIONES-------------

t_list* intersect_listas_pokemones(t_list*, t_list*);

t_list *get_pokemones(t_team*, int , int );

bool  pokemones_es_misma_especie(t_pokemon*,t_pokemon*);

int cantidad_pokemones_especie(t_list* , char*);

t_list* pokemones_misma_especie(t_list*, char*);

t_list*listar_pokemones(char**);

void mostrar_pokemon(t_pokemon*);

int pokemon_util_a_entrenador(t_entrenador* , t_pokemon *);

bool pokemon_util_a_team(t_team*,t_pokemon*);






#endif /* POKEMONES_H_ */
