/*
 * deadlock.h
 *
 *  Created on: 24 jun. 2020
 *      Author: utnso
 */

#ifndef SRC_DEADLOCK_H_
#define SRC_DEADLOCK_H_
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "team.h"
#include <commons/collections/list.h>
#include "pokemones.h"
#include "entrenadores.h"


typedef struct {
	t_entrenador*entrenador1;
	t_pokemon*pokemon1; //donante ,pasara a estar en entrenador 2
	t_entrenador*entrenador2;
	t_pokemon*pokemon2;

} t_intercambio;



//------------FUNCIONES----------------
bool detectar_deadlock(t_team*);
void resolver_deadlock(t_team* ,sem_t*);
t_entrenador* buscar_mejor_entrenador_para_intercambio(t_list*,t_entrenador*);
bool hay_ciclo(t_entrenador*,t_entrenador*);
bool quiere_algo_de(t_entrenador* ,t_entrenador* );
t_list* obtener_pokemones_faltantes(t_entrenador* );
t_list* obtener_pokemones_sobrantes(t_entrenador* );
t_entrenador* buscar_entrenador_para_intercambio(t_list* ,t_entrenador* );
void asignar_tarea_intercambio(t_entrenador* , t_entrenador* ,t_team* ,sem_t*);
void asignar_mejor_tarea_intercambio(t_entrenador* , t_entrenador* ,t_team* ,sem_t*);
void realizar_intercambio(t_entrenador* ,t_entrenador* );


#endif /* SRC_DEADLOCK_H_ */
