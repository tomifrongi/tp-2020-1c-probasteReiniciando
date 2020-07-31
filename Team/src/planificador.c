#include "planificador.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

/*------------------------------------------------FUNCIONES DE PLANIFICACION------------------------------------------------*/


void crear_hilos_entrenadores(t_team* team) {

}


//__________________________Guion_____________________________________________________________________-

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



//Llamar cada vez que se desocupa un entrenador o aparece un pokemon nuevo en el mapa o cuando un entrenador no pudo agarrar el pokemon
// Si el entrenador no pudo agarrar el pokemon hay que agregar devuelta la especie a la lista "team->objetivo_pokemones_restantes"

void planificar_entrenador(t_team * team){

	t_list* posiciones_pokemon_mapa = buscar_especie_objetivo_en_mapa(team);
	while(!list_is_empty(team->entrenadores_desocupados) && !list_is_empty(team->mapa_pokemones) && posiciones_pokemon_mapa != NULL){

		int j = 0;
		t_list* pares_entrenadores_mas_cercanos = list_create();
		int size_posiciones_pokemon_mapa = list_size(posiciones_pokemon_mapa);
		while(j<size_posiciones_pokemon_mapa){
			t_pokemon* pokemon = list_get(posiciones_pokemon_mapa,j);
			t_entrenador* entrenador_mas_cercano = buscar_entrenador_mas_cercano(team->entrenadores_desocupados,pokemon);
			t_distancia_pokemon_entrenador* distancia;
			distancia->pokemon = pokemon;
			distancia->entrenador = entrenador_mas_cercano;
			distancia->distancia = malloc(sizeof(int));
			*(distancia->distancia) = distancia_entrenador_pokemon(entrenador_mas_cercano,pokemon);
			list_add(pares_entrenadores_mas_cercanos,distancia);
			j++;
		}


		ordenar_t_distancia(pares_entrenadores_mas_cercanos);
		t_distancia_pokemon_entrenador* mejor_distancia = list_remove(pares_entrenadores_mas_cercanos,0);
		asignar_tarea_atrapar(mejor_distancia->entrenador,mejor_distancia->pokemon);
		remover_entrenador(team->entrenadores_desocupados,mejor_distancia->entrenador);
		remover_pokemon(team->mapa_pokemones,mejor_distancia->pokemon);
		remover_especie_y_destruir(team->objetivo_pokemones_restantes,mejor_distancia->pokemon->especie);
		borrar_int_t_distancia_pokemon_entrenador(pares_entrenadores_mas_cercanos);

		list_destroy(posiciones_pokemon_mapa);
		t_list* posiciones_pokemon_mapa = buscar_pokemones_objetivo_en_mapa(team);


	}
}

void borrar_int_t_distancia_pokemon_entrenador(t_list* distancias){
	void eliminar_int(void* d){
		t_distancia_pokemon_entrenador* t_d = d;
		free(t_d->distancia);
	}
	list_destroy_and_destroy_elements(distancias,eliminar_int);
}


int tiempo_rafaga(t_entrenador* entrenador,t_pokemon* pokemon) {
	return RETARDO_CICLO_CPU * distancia_entrenador_pokemon(entrenador, pokemon);

}



//PROCESAMIENTO DE LOCALIZED Y APPEARED ----------------------------
// Este link explica como proceso los mensajes appeared y localized
// https://github.com/sisoputnfrba/foro/issues/1749

bool contain_id_get(t_list* ids, int id_correlativo){
	bool mismo_id(void* id_lista){
		int* id_casteado = id_lista;
		return(id_casteado == id_correlativo);
	}
	void* id_encontrado = list_find(ids,mismo_id);
	return (id_encontrado != NULL);
}

