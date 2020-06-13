#define MAP_SIZE 10
#include <stdbool.h>
#include"team.h"

#ifndef SRC_MAPA_H_
#define SRC_MAPA_H_



double map_distancia(int p1_x,int p1_y,int p2_x,int p2_y);

double distancia_entrenador_pokemon(t_entrenador*entrenador,t_pokemon*pokemon);

bool mapa_hay_pokemon_suelto(t_team*team);


#endif /* SRC_MAPA_H_ */
