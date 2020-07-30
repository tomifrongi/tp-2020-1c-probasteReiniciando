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
		pthread_join(hilos[i]);

	}
	return &hilos;
}


//__________________________Guion_____________________________________________________________________-



bool verificar_nuevo_localized(t_team*team, t_pokemon*pokemon) { //TODO
		char* especie;
		t_message mensaje;
		if (hay_nuevos_mensajes()) {
			t_pokemon*pokemon = recibir_mensaje(); //aca la idea es que devuelva por parametro el pokemon para despues capturarlo
			if (team_necesita_especie(especie) & primer_mensaje(team, especie)) { //NO ME GUSTA ASI...//solo me quedo con el primer localized de cada especie ,no puede haber dos en mapa sueltos de cada especie
				agregar_nuevos(team); //agrego a la lista de pokemones sueltos los que llegaron //mapeo
				no_es_primer_mensaje(team, especie); //avisa que ya no es el primer mensaje el proximo que vendra de esa especie,tendra que volverlo a poner en blanco el entrenador al capturar,no se si n se podria hacer directamente contando los que hay sueltos en mapa

			} else {
				descartar_mensaje(mensaje);
				return false;
			}
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

//armo los planificadores:
//atrapada:entrenador y pokemon involucrado
int get_indice(t_team*team, hilo_entrenador) {
	int indice = 0;
	if (list_size(team->entrenadores_planificados) == 0) { //si no hay elementos lo pongo primero
		return 0;
	}
	while (indice < list_size(team->entrenadores_planificados)) {
		t_entrenador*hilo_otro_entrenador = list_get(team->entrenadores_planificados, indice);
		if (hilo_entrenador->tiempo_rafaga > hilo_otro_entrenador->tiempo_rafaga) {
			indice++;
		} else {
			return indice;
		}

	}

}



void planificar_entrenador(t_team * team, t_pokemon * pokemon) //TODO ACTUALIZAR
{
	//pasar su hilo a ready
	t_entrenador*entrenador = entrenador_mas_cercano(team->entrenadores, pokemon);
//aca armo el hilo y se los paso a los planificadores//seria otro tad...

	switch (team->planificador){
	case FIFO:{

	}


	case RR:{

		list_add(team->entrenadores_planificados, hilo_entrenador); //se agrega al final
		break;
	}


	case SJF_CD:{
		int indice = get_indice(team, hilo_entrenador);
		if (indice == 0) {
			desalojar_hilo(); //TODO que desaloje y se ejecute el nuevo hilo
		}
		list_add_in_index(team->entrenadores_planificados, indice, hilo_entrenador);

		break;
	}

	case SJF_SD: {//lo agrega ordenado segun su tiempo

		list_add_in_index(team->entrenadores_planificados, get_indice(team, hilo_entrenador), hilo_entrenador);
		break;
	}

	}

}

int tiempo_rafaga(t_entrenador* entrenador,t_pokemon* pokemon) {
	return team->retardo_ciclo_cpu * distancia_entrenador_pokemon(entrenador, pokemon);

}

void planificacion_rr(t_team* team, entrenador_cercano) {
	int quantum = team->quantum;
	list_add(team->entrenadores_planificados, entrenador_cercano);
	int tiempo_total_captura = tiempo_rafaga(entrenador, pokemon); //lo paso a una variable por que sino al moverse el entrenador altera la cuenta
	for (int i = 0; i <= quantum; i++) {
		if (i == quantum) { //agoto tiempo de quuantum,pasa a bloqueado
			i++;
		}
	}
}

planificacion_sjf_cd( team, hilo_entrenador){

}

//PROCESAMIENTO DE LOCALIZED Y APPEARED ----------------------------
// Este link explica como proceso los mensajes appeared y localized
// https://github.com/sisoputnfrba/foro/issues/1749

bool contain_id_get(t_team* team,uint32_t id_correlativo){
	bool mismo_id(void* id_lista){
		uint32_t* id_casteado = id_lista;
		return(id_casteado == id_correlativo);
	}
	void* id_encontrado = list_find(team->idsGet,mismo_id);
	return (id_encontrado != NULL);
}

bool verificar_nuevo_localized(t_team* team, t_pokemon* pokemon,uint32_t id_correlativo){
	if(contain_id_get(team,id_correlativo) && team_necesita_especie(team,pokemon) && contain_especie_recibida(team,pokemon))
		return false;
	else{
		char* especie_nueva_recibida = malloc(strlen(pokemon->especie));
		strcpy(especie_nueva_recibida,pokemon->especie);
		list_add(team->especiesRecibidas,especie_nueva_recibida);
		return true;
	}

}

void procesar_localized(t_team* team){
	while(1){
		sem_wait(team->semaforo_contador_localized);
		localized_pokemon* mensaje = queue_pop(team->cola_localized);

		t_pokemon pokemon_a_capturar;
		pokemon_a_capturar.especie = mensaje->nombrePokemon;
		coordenada* posicion = list_get(mensaje->posiciones,0);
		pokemon_a_capturar.posicion_x = posicion->posicionEjeX;
		pokemon_a_capturar.posicion_x = posicion->posicionEjeY;
		t_pokemon* pokemon_a_capturar_creado = crear_t_pokemon(pokemon_a_capturar);
		t_list* pokemones_localized = list_create();

		void agregar_pokemon_suelto(void* posicion){
			coordenada* posicionCasteada = posicion;
			t_pokemon pokemon_a_capturar;
			pokemon_a_capturar.especie = mensaje->nombrePokemon;
			pokemon_a_capturar.posicion_x = posicionCasteada->posicionEjeX;
			pokemon_a_capturar.posicion_x = posicionCasteada->posicionEjeY;
			t_pokemon* pokemon_a_capturar_creado = crear_t_pokemon(pokemon_a_capturar);
			list_add(pokemones_localized,pokemon_a_capturar_creado);
		}

		if(verificar_nuevo_localized(team,pokemon_a_capturar_creado,mensaje->idCorrelativo)){
			borrar_t_pokemon(pokemon_a_capturar);
			list_iterate(mensaje->posiciones,agregar_pokemon_suelto);

			//TODO busco_cuantos_necesito
			//TODO planifico los mas cercanos a un entrenador
			//TODO el resto los agrego a una lista por si los llego a necesitar
			//planificar_entrenador(team,pokemon_a_capturar_creado);
		}


	}
}

bool contain_especie_recibida(t_team* team,t_pokemon* pokemon){
	bool misma_especie(void* especie_lista){
		char* especie_casteada = especie_lista;
		return(strcmp(especie_casteada,pokemon->especie)== 0);
	}
	void* pokemonEncontrado = list_find(team->especiesRecibidas,misma_especie);
	return (pokemonEncontrado != NULL);
}

bool team_necesita_especie(t_team* team, t_pokemon* pokemon); //TODO verificar si el pokemon esta dentro de los objetivos

bool verificar_nuevo_appeared(t_team* team, t_pokemon* pokemon){
	if(team_necesita_especie(team,pokemon) && contain_especie_recibida(team,pokemon))
		return false;
	else{
		char* especie_nueva_recibida = malloc(strlen(pokemon->especie));
		strcpy(especie_nueva_recibida,pokemon->especie);
		list_add(team->especiesRecibidas,especie_nueva_recibida);
		return true;
	}
}

void procesar_appeared(t_team* team){
	while(1){
		sem_wait(team->semaforo_contador_appeared);
		appeared_pokemon* mensaje = queue_pop(team->cola_appeared);
		t_pokemon pokemon_a_capturar;
		pokemon_a_capturar.especie = mensaje->nombrePokemon;
		pokemon_a_capturar.posicion_x = mensaje->posicionEjeX;
		pokemon_a_capturar.posicion_x = mensaje->posicionEjeY;
		t_pokemon* pokemon_a_capturar_creado = crear_t_pokemon(pokemon_a_capturar);
		if(verificar_nuevo_appeared(team,pokemon_a_capturar_creado))
			planificar_entrenador(team,pokemon_a_capturar_creado);
	}

}


void handler_entrenador(t_entrenador* entrenador){
	while(1){


		sem_wait(entrenador->semaforo);

		ejecutar_ciclo_cpu(entrenador);


		if(calcular_rafagas_necesarias(entrenador) == 0){
			switch(entrenador->tarea->tipo_tarea){
				case ATRAPAR:{
					entrenador->esta_en_entrada_salida = true;
					int id_mensaje = enviar_catch(entrenador->tarea->pokemon);
					int* id_mensaje_creado = malloc(sizeof(int));
					*id_mensaje_creado = id_mensaje;
					list_add(idsCatch,id_mensaje_creado);
					entrenador->id_correlativo_esperado = id_mensaje;
					break;
				}

				case INTERCAMBIO:{
					realizar_intercambio(entrenador,entrenador->tarea->entrenador_intercambio);
					if(entrenador_cumplio_objetivos(entrenador))
						cambiar_estado(entrenador,EXIT);

					else{

					}
					break;
				}
			}
		}
		else{
			sleep(RETARDO_CICLO_CPU);
		}
	}
}

//------------------------------------------------------------------

//RESUMEN SEMAFOROS-------------
/*
pthread_t thread;
pthread_create();
pthread_detach();

sem_t semaforo_contador;
sem_init();
sem_post();
sem_wait();

pthread_mutex_t semaforo_mutex;
pthread_mutex_init();
pthread_mutex_lock();
pthread_mutex_unlock();
*/
//-----------------------------


//retorna si le alcanzo o no las rafagas
bool sem_post_algoritmo(t_entrenador* entrenador,t_list* entrenadores_planificados){

	switch(TEAM->planificador){

	case FIFO:{
		int rafagas_necesarias = calcular_rafagas_necesarias(entrenador);
		int i = 1;
		cambiar_estado(entrenador,EXEC);
		while(i<=rafagas_necesarias){
			sem_post(entrenador->semaforo);
			i++;
		}
		cambiar_estado(entrenador,BLOCK);
		return true;
	}

	case RR:{
		int rafagas_necesarias = calcular_rafagas_necesarias(entrenador);
		if(rafagas_necesarias<=QUANTUM){
			int i = 1;
			cambiar_estado(entrenador,EXEC);
			while(i<=rafagas_necesarias){
				sem_post(entrenador->semaforo);
				i++;
			}
			cambiar_estado(entrenador,BLOCK);
			return true;
		}
		else{
			int i = 1;
			cambiar_estado(entrenador,EXEC);
			while(i<=QUANTUM){
				sem_post(entrenador->semaforo);
				i++;
			}
			cambiar_estado(entrenador,READY);
			return false;
		}
	}

	case SJF_CD:{
		int rafagas_necesarias = calcular_rafagas_necesarias(entrenador);
		int i = 1;
		pthread_mutex_lock(mutex_entrenadores_disponibles);
		int longitud_lista_actual = list_size(entrenadores_planificados);
		pthread_mutex_unlock(mutex_entrenadores_disponibles);
		cambiar_estado(entrenador,EXEC);
		while(i<=rafagas_necesarias){
			sem_post(entrenador->semaforo);

			pthread_mutex_lock(mutex_entrenadores_disponibles);
			int longitud_lista_actualizada = list_size(entrenadores_planificados);
			pthread_mutex_unlock(mutex_entrenadores_disponibles);

			if(longitud_lista_actualizada != longitud_lista_actual){
				actualizar_estimados(entrenador,i,entrenador->estimado_rafaga_proxima);
				cambiar_estado(entrenador,READY);
				return false;
			}
			i++;
		}
		cambiar_estado(entrenador,BLOCK);

		actualizar_estimados(entrenador,i-1,entrenador->estimado_rafaga_proxima);
		return true;

	}

	case SJF_SD:{
		int rafagas_necesarias = calcular_rafagas_necesarias(entrenador);
		int i = 1;
		while(i<=rafagas_necesarias){
			sem_post(entrenador->semaforo);
			i++;
		}
		cambiar_estado(entrenador,BLOCK);
		actualizar_estimados(entrenador,i-1,entrenador->estimado_rafaga_proxima);
		return true;

	}

	default:
		return false;
	}

}

void agregar_entrenador_a_planificar(t_entrenador* entrenador,t_team* team){
	cambiar_estado(entrenador,READY);
	switch(team->planificador){
	case FIFO:{
		pthread_mutex_lock(mutex_entrenadores_disponibles);
		list_add(team->entrenadores_planificados,entrenador);
		pthread_mutex_unlock(mutex_entrenadores_disponibles);
		sem_post(semaforo_entrenadores_disponibles);
		break;
		}

	case RR:{
		pthread_mutex_lock(mutex_entrenadores_disponibles);
		list_add(team->entrenadores_planificados,entrenador);
		pthread_mutex_unlock(mutex_entrenadores_disponibles);
		sem_post(semaforo_entrenadores_disponibles);
		break;
		}

	case SJF_CD:{
		pthread_mutex_lock(mutex_entrenadores_disponibles);
		list_add(team->entrenadores_planificados,entrenador);
		ordenar_entrenadores_planificados_por_estimacion(team);
		pthread_mutex_unlock(mutex_entrenadores_disponibles);
		sem_post(semaforo_entrenadores_disponibles);
		break;
		}

	case SJF_SD:{
		pthread_mutex_lock(mutex_entrenadores_disponibles);
		list_add(team->entrenadores_planificados,entrenador);
		ordenar_entrenadores_planificados_por_estimacion(team);
		pthread_mutex_unlock(mutex_entrenadores_disponibles);
		sem_post(semaforo_entrenadores_disponibles);
		break;
		}

	default:{
		break;
		}

	}
}



void planificacion_general(t_team* team){

	while (!team_cumplio_objetivo_global(team)){

		sem_wait(semaforo_entrenadores_disponibles);

		pthread_mutex_lock(mutex_entrenadores_disponibles);
		t_entrenador* entrenador = list_remove(team->entrenadores_planificados,0);
		pthread_mutex_unlock(mutex_entrenadores_disponibles);

		if(!sem_post_algoritmo(entrenador))
			agregar_entrenador_a_planificar(entrenador,team);





		//SEMAFORO QUE ESPERA A QUE HAYA ENTRENADORES PARA PLANIFICAR EN "team->entrenadores_planificados"


		//wait_sem(&semaforo_entrenadores_disponibles_para_planificar)

		//WAIT MUTEX DE "team->entrenadores_planificados;"
		//EJECUTO LA RAFAGA DE CPU PARA EL ENTRENADOR QUE CORRESPONDA, CUANTO DURA LA RAFAGA DEPENDERA DEL ALGORITMO

			//LOS VOY MOVIENDO A LA POSICION DEL POKEMON QUE QUIEREN ATRAPAR
			//OOO LOS VOY MOVIENDO A LA POSICION DEL ENTRENADOR QUE NECESITA EL INTERCAMBIO

		//SI LLEGO A LA POSICION QUE BUSCA MANDO EL CATCH O HAGO EL INTERCAMBIO DE POKEMON
			//SI EL ENTRENADOR TIENE QUE SEGUIR SIENDO PLANIFICADO, HAGO UN signal_sem(&semaforo_entrenadores_disponibles_para_planificar)
			//ES DECIR, NO SACO AL ENTRENADOR DE LA LISTA: ""team->entrenadores_planificados"


		//SIGNAL MUTEX DE "team->entrenadores_planificados;"
	}
	if(detectarDeadlocks(team))
			resolverDeadlocks();

}


//----------------------------------------------------------GUION/funcion principal:


void planificar_team(t_team*team) {

	//sem_init(&semaforo_contador_localized,0,0);
	//bool suscripcion_ok = suscribirse_a_colas(team);
	t_list* pokemones_por_especie = list_create();

	pthread_t appeared_thread;
	pthread_t localized_thread;
	pthread_t caught_thread;
	pthread_t appeared_gameboy_thread;

	uint32_t a = APPEARED;
	uint32_t l = LOCALIZED;
	uint32_t c = CAUGHT;


	pthread_create(&appeared_thread, NULL, (void*) handler_broker, &a);//TODO agregar semaforo contador (post) a estos 3 hilos
	pthread_detach(appeared_thread);

	pthread_create(&localized_thread, NULL, (void*) handler_broker, &l);
	pthread_detach(localized_thread);

	pthread_create(&caught_thread, NULL, (void*) handler_broker, &c);
	pthread_detach(caught_thread);

	int listener_socket = init_server(PUERTO_TEAM);
	pthread_create(&appeared_gameboy_thread, NULL, (void*) escuchar_mensajes_gameboy, &listener_socket);//TODO ordenar escuchar_mensajes_gameboy
	pthread_detach(appeared_gameboy_thread);

	enviar_gets(pokemones_por_especie);



	pthread_t consumidor_cola_localized;
	pthread_create(&consumidor_cola_localized,NULL,procesar_localized,team);
	pthread_detach(consumidor_cola_localized);


	pthread_t consumidor_cola_appeared;
	pthread_create(&consumidor_cola_appeared,NULL,procesar_appeared,NULL);
	pthread_detach(consumidor_cola_appeared);

	pthread_t consumidor_cola_caught;
	pthread_create(&consumidor_cola_caught,NULL,procesar_caught,NULL);
	pthread_detach(consumidor_cola_caught);

	pthread_t hilo_principal;
	pthread_create(&hilo_principal,NULL,planificacion_general,NULL);
	pthread_detach(hilo_principal);

	crear_hilos_entrenadores();

	//ESTE HILO ESTA CONSTAMENTE EN UN WHILE 1, SI HAY ENTRENADORES PARA PLANIFICAR, LOS AGARRO Y HAGO LO QUE TENGA QUE HACER








//	while (team_cumplio_objetivo_global(team) == false) {
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//			if (verificar_nuevo_localized(team,pokemon_a_capturar)&& team_puede_capturar(team) ) //las repuestas a los get,pueden ser dirigidos a otro team
//			{
//				/*
//				 *
//				 * la idea en este punto es mandarlo a capturar,  osea ponerlo en ready, despues el algoritmo de planificacion dira cuando se ejecuta
//				 */
//				planificar_entrenador(team, pokemon_a_capturar); //aca ya tendria la cola de hilos lista y actualizada para que laburen
//
//			} else { ///mientras no llegan mensajes o se quedo trabajdo el team ,se purgan los deadlocks
//				if (verificar_deadlock(team)) { //si la causa es un deadlock aca se soluciona,si es que no esta llegando mensajes se esperaran
//					team->cantidad_deadlocks_detectados++;
//					salvar_deadlocks(team);
//				}
//				else{//en este punto se supone que no hay pokemones libres en mapa ni deadlock, hay que seguir el curso normal
//					ejecutar_planificador();
//				}
//			}
//		}


	//de aca en adelante se cumplio el objetivo del team...van las funciones de cerrar

}



/* 1. Interconexion con los demas modulos.
 * 2. Activacion de mecanismo de antifallos.
 * 2.1 Si falla activar el mecanismo antifallos.
 * 2.2 Purgar deadlocks internos. // vamo a ver que se puede
 * 3. Al conectar mando los get_pokemon.
 * 3.1 while (el team no haya cumplido el objetivo global){
 * 		4. Escucho los localized_pokemon.
 * 		5. Planifico los entrenadores.
 * 		5.1 Mandar los catch
 * 		5.1.1 Si la subcripcion al broker rompe se considera que se atrapo.
 * 		5.1.2 Si no rompe se queda en espera activa.
 * 		6. Al recibir caught se queda en bloqueado
 * 		6.1 Verificar si el caugth es positivo pasara a EXIT o a BLOCK
 * 		if(detectar_deadlock)
 * 			solucionar_deadlock
 * }
 *
 *  */

