
#include "team.h"
#include "planificador.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>


/*------------------------------------------------FUNCIONES DE PLANIFICACION------------------------------------------------*/

//planificar un entrenador significa pasarlo a ready

void* crear_hilos_entrenadores(team){
	int cant_entrenadores=cantidad_entrenadores(team);
	pthread_t hilos[cant_entrenadores];
	for (int i=0;i<cant_entrenadores;i++){
		phtread_create(hilos[i],NULL,planificar,NULL);

	}
return &hilos;
}

/*
 * entrenadores en new
 * envio n mensajes get de los pokes buscados
 * llegan n  mensajes localized ,
 *     descarto los que no sirven
 *     me quedo con el primer mensaje del localized de cada esoecie
 *     mapeo el primer poke de localized de la especie
 * planificao(paso a ready) al entrenador mas cercano que no este haceiendo nada(que este en new , o block por que no teine nada para hacer)
 * si no hay nadie en exec,paso al entrenador que esta en ready a exec
 * se ejecutan los ciclos de cpu para mover al entrenador en el mapa
 * al llegar al pokemon se realiza un nuevo connect al brocker y se envia el mensaje "catch squitrle 4 4",
 *   tomamos el id que nos deuvelve broker yse cierra conexion ,se cierra socket
 *  recibimos el caugh afirmando/negando la atrapada con el id correspondiente y si es positivo le sumamos el pokemon al entrenador
 *   hago verificacion de objetivos: si colmo capacidad maxima de captura :queda bloqueado a la espera de deadlock
 *
 *
 */


