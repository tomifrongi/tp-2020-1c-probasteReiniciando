

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
	coordenada posicion_actual;
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


bool esta_disponible(t_entrenador*);

void cambiar_estado(t_entrenador*,t_state);

void bloquear_entrenador(t_entrenador*);

void capturar_pokemon(t_entrenador*,t_pokemon*);

int cant_pokemones_especie(t_entrenador*,t_pokemon*);

int cantidad_objetivos(t_entrenador*);

int puede_atrapar(t_entrenador*,t_pokemon*);

int necesita_pokemon(t_entrenador*,t_pokemon*);

int calcular_rafagas_necesarias(t_entrenador*);

void actualizar_estimados(t_entrenador*,int*,int*);



#endif /* ENTRENADORES_H_ */

