
#ifndef TEAM_H_
#define TEAM_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <math.h>
#include <semaphore.h>

typedef enum {
	FIFO,
	RR,
	SJF_CD,
	SJF_SD
}t_planificador;

//2 squirtle y 1 pikachu

typedef struct{
	t_list* entrenadores; //esta lista contiene todos los entrenadores siempre, no se sacan ni se agregan
	t_list* hilos_entrenadores;
	t_list* entrenadores_ready; //los que estan en READY
	t_list* entrenadores_desocupados; //toodos los entrenadores arrancan en esta lista ya que estan desocupados
									  //no consideramos que algun entrenador arranque sin poder capturar mas pokemones o
										// arranque ya habiendo cumplido su objetivo
									  //esto tiene sentido con el diagrama de estados. arrancan todos en new (enunciado) y solo pueden pasar a ready

	t_list* objetivo_pokemones_restantes; //tiene toodos los pokemones que le falta al team y puede haber repetidos
										 //si se captura un pokemon, hay que sacarlo de esta lista.
										//si se le asigno a un entrenador, capturar un pokemon de esta lista, se lo saca de la lista
													//si no se lo pudo capturar se lo vuelve a agregar
										// es una lista de char*, no de t_pokemones
	t_list* mapa_pokemones;
	t_planificador planificador;
	//contadores finales :break;
	int cantidad_cambios_de_contexto;
	bool hubo_deadlocks;
	bool conectado_al_broker;

}t_team;

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT
}t_state;

typedef enum {
	ATRAPAR,
	INTERCAMBIO
}t_tipo_tarea;




typedef struct{
	int id;
//	t_team* team;
	int posicion_x;
	int posicion_y;
	t_list* pokemones_capturados; //lista de t_pokemon, por mas que los tenga en el bolsillo, dejamos la posicion
	t_list* pokemones_buscados; //lista de t_pokemon, por mas que los tenga en el bolsillo, dejamos la posicion
	t_state estado;
	struct t_tarea* tarea;
	sem_t* semaforo;
	double estimado_rafaga_anterior;
	double estimado_rafaga_proxima;
	int real_rafaga_anterior;

	bool esta_en_entrada_salida;
	int id_correlativo_esperado;
	int rafagas_intercambio_realizadas;
	bool esperando_intercambio;
	int ciclos_cpu_realizados;
}t_entrenador;

typedef struct{
	char* especie;
	int posicion_x;
	int posicion_y;
}t_pokemon;

struct t_tarea{
	t_tipo_tarea tipo_tarea;
	t_pokemon* pokemon;
	t_entrenador* entrenador_intercambio;
	t_pokemon* pokemon_a_pedir;
	t_pokemon* pokemon_a_otorgar;
};




int cantidad_entrenadores(t_team*);
void agregar_pokemon(t_entrenador* ,t_pokemon* );

t_list * objetivo_personal(t_entrenador*entrenador);
t_list * objetivo_global(t_team*team) ;
int cantidad_por_especie(t_team*,t_pokemon*);
void crear_hilo(t_entrenador*);//no se si es tan asi

t_list * team_get_objetivo_global(t_team*);



t_list*team_entrenadores_disponibles(t_team*);
bool team_cumplio_objetivo_global(t_team*);
void team_verificar_finalizacion(t_team*);
void consumir_ciclos_cpu(t_team*,int);

bool algunos_pueden_atrapar(t_team*);

//Devuelve un lista de posiciones de alguna especie objetivo que estan en el mapa. Si no encuentra devuelve NULL
t_list* buscar_especie_objetivo_en_mapa(t_team*);
void ordenar_entrenadores_planificados_por_estimacion(t_team*);

t_list * pokemones_sueltos;

#endif /* TEAM_H_ */
