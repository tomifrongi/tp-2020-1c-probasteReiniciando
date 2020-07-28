
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
	char* especie;
	//cambio de planes tenemos mapa again,pero mantenemos posicion para algunos calculos
	int posicion_x;
	int posicion_y;//todos arrancan en el bolsillo del entrenador

}t_pokemon;

 typedef struct {
	//char* nombre;
	int id;
	int team;//team al que pertenece
	int posicion_x;
	int posicion_y;
	t_list* pokemones_capturados;
	t_list* pokemones_buscados;
	 t_state estado ;//no se puede inicializar en este.h parece
}t_entrenador;


typedef struct{
	int id;
	char* path_config;
	t_list * entrenadores;
	t_list* objetivo_global;
	//t_list*mapa [MAP_SIZE][MAP_SIZE];
	t_list*pokemones_sueltos;
	char*path_logfile;//logfile
	int ip_broker;
	int puerto_broker;
	int tiempo_reconexion;
	int retardo_ciclo_cpu;
	int quantum;
	t_planificador planificador;
	t_list*entrenadores_planificados;
	//contadores finales :
	int cantidad_ciclos_cpu_ejecutados=0;
	int cantidad_deadlocks=0;//mm hace falta
	int cantidad_deadlocks_detectados=0;
	int cantidad_deadlocks_solucionados=0;
	bool conectado_al_broker;
	t_queue cola_localized;
	t_queue cola_appeared;
	t_queue cola_caught;
}t_team;

t_list*lista_teams=list_create();//TODO al main




int cantidad_entrenadores(t_team*);
void agregar_pokemon(t_entrenador* ,t_pokemon* );

t_list * objetivo_personal(t_entrenador*entrenador);
t_list * objetivo_global(t_team*team) ;
int cantidad_por_especie(t_team*,t_pokemon*);
void crear_hilo(t_entrenador*);//no se si es tan asi

t_list * pokemones_sueltos;

#endif /* TEAM_H_ */