bool verificar_nuevo_localized(t_team* team, t_pokemon* pokemon,uint32_t id_correlativo){ //TODO REVER VERIFICAR_LOCALIZED

	pthread_mutex_lock(mutex_especiesRecibidas);
	pthread_mutex_lock(mutex_idsGet);
	bool respuesta_a_mensaje_get_propio = contain_id_get(idsGet,id_correlativo);
	pthread_mutex_unlock(mutex_idsGet);



	if(respuesta_a_mensaje_get_propio && !contain_especie_recibida(especiesRecibidas,pokemon))
		return false;
	else{
		char* especie_nueva_recibida = malloc(strlen(pokemon->especie)+1);
		strcpy(especie_nueva_recibida,pokemon->especie);
		list_add(especiesRecibidas,especie_nueva_recibida);
		return true;
	}


	pthread_mutex_unlock(mutex_especiesRecibidas);
}

void procesar_localized(t_team* team){
	while(1){
		sem_wait(semaforo_contador_localized);

		pthread_mutex_lock(mutex_cola_localized);
		localized_pokemon* mensaje = queue_pop(cola_localized);
		pthread_mutex_unlock(mutex_cola_localized);

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
			strcpy(pokemon_a_capturar.especie,mensaje->nombrePokemon);
			pokemon_a_capturar.posicion_x = posicionCasteada->posicionEjeX;
			pokemon_a_capturar.posicion_x = posicionCasteada->posicionEjeY;
			t_pokemon* pokemon_a_capturar_creado = crear_t_pokemon(pokemon_a_capturar);
			list_add(pokemones_localized,pokemon_a_capturar_creado);
		}
		bool b = verificar_nuevo_localized(team,pokemon_a_capturar_creado,mensaje->idCorrelativo);
		borrar_t_pokemon(pokemon_a_capturar);
		if(b){
			list_iterate(mensaje->posiciones,agregar_pokemon_suelto);
			pthread_mutex_lock(mutex_planificar_entrenador);
			list_add_all(team->mapa_pokemones,pokemones_localized);
			planificar_entrenador(team);
			pthread_mutex_unlock(mutex_planificar_entrenador);
		}
		borrar_localized_pokemon(mensaje);
		list_destroy(pokemones_localized);
	}
}

bool contain_especie_recibida(t_list* especies,t_pokemon* pokemon){
	bool misma_especie(void* especie_lista){
		char* especie_casteada = especie_lista;
		return(strcmp(especie_casteada,pokemon->especie)== 0);
	}
	void* pokemonEncontrado = list_find(especies,misma_especie);
	return (pokemonEncontrado != NULL);
}

bool verificar_nuevo_appeared(t_team* team, t_pokemon* pokemon){ //TODO ESPERAR RESPUESTA NICO
	pthread_mutex_lock(mutex_especiesRecibidas);
	if(contain_especie_recibida(especiesRecibidas,pokemon))
		return false;
	else{
		char* especie_nueva_recibida = malloc(strlen(pokemon->especie)+1);
		strcpy(especie_nueva_recibida,pokemon->especie);
		list_add(especiesRecibidas,especie_nueva_recibida);
		return true;
	}
	pthread_mutex_unlock(mutex_especiesRecibidas);
}

void procesar_appeared(t_team* team){
	while(1){
		sem_wait(semaforo_contador_appeared);
		pthread_mutex_lock(mutex_cola_appeared);
		appeared_pokemon* mensaje = queue_pop(cola_appeared);
		pthread_mutex_unlock(mutex_cola_appeared);
		t_pokemon pokemon_a_capturar;
		strcpy(pokemon_a_capturar.especie,mensaje->nombrePokemon);
		pokemon_a_capturar.posicion_x = mensaje->posicionEjeX;
		pokemon_a_capturar.posicion_x = mensaje->posicionEjeY;
		t_pokemon* pokemon_a_capturar_creado = crear_t_pokemon(pokemon_a_capturar);
		borrar_appeared_pokemon(mensaje);
		if(verificar_nuevo_appeared(team,pokemon_a_capturar_creado)){
		pthread_mutex_lock(mutex_planificar_entrenador);
		list_add(team->mapa_pokemones,pokemon_a_capturar_creado);
		planificar_entrenador(team);
		pthread_mutex_unlock(mutex_planificar_entrenador);
		}
	}

}

