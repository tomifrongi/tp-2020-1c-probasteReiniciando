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

//------------FUNCIONES-------------


void* crear_hilos_entrenadores(team);


bool verificar_nuevo_localized(t_team*,t_pokemon*);

int get_indice(t_team*team, hilo_entrenador);

void planificar_entrenador(t_team * , t_pokemon * );

int tiempo_rafaga(t_entrenador*, t_pokemon*);

bool contain_id_get(t_team* ,uint32_t );

bool verificar_nuevo_localized(t_team* , t_pokemon* ,uint32_t );

void procesar_localized(t_team* );

bool contain_especie_recibida(t_team* ,t_pokemon* );

bool team_necesita_especie(t_team* , t_pokemon*);

bool team_necesita_especie(t_team* , t_pokemon* );

bool verificar_nuevo_appeared(t_team*,t_pokemon*);

void procesar_appeared(t_team*);

void handler_entrenador(t_entrenador*);

bool sem_post_algoritmo(t_entrenador* ,t_list*);

void agregar_entrenador_a_planificar(t_entrenador* ,t_team* );

void planificar_team(t_team*);








/*
// TODO
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
