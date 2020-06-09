#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#define MAP_SIZE 10


#ifndef TEAM_H_
#define TEAM_H_


typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT
}t_state;

typedef struct{
	char* especie;
	//cambio de planes tenemos mapa again,pero mantenemos posicion para algunos calculos
	int posicion[2];//todos arrancan en el bolsillo del entrenador
}t_pokemon;

 typedef struct {
	//char* nombre;
	int id;
	int posicion [2];//[x,y]
	t_list* pokemones_capturados;
	t_list* pokemones_buscados;
	t_state estado ;//no se puede inicializar en este.h parece
}t_entrenador;


typedef struct{
	//char* nombre;
	t_list * entrenadores;
	t_list* objetivo_global;
	t_list*mapa [MAP_SIZE][MAP_SIZE];
}t_team;

/*//creo que va a ser una funcion que llama al entrenador
typedef struct{
	t_pokemon pokemon;
	int cantidad;
	//struct t_objetivo* sgte;
}t_objetivo;*/


int cantidad_entrenadores(t_team*);
void agregar_pokemon(t_entrenador* ,t_pokemon* );

t_list * objetivo_personal(t_entrenador*entrenador);
t_list * objetivo_global(t_team*team) ;
int cantidad_por_especie(t_team*,t_pokemon*);
void crear_hilo(t_entrenador*);//no se si es tan asi

t_list * pokemones_sueltos;

#endif /* TEAM_H_ */
