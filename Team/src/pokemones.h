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
#include <stdbool.h>




//-----------------FUNCIONES-------------

t_list* intersect_listas_pokemones(t_list*, t_list*);

t_list *get_pokemones(t_team*, int , int );

bool  pokemones_es_misma_especie(t_pokemon*,t_pokemon*);

int cantidad_pokemones_especie(t_list* , char*);

t_list* pokemones_misma_especie(t_list*, char*);

t_list*listar_pokemones(char**);
int team_cantidad_maxima_especie(t_team*, char*);
int team_cantidad_total_especie(t_team*, char*);
bool team_puede_capturar(t_team*team, t_pokemon*);

void mostrar_pokemon(t_pokemon*);

int pokemon_util_a_entrenador(t_entrenador* , t_pokemon *);

//bool pokemon_util_a_team(t_team*,t_pokemon*);

void remover_pokemon(t_list*,t_pokemon*);

void remover_especie_y_destruir(t_list*,char*);

t_pokemon* crear_t_pokemon(t_pokemon);

void borrar_t_pokemon(t_pokemon*);


#endif /* POKEMONES_H_ */