int* buscar_id(t_list* ids,int id){
	bool mismo_id(void* id_lista){
		uint32_t* id_casteado = id_lista;
		return(id_casteado == id);
	}
	return list_find(ids,mismo_id);
}

void procesar_caught(t_team* team){
	sem_wait(semaforo_contador_caught);
	pthread_mutex_lock(mutex_cola_caught);
	caught_pokemon* mensaje = queue_pop(cola_caught);
	pthread_mutex_unlock(mutex_cola_caught);

	pthread_mutex_lock(mutex_idsCatch);
	int* id = buscar_id(idsCatch,mensaje->idCorrelativo);
	pthread_mutex_unlock(mutex_idsCatch);
	if(id != NULL){
		t_entrenador* entrenador = buscar_entrenador_por_id_correlativo(team->entrenadores,*id);
		if(mensaje->pokemonAtrapado == 1){
			list_add(entrenador->pokemones_capturados,entrenador->tarea->pokemon);
			entrenador->esta_en_entrada_salida =false;
			entrenador->id_correlativo_esperado = -1;
			if(entrenador_puede_capturar(entrenador)){
				pthread_mutex_lock(mutex_planificar_entrenador);
				list_add(entrenador->team->entrenadores_desocupados,entrenador);
				planificar_entrenador(entrenador->team);
				pthread_mutex_unlock(mutex_planificar_entrenador);
			}
			else if(entrenador_cumplio_objetivos(entrenador))
				cambiar_estado(entrenador,EXIT);
			entrenador->esta_en_entrada_salida =false;
		}
		else{
			entrenador->esta_en_entrada_salida =false;
			entrenador->id_correlativo_esperado = -1;
			int longitud_especie = strlen(entrenador->tarea->pokemon->especie);
			char* especie = malloc(longitud_especie+1);
			strcpy(especie,entrenador->tarea->pokemon->especie);
			borrar_t_pokemon(entrenador->tarea->pokemon);
			pthread_mutex_lock(mutex_planificar_entrenador);
			list_add(entrenador->team->entrenadores_desocupados,entrenador);
			list_add(entrenador->team->objetivo_pokemones_restantes,especie);
			planificar_entrenador(entrenador->team);
			pthread_mutex_unlock(mutex_planificar_entrenador);
		}
		free(id);
	}
	borrar_caught_pokemon(mensaje);
}


