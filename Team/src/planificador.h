/*
 * planificador.h
 *
 *  Created on: 24 jun. 2020
 *      Author: utnso
 */

#include "team.h"
#include "configuracion.h"
#include <pthread.h>
#include <semaphore.h>


bool conectado_al_broker;
t_queue* cola_localized;
sem_t* semaforo_contador_localized;
t_queue* cola_appeared;
sem_t* semaforo_contador_appeared;
t_queue* cola_caught;
sem_t* semaforo_contador_caught;
t_list* idsGet;
t_list* idsCatch;
t_list* especiesRecibidas;
pthread_mutex_t* mutex_entrenadores_disponibles;
sem_t* semaforo_entrenadores_disponibles;

t_team* TEAM;

#ifndef SRC_PLANIFICADOR_H_
#define SRC_PLANIFICADOR_H_



#endif /* SRC_PLANIFICADOR_H_ */
