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


typedef struct {
	t_entrenador*entrenador1;
	t_pokemon*pokemon1; //donante ,pasara a estar en entrenador 2
	t_entrenador*entrenador2;
	t_pokemon*pokemon2;

} t_intercambio;

void matriz_adyacencia(t_team*);


#endif /* SRC_DEADLOCK_H_ */
