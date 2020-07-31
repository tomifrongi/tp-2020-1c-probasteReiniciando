
#ifndef ENTRENADORES_H_
#define ENTRENADORES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "team.h"
#include "mapa.h"
#include "configuracion.h"
#include "ProtocoloDeMensajes.h"







//--------FUCIONES------

t_entrenador *iniciar_entrenador(int , int , int , t_list*, t_list*);

t_list * inicializar_entrenadores(t_team*);

void mostrar_entrenador(t_entrenador*);

void ejecutar_ciclo_cpu(t_entrenador*);


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

void mover_entrenador_una_posicion(t_entrenador*, int , int);

void mover_entrenador(t_entrenador*, int , int );

int calcular_rafagas_necesarias(t_entrenador* );

void actualizar_estimados(t_entrenador* ,int ,int);

t_entrenador* buscar_entrenador_por_id_correlativo(t_list*,int);

t_entrenador* buscar_entrenador_mas_cercano(t_list*,t_pokemon*);

void asignar_tarea_atrapar(t_entrenador* entrenador,t_pokemon* pokemon,sem_t* semaforo_readys);

void remover_entrenador(t_list*,t_entrenador*);

#endif /* ENTRENADORES_H_ */

