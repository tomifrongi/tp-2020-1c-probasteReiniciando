
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

typedef struct{
//	int id;
//	char* path_config;
	t_list * entrenadores;
	t_list* objetivo_global;
	//t_list*mapa [MAP_SIZE][MAP_SIZE];
	t_list*pokemones_sueltos;
//	char*path_logfile;//logfile
//	int ip_broker;
//	int puerto_broker;
//	int tiempo_reconexion;
//	int retardo_ciclo_cpu;
//	int quantum;
	t_planificador planificador;
	t_list*entrenadores_planificados;
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




t_list * pokemones_sueltos;

#endif /* TEAM_H_ */
