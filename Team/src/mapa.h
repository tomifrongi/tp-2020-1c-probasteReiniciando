#define MAP_SIZE 10
#include <stdbool.h>
#include"team.h"

#ifndef SRC_MAPA_H_
#define SRC_MAPA_H_

//-----------------FUNCIONES----------------
double map_distancia(int ,int ,int ,int );

double distancia_entrenador_pokemon(t_entrenador*,t_pokemon*);

double distancia_entrenador_entrenador(t_entrenador*,t_entrenador*);

double distancia_entrenador_destino(t_entrenador*,int ,int );

bool mapa_hay_pokemon_suelto(t_team*);

t_entrenador*entrenador_mas_cercano(t_list*,t_pokemon*);



#endif /* SRC_MAPA_H_ */
