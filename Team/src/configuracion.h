

#ifndef SRC_CONFIGURACION_H_
#define SRC_CONFIGURACION_H_


#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
//#include "entrenadores.h"
#include "team.h"

//CAMPOS-------------------------

//POSICIONES_ENTRENADORES
//POKEMON_ENTRENADORES
//OBJETIVOS_ENTRENADORES
//Estos 3 de arriba nose como ponerlos
int TIEMPO_RECONEXION;
int RETARDO_CICLO_CPU;
char* ALGORITMO_PLANIFICACION;
int QUANTUM;
int ALPHA;

char* IP_BROKER;
int ESTIMACION_INICIAL;
int PUERTO_BROKER;
char* LOG_FILE;

//Estos 2 campos son adicionales
char* IP_TEAM;
int PUERTO_TEAM;

//-------------------------------

//LOGS Y CONFIG------------------

t_config * config_team;
t_log* log_team_oficial; //Aca van toodos los logs obligatorios
t_log* log_team_auxiliar; //Aca van toodos los logs que necesiten que no sean obligatorios.

//-------------------------------


//FUNCIONES----------------------

void cargar_configuracion();
void inicializar_logger();
t_config * leer_config();
void separa_valores(char*string_valores, t_list * ptr_valores) ;
t_list*array_to_list(char**array);
int largo_array(char**array);

int cantidad_entrenadores(t_team*team);

//-------------------------------

#endif /* SRC_CONFIGURACION_H_ */
