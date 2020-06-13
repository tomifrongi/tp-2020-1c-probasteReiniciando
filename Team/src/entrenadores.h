

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include "team.h"
#include "config.h"
#include "pokemones.h"

#ifndef ENTRENADORES_H_
#define ENTRENADORES_H_

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT
}t_state;

bool esta_disponible(t_entrenador*);

void cambiar_estado(t_entrenador*,t_state);

void bloquear_entrenador(t_entrenador*);

void capturar_pokemon(t_entrenador*,t_pokemon*);

int cant_pokemones_especie(t_entrenador*,t_pokemon*);

int cantidad_objetivos(t_entrenador*);

int puede_atrapar(t_entrenador*,t_pokemon*);

int necesita_pokemon(t_entrenador*,t_pokemon*);


#endif /* ENTRENADORES_H_ */

