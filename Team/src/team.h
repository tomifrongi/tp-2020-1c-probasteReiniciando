
#ifndef TEAM_H_
#define TEAM_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <math.h>
#include "entrenadores.h"

typedef enum {
	FIFO,
	RR,
	SJF_CD,
	SJF_SD
}t_planificador;

//2 squirtle y 1 pikachu

typedef struct{
	t_list* entrenadores; //esta lista contiene todos los entrenadores siempre, no se sacan ni se agregan
	t_list* entrenadores_planificados; //los que estan en READY
	t_list* entrenadores_desocupados; //toodos los entrenadores arrancan en esta lista ya que estan desocupados

	t_list* objetivo_pokemones_restantes; //tiene toodos los pokemones que le falta al team y puede haber repetidos
							//si se captura un pokemon, hay que sacarlo de esta lista.
	t_list* mapa_pokemones;
	t_planificador planificador;

	//contadores finales :break;
	int cantidad_ciclos_cpu_ejecutados;
	int cantidad_deadlocks;//mm hace falta
	int cantidad_deadlocks_detectados;
	int cantidad_deadlocks_solucionados;

}t_team;




int cantidad_entrenadores(t_team*);
void agregar_pokemon(t_entrenador* ,t_pokemon* );

t_list * objetivo_personal(t_entrenador*entrenador);
t_list * objetivo_global(t_team*team) ;
int cantidad_por_especie(t_team*,t_pokemon*);
void crear_hilo(t_entrenador*);//no se si es tan asi

t_list * team_get_objetivo_global(t_team*);

int team_cantidad_maxima_especie(t_team*, char*);
int team_cantidad_total_especie(t_team*, char*);
bool team_puede_capturar(t_team*team, t_pokemon*);
t_list*team_entrenadores_disponibles(t_team*);
bool team_cumplio_objetivo_global(t_team*);
void team_verificar_finalizacion(t_team*);
void consumir_ciclos_cpu(t_team*,int);

bool algunos_pueden_atrapar(t_team*);


t_list * pokemones_sueltos;

#endif /* TEAM_H_ */