void handler_entrenador(t_entrenador* entrenador){

	while(entrenador->estado != EXIT){
		sem_wait(entrenador->semaforo);
		ejecutar_ciclo_cpu(entrenador);


		if(calcular_rafagas_necesarias(entrenador) == 0){
			sleep(RETARDO_CICLO_CPU); //TODO REVER
			sem_post(semaforo_termino_rafaga_cpu);
			switch(entrenador->tarea->tipo_tarea){
				case ATRAPAR:{

					if(entrenador->team->conectado_al_broker){

						cambiar_estado(entrenador,BLOCK);
						entrenador->esta_en_entrada_salida = true;
						pthread_mutex_lock(mutex_idsCatch);
						int id_mensaje = enviar_catch(entrenador->tarea->pokemon);
						int* id_mensaje_creado = malloc(sizeof(int));
						*id_mensaje_creado = id_mensaje;
						list_add(idsCatch,id_mensaje_creado);
						pthread_mutex_unlock(mutex_idsCatch);
						entrenador->id_correlativo_esperado = id_mensaje;

					}
					else{
						list_add(entrenador->pokemones_capturados,entrenador->tarea->pokemon);
						entrenador->esta_en_entrada_salida =false;
						//entrenador->estado
						if(entrenador_puede_capturar(entrenador)){
							pthread_mutex_lock(mutex_planificar_entrenador);
							list_add(entrenador->team->entrenadores_desocupados,entrenador);
							planificar_entrenador(entrenador->team);
							pthread_mutex_unlock(mutex_planificar_entrenador);
						}
						else if(entrenador_cumplio_objetivos(entrenador))
							cambiar_estado(entrenador,EXIT);
					}
					break;

				}

				case INTERCAMBIO:{ //TODO REVISAR CASE INTERCAMBIO HANDLER_ENTRENADOR
					realizar_intercambio(entrenador,entrenador->tarea->entrenador_intercambio);
					if(entrenador_cumplio_objetivos(entrenador))
						cambiar_estado(entrenador,EXIT);
					else{
						if(detectar_deadlocks())
							resolver_deadlocs();
					}
					break;
				}
			}
		}
		else{
			sleep(RETARDO_CICLO_CPU); //TODO REVER
			sem_post(semaforo_termino_rafaga_cpu);
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
			sem_wait(semaforo_termino_rafaga_cpu);
			i++;
		}
		//cambiar_estado(entrenador,BLOCK);
		return true;
	}

	case RR:{
		int rafagas_necesarias = calcular_rafagas_necesarias(entrenador);
		if(rafagas_necesarias<=QUANTUM){
			int i = 1;
			cambiar_estado(entrenador,EXEC);
			while(i<=rafagas_necesarias){
				sem_post(entrenador->semaforo);
				sem_wait(semaforo_termino_rafaga_cpu);
				i++;
			}
			//cambiar_estado(entrenador,BLOCK);
			return true;
		}
		else{
			int i = 1;
			cambiar_estado(entrenador,EXEC);
			while(i<=QUANTUM){
				sem_post(entrenador->semaforo);
				sem_wait(semaforo_termino_rafaga_cpu);
				i++;
			}
			cambiar_estado(entrenador,READY);
			return false;
		}
	}

	case SJF_CD:{
		int rafagas_necesarias = calcular_rafagas_necesarias(entrenador);
		int i = 1;
		pthread_mutex_lock(mutex_entrenadores_ready);
		int longitud_lista_actual = list_size(entrenadores_planificados);
		pthread_mutex_unlock(mutex_entrenadores_ready);
		cambiar_estado(entrenador,EXEC);
		while(i<=rafagas_necesarias){
			sem_post(entrenador->semaforo);
			sem_wait(semaforo_termino_rafaga_cpu);
			pthread_mutex_lock(mutex_entrenadores_ready);
			int longitud_lista_actualizada = list_size(entrenadores_planificados);
			pthread_mutex_unlock(mutex_entrenadores_ready);
			int rafagas_necesarias_restantes = calcular_rafagas_necesarias(entrenador);
			if((longitud_lista_actualizada != longitud_lista_actual) && rafagas_necesarias_restantes > 0){
				actualizar_estimados(entrenador,i,entrenador->estimado_rafaga_proxima);
				cambiar_estado(entrenador,READY);
				return false;
			}
			i++;
		}
		//cambiar_estado(entrenador,BLOCK);

		actualizar_estimados(entrenador,i-1,entrenador->estimado_rafaga_proxima);
		return true;

	}

	case SJF_SD:{
		int rafagas_necesarias = calcular_rafagas_necesarias(entrenador);
		int i = 1;
		while(i<=rafagas_necesarias){
			sem_post(entrenador->semaforo);
			sem_wait(semaforo_termino_rafaga_cpu);
			i++;
		}
		//cambiar_estado(entrenador,BLOCK);
		actualizar_estimados(entrenador,i-1,entrenador->estimado_rafaga_proxima);
		return true;

	}

	default:
		return false;
	}

}

