#include "team.h"
#include "planificador.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

/*------------------------------------------------FUNCIONES DE PLANIFICACION------------------------------------------------*/

//planificar un entrenador significa pasarlo a ready
void* crear_hilos_entrenadores( team) { //debe hacerse al inciar el proceso team
	int cant_entrenadores = cantidad_entrenadores(team);
	pthread_t hilos[cant_entrenadores];
	for (int i = 0; i < cant_entrenadores; i++) {

		phtread_create(hilos[i], NULL, planificar, NULL);
		pthread_join(hilo[i]);

	}
	return &hilos;
}

// TODO
//planificar team
activar_proceso_reconexion();//un thread
abrir_socket_gameboy();//marcos
recibir_mensajes_gameboy(socket_gameboy);//marcos
team_inicializar(team);
crear_pokemon();
planificar_entrenador(team,pokemon);
verificar_deadlock(team);
salvar_deadlocks(team);
//suscribirse a colas
suscribirse_apparead();//marcos
suscribirse_caught();//marcos
suscribirse_localized();//marcos
//pedir_pokemones_necesariosg
team_especies_pokemones_necesarios(team);
connectar_con_broker();//marcos
enviar_mensaje_a_broker(get, list_get(especies_necesarias, i), socket);//marcos
cerrar_coneccion_broker();//marcos
//verificar_nuevo_localized
hay_nuevos_mensajes();//marcos
recibir_mensaje();//marcos   //aca la idea es que devuelva por parametro el pokemon para despues capturarlo
team_necesita_especie(esṕecie) ;
primer_mensaje(team,especie);//solo me quedo con el primer localized de cada especie ,no puede haber dos en mapa sueltos de cada especie
agregar_nuevos(team);
no_es_primer_mensaje(team,especie);//avisa que ya no es el primer mensaje el proximo que vendra de esa especie,tendra que volverlo a poner en blanco el entrenador al capturar,no se si n se podria hacer directamente contando los que hay sueltos en mapa
descartar_mensaje(mensaje);
//__________________________Guion_____________________________________________________________________-
void planificar_team(t_team*team) {
	bool suscripcion_ok=suscribirse_a_colas(team);
	if (suscripcion_ok==false){
		activar_proceso_reconexion();//un thread
		int socket_gameboy=abrir_socket_gameboy();//no se si no se hace aun cuando el broker si conecta
		recibir_mensajes_gameboy(socket_gameboy);//otro thread
	}
	team_inicializar(team);//para las cuestiones de objetivo y demas carga inicial
	pedir_pokemones_necesarios(team); //gets
	t_pokemon*a_capturar=crear_pokemon();//crear pokemon en blanco ,mmalloc y demas(faltaria para despues una de liberar)

	while(team_cumplio_objetivo_global(team)==false) {
{
		if(verificar_nuevo_localized(team,pokemon)==true&&team_puede_capturar(team)==true)//las repuestas a los get,pueden ser dirigidos a otro team
		{
			/*
			 * la idea en este punto es mandarlo a capturar,  osea ponerlo en ready, despues el algoritmo de planificacion dira cuando se ejecuta
		*/
			planificar_entrenador(team,pokemon);}
		else{//mientras no llegan mensajes o se quedo trabajdo el team ,se purgan los deadlocks
			if (verificar_deadlock(team)){//si la causa es un deadlock aca se soluciona,si es que no esta llegando mensajes se esperaran
				salvar_deadlocks(team);
			}
			}
		}
}

}
bool suscribirse_a_colas(t_team*team) {
	int socket_apparead = suscribirse_apparead();
	int socket_caught = suscribirse_caught();
	int socket_localized = suscribirse_localized();
return((socket_localized ==0)||(socket_caught==0)||(socket_apparead==0)) ;



pedir_pokemones_necesarios(t_team*team) {
t_list*especies_necesarias = list_create();
especies_necesarias = team_especies_pokemones_necesarios(team); //solo mando los tipos de especie que me interesan,no la cantidad que necesite

for (int i = 0; i < list_size(especies_necesarias); i++) {
	int socket = connectar_con_broker(); //tema sockets
	enviar_mensaje_a_broker(get, list_get(especies_necesarias, i), socket);
	//creo que son las tres cosas que necesita nro de socket,especie
	cerrar_coneccion_broker();
}
}


bool verificar_nuevo_localized(team,pokemon) {//TODO
if (hay_nuevos_mensajes()) {
	t_pokemon*pokemon=recibir_mensaje();//aca la idea es que devuelva por parametro el pokemon para despues capturarlo
	if(team_necesita_especie(esṕecie) & primer_mensaje(team,especie)){//NO ME GUSTA ASI...//solo me quedo con el primer localized de cada especie ,no puede haber dos en mapa sueltos de cada especie
	agregar_nuevos(team);//agrego a la lista de pokemones sueltos los que llegaron //mapeo
	no_es_primer_mensaje(team,especie);//avisa que ya no es el primer mensaje el proximo que vendra de esa especie,tendra que volverlo a poner en blanco el entrenador al capturar,no se si n se podria hacer directamente contando los que hay sueltos en mapa

	}else{descartar_mensaje(mensaje);
	return false;}
}
return true;
}

/* entrenadores en new
 *
 * envio n mensajes get de los pokes buscados
 *
 *
 * llegan n  mensajes localized ,
 *     descarto los que no sirven
 *     me quedo con el primer mensaje del localized de cada esoecie(el segundo lo tengo guardado pro si acaso)
 *     mapeo el primer poke de localized de la especie
 *
 planificar_entrenador:* planificao(paso a ready) al entrenador mas cercano que no este haceiendo nada(que este en new , o block por que no teine nada para hacer)
 * si no hay nadie en exec,paso al entrenador que esta en ready a exec
 * se ejecutan los ciclos de cpu para mover al entrenador en el mapa
 * al llegar al pokemon se realiza un nuevo connect al brocker y se envia el mensaje "catch squitrle 4 4",
 *   tomamos el id que nos deuvelve broker yse cierra conexion ,se cierra socket y se bloquea el entrenador
 *  recibimos el caught afirmando/negando la atrapada con el id correspondiente y si es positivo le sumamos el pokemon al entrenador
 *       (*1)hago verificacion de objetivos: si colmo capacidad maxima de captura :queda bloqueado a la espera de deadlock,si cumplio sus objetivos va a exit
 *        si quedo algun pokemon suelto en mapa por que llegaron mas rapido que lo que los entrenadores capturan,lo pongo en estado ready y lo vuelvo a planificar
 *        si no paso nada de lo anterior (que haya pokemones sueltos o que cumplio sus objetivos) queda bloqueado a la espera de que aparezcan pokemones
 *
 /*
 *otro caso:localized envio 2 pokemones pero el obj global solo necesita 1 ,planifico al mas cercano,pero no borro el otro por si el caught es negativo
 *otro caos: localized envio 2 pokemones  y necesito los 2,planifico a dos pokemones mas cercanos para ir a capturar
 *
 *deteccion de deadlock:
 *   el entrenador debe estar bloqeueado
 *   deben poseer pokemones que  no necesitan
 *   se forma espera circular entre entrenadoreses
 *
 * intercambio:se mueve un entrenador al lugar del otro
 * se realiza el intercambio que consume  5 ciclos
 * se realizan las mismas verificaciones que en el caso de caught (*1)
 */

