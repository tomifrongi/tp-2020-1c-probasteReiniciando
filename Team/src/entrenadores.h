
#ifndef ENTRENADORES_H_
#define ENTRENADORES_H_

#include "team.h"



void cambiar_estado(t_entrenador*,t_state);

void bloquear_entrenador(t_entrenador*);

void capturar_pokemon(t_entrenador*,t_pokemon*);

int cant_pokemones_especie(t_entrenador*,t_pokemon*);


int cantidad_objetivos(t_entrenador*);

int puede_atrapar(t_entrenador*,t_pokemon*);

int necesita_pokemon(t_entrenador*,t_pokemon*);


#endif /* ENTRENADORES_H_ */

