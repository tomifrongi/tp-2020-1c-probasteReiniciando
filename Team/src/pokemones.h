/*
 * pokemones.h
 *
 *  Created on: 4 jun. 2020
 *      Author: utnso
 */
#include "team.h"


#ifndef POKEMONES_H_
#define POKEMONES_H_

t_list *pokemones_capturados(t_team*team,int index);
t_list *pokemones_buscados(t_team*team,int index);
int cantidad_objetivos(t_entrenador *entrenador);
int cantidad_pokemones_especie(t_list* pokemones, char*mi_especie);
int necesita_pokemon(t_entrenador* entrenador, t_pokemon *pokemon); //ni me acuero que hace
t_list*listar_pokemones(char**array);
void mostrar_pokemon(t_pokemon*pokemon);
void mostrar_pokemones(t_list*pokemones);
#endif /* POKEMONES_H_ */
