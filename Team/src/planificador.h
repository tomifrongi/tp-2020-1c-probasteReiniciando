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
#include <stdint.h>
#include "mapa.h"
#include "conexiones.h"
#include "ProtocoloDeMensajes.h"
#include "entrenadores.h"
#include "pokemones.h"

typedef struct{
	t_pokemon* pokemon;
	t_entrenador* entrenador;
	int* distancia;
}t_distancia_pokemon_entrenador;


t_queue* cola_localized;
sem_t* semaforo_contador_localized;
pthread_mutex_t* mutex_cola_localized;

t_queue* cola_appeared;
sem_t* semaforo_contador_appeared;
pthread_mutex_t* mutex_cola_appeared;

t_queue* cola_caught;
sem_t* semaforo_contador_caught;
pthread_mutex_t* mutex_cola_caught;

pthread_mutex_t* mutex_idsGet; //lista de int
t_list* idsGet;

pthread_mutex_t* mutex_idsCatch; //lista de int
t_list* idsCatch;

pthread_mutex_t* mutex_entrenadores_ready;
sem_t* semaforo_entrenadores_ready;

t_list* especiesRecibidas;
pthread_mutex_t* mutex_especiesRecibidas;

sem_t* semaforo_termino_rafaga_cpu;

t_team* TEAM;

pthread_mutex_t* mutex_planificar_entrenador;

#ifndef SRC_PLANIFICADOR_H_
#define SRC_PLANIFICADOR_H_

//------------FUNCIONES-------------


//void* crear_hilos_entrenadores(team);


void planificar_entrenador(t_team *);

int tiempo_rafaga(t_entrenador*, t_pokemon*);

bool contain_id_get(t_list*, int);

bool verificar_nuevo_localized(t_team* , t_pokemon* ,uint32_t );

void* procesar_localized(void*);

bool contain_especie_recibida(t_list*,t_pokemon*);

bool team_necesita_especie(t_team* , t_pokemon*);

bool team_necesita_especie(t_team* , t_pokemon* );

bool verificar_nuevo_appeared(t_team*,t_pokemon*);

void* procesar_appeared(void*);

void* procesar_caught(void*);

void* handler_entrenador(void*);

bool sem_post_algoritmo(t_entrenador* ,t_list*);

void agregar_entrenador_a_cola_ready(t_entrenador* ,t_team* );

void planificar_team(t_team*);


void borrar_int_t_distancia_pokemon_entrenador(t_list* distancias);
void ordenar_t_distancia(t_list* distancias);





/*

//planificar team
activar_proceso_reconexion(); //un thread
abrir_socket_gameboy(); //marcos
recibir_mensajes_gameboy( socket_gameboy); //marcos
team_inicializar( team);
crear_pokemon();
planificar_entrenador( team, pokemon);
verificar_deadlock( team);
salvar_deadlocks( team);
//suscribirse a colas                                                                     ESTO VUELA
suscribirse_apparead(); //marcos
suscribirse_caught(); //marcos
suscribirse_localized(); //marcos
//pedir_pokemones_necesariosg
team_especies_pokemones_necesarios( team);
connectar_con_broker(); //marcos
enviar_mensaje_a_broker( get, list_get( especies_necesarias, i), socket); //marcos
cerrar_coneccion_broker(); //marcos
//verificar_nuevo_localized
hay_nuevos_mensajes(); //marcos
recibir_mensaje(); //marcos   //aca la idea es que devuelva por parametro el pokemon para despues capturarlo
primer_mensaje( team, especie); //solo me quedo con el primer localized de cada especie ,no puede haber dos en mapa sueltos de cada especie
agregar_nuevos( team);
no_es_primer_mensaje( team, especie); //avisa que ya no es el primer mensaje el proximo que vendra de esa especie,tendra que volverlo a poner en blanco el entrenador al capturar,no se si n se podria hacer directamente contando los que hay sueltos en mapa
descartar_mensaje( mensaje);

*/



#endif /* SRC_PLANIFICADOR_H_ */