void agregar_entrenador_a_cola_ready(t_entrenador* entrenador,t_team* team){
	cambiar_estado(entrenador,READY);
	switch(team->planificador){
	case FIFO:{
		pthread_mutex_lock(mutex_entrenadores_ready);
		list_add(team->entrenadores_planificados,entrenador);
		pthread_mutex_unlock(mutex_entrenadores_ready);
		sem_post(semaforo_entrenadores_ready);
		break;
		}

	case RR:{
		pthread_mutex_lock(mutex_entrenadores_ready);
		list_add(team->entrenadores_planificados,entrenador);
		pthread_mutex_unlock(mutex_entrenadores_ready);
		sem_post(semaforo_entrenadores_ready);
		break;
		}

	case SJF_CD:{
		pthread_mutex_lock(mutex_entrenadores_ready);
		list_add(team->entrenadores_planificados,entrenador);
		ordenar_entrenadores_planificados_por_estimacion(team);
		pthread_mutex_unlock(mutex_entrenadores_ready);
		sem_post(semaforo_entrenadores_ready);
		break;
		}

	case SJF_SD:{
		pthread_mutex_lock(mutex_entrenadores_ready);
		list_add(team->entrenadores_planificados,entrenador);
		ordenar_entrenadores_planificados_por_estimacion(team);
		pthread_mutex_unlock(mutex_entrenadores_ready);
		sem_post(semaforo_entrenadores_ready);
		break;
		}

	default:{
		break;
		}

	}
}


void planificacion_general(t_team* team){

	while (algunos_pueden_atrapar(team)){

		sem_wait(semaforo_entrenadores_ready);

		pthread_mutex_lock(mutex_entrenadores_ready);
		t_entrenador* entrenador = list_remove(team->entrenadores_planificados,0);
		pthread_mutex_unlock(mutex_entrenadores_ready);

		if(!sem_post_algoritmo(entrenador))
			agregar_entrenador_a_cola_ready(entrenador,team);

	}

	if(detectar_deadlocks()) //TODO DEADLOCK
		resolverDeadlocks();

	while (!team_cumplio_objetivo_global(team)) //TODO REVISAR
	{
		sem_wait(semaforo_entrenadores_ready);
		pthread_mutex_lock(mutex_entrenadores_ready);
		t_entrenador* entrenador = list_remove(team->entrenadores_planificados,0);
		pthread_mutex_unlock(mutex_entrenadores_ready);

		if(!sem_post_algoritmo(entrenador))
			agregar_entrenador_a_cola_ready(entrenador,team);

	}


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


	pthread_create(&appeared_thread, NULL, (void*) handler_broker, &a);//TODO PONER BIEN LO QUE RECIBEN ESTOS 4 HILOS
	pthread_detach(appeared_thread);

	pthread_create(&localized_thread, NULL, (void*) handler_broker, &l);
	pthread_detach(localized_thread);

	pthread_create(&caught_thread, NULL, (void*) handler_broker, &c);
	pthread_detach(caught_thread);

	int listener_socket = init_server(PUERTO_TEAM);
	pthread_create(&appeared_gameboy_thread, NULL, (void*) escuchar_mensajes_gameboy, &listener_socket);
	pthread_detach(appeared_gameboy_thread);

	enviar_gets(team->objetivo_pokemones_restantes,idsGet,mutex_idsGet); //TODO ENVIAR GETS
	crear_hilos_entrenadores(); //TODO CREAR HILOS ENTRENADORES


	pthread_t consumidor_cola_localized;
	pthread_create(&consumidor_cola_localized,NULL,procesar_localized,team);
	pthread_detach(consumidor_cola_localized);


	pthread_t consumidor_cola_appeared;
	pthread_create(&consumidor_cola_appeared,NULL,procesar_appeared,NULL);
	pthread_detach(consumidor_cola_appeared);

	pthread_t consumidor_cola_caught;
	pthread_create(&consumidor_cola_caught,NULL,procesar_caught,NULL); //TODO RECIBIR CAUGHT
	pthread_detach(consumidor_cola_caught);


	pthread_t hilo_principal;
	pthread_create(&hilo_principal,NULL,planificacion_general,NULL);
	pthread_detach(hilo_principal);


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

