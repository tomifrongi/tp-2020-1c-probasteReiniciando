

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
//#include "team.h"
#include "configuracion.h"
#include "pokemones.h"
#include "ProtocoloDeMensajes.h"

#ifndef ENTRENADORES_H_
#define ENTRENADORES_H_

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT
}t_state;

typedef enum {
	ATRAPAR,
	BUSCAR_ENTRENADOR,
	INTERCAMBIO
}t_tipo_tarea;

typedef struct {
	t_tipo_tarea tipo_tarea;
	t_pokemon pokemon;
	t_entrenador entrenador_intercambio;
}t_tarea;

typedef struct {
	int id;
	t_team* team;
	//coordenada posicion_actual; esto efea
	int posicion_x;
	int posicion_y;
	t_list* pokemones_capturados;
	t_list* pokemones_buscados;
	t_state estado;
	t_tarea* tarea;
	sem_t* semaforo;

	int estimado_rafaga_anterior;
	int estimado_rafaga_proxima;
	int real_rafaga_anterior;

	bool esta_en_entrada_salida;
	int id_correlativo_esperado;
	int rafagas_intercambio_realizadas;
}t_entrenador;

//--------FUCIONES------

t_entrenador *iniciar_entrenador(int , int , int , t_list*, t_list*);

t_list * inicializar_entrenadores(t_team*);

void mostrar_entrenador(t_entrenador*);

void mostrar_entrenadores(t_list*);

void cambiar_estado(t_entrenador* , t_state );

void entrenador_bloquear(t_entrenador *);

bool entrenador_cumplio_objetivos(t_entrenador *);

int entrenador_finalizar(t_entrenador *);

bool entrenador_finalizo(t_entrenador*);

bool entrenador_desocupado(t_entrenador*);

bool disponible_para_planificar(t_entrenador*);

t_list * entrenador_pokemones_faltantes(t_entrenador*);

t_list* entrenador_pokemones_sobrantes(t_entrenador*);

int entrenador_cantidad_objetivos(t_entrenador *);

int entrenador_cantidad_capturados(t_entrenador *);

bool entrenador_puede_capturar(t_entrenador*);

void mover_entrenador(t_entrenador*, int , int );

int calcular_rafagas_necesarias(t_entrenador* );

void actualizar_estimados(t_entrenador* ,int* ,int*);

#endif /* ENTRENADORES_H_ */

