#include "planificador.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

/*------------------------------------------------FUNCIONES DE PLANIFICACION------------------------------------------------*/


void crear_hilos_entrenadores(t_team* team) {
	int i = 0;
	int size = list_size(team->entrenadores);
	while(i<size){
		void* entrenador = list_get(team->entrenadores,i);
		pthread_t* hilo_entrenador = malloc(sizeof(hilo_entrenador));
		pthread_create(hilo_entrenador,NULL,handler_entrenador,entrenador);
		pthread_detach(*hilo_entrenador);
		list_add(team->hilos_entrenadores,hilo_entrenador);
		i++;
	}
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
		asignar_tarea_atrapar(mejor_distancia->entrenador,TEAM,mejor_distancia->pokemon,semaforo_entrenadores_ready);
		remover_entrenador(team->entrenadores_desocupados,mejor_distancia->entrenador);
		remover_pokemon(team->mapa_pokemones,mejor_distancia->pokemon);
		remover_especie_y_destruir(team->objetivo_pokemones_restantes,mejor_distancia->pokemon->especie);
		borrar_int_t_distancia_pokemon_entrenador(pares_entrenadores_mas_cercanos);

		list_destroy(posiciones_pokemon_mapa);
		posiciones_pokemon_mapa = buscar_especie_objetivo_en_mapa(team);


	}
}

void borrar_int_t_distancia_pokemon_entrenador(t_list* distancias){
	void eliminar_int(void* d){
		t_distancia_pokemon_entrenador* t_d = d;
		free(t_d->distancia);
	}
	list_destroy_and_destroy_elements(distancias,eliminar_int);
}

void ordenar_t_distancia(t_list* distancias){
	bool menor_t_distancia(void* d1,void*d2){
		t_distancia_pokemon_entrenador* distancia1 = d1;
		t_distancia_pokemon_entrenador* distancia2 = d2;
		return (*(distancia1->distancia)< *(distancia2->distancia));
	}
	list_sort(distancias,menor_t_distancia);
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
		return(*id_casteado == id_correlativo);
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

void* procesar_localized(void* t){
	t_team* team = t;
	while(1){
		sem_wait(semaforo_contador_localized);

		pthread_mutex_lock(mutex_cola_localized);
		localized_pokemon* mensaje = queue_pop(cola_localized);
		pthread_mutex_unlock(mutex_cola_localized);

		t_pokemon pokemon_a_capturar;
		pokemon_a_capturar.especie = malloc(mensaje->sizeNombre);
		strcpy(pokemon_a_capturar.especie,mensaje->nombrePokemon);
		coordenada* posicion = list_get(mensaje->posiciones,0);
		pokemon_a_capturar.posicion_x = posicion->posicionEjeX;
		pokemon_a_capturar.posicion_x = posicion->posicionEjeY;
		t_pokemon* pokemon_a_capturar_creado = crear_t_pokemon(pokemon_a_capturar);
		free(pokemon_a_capturar.especie);
		t_list* pokemones_localized = list_create();
		void agregar_pokemon_suelto(void* posicion){
			coordenada* posicionCasteada = posicion;
			t_pokemon pokemon_a_capturar;
			strcpy(pokemon_a_capturar.especie,mensaje->nombrePokemon);
			pokemon_a_capturar.posicion_x = posicionCasteada->posicionEjeX;
			pokemon_a_capturar.posicion_x = posicionCasteada->posicionEjeY;
			t_pokemon* pokemon_a_capturar_creado_aux = crear_t_pokemon(pokemon_a_capturar);
			list_add(pokemones_localized,pokemon_a_capturar_creado_aux);
		}
		bool b = verificar_nuevo_localized(team,pokemon_a_capturar_creado,mensaje->idCorrelativo);
		borrar_t_pokemon(pokemon_a_capturar_creado);
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
	return NULL;
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

void* procesar_appeared(void* t){
	t_team* team = t;
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
	return NULL;

}

int* buscar_id(t_list* ids,int id){
	bool mismo_id(void* id_lista){
		uint32_t* id_casteado = id_lista;
		return(*id_casteado == id);
	}
	return list_find(ids,mismo_id);
}

void* procesar_caught(void* t){
	t_team* team = t;
	while(1){
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
					list_add(TEAM->entrenadores_desocupados,entrenador);
					planificar_entrenador(TEAM);
					pthread_mutex_unlock(mutex_planificar_entrenador);
				}
				else if(entrenador_cumplio_objetivos(entrenador)){
					cambiar_estado(entrenador,EXIT);
					entrenador->esta_en_entrada_salida =false;
				}
			}
			else{
				entrenador->esta_en_entrada_salida =false;
				entrenador->id_correlativo_esperado = -1;
				int longitud_especie = strlen(entrenador->tarea->pokemon->especie);
				char* especie = malloc(longitud_especie+1);
				strcpy(especie,entrenador->tarea->pokemon->especie);
				borrar_t_pokemon(entrenador->tarea->pokemon);
				pthread_mutex_lock(mutex_planificar_entrenador);
				list_add(TEAM->entrenadores_desocupados,entrenador);
				list_add(TEAM->objetivo_pokemones_restantes,especie);
				planificar_entrenador(TEAM);
				pthread_mutex_unlock(mutex_planificar_entrenador);
			}
			free(id);
		}
		borrar_caught_pokemon(mensaje);
	}
	return NULL;
}


void* handler_entrenador(void* e){
	t_entrenador* entrenador = e;
	while(entrenador->estado != EXIT){
		sem_wait(entrenador->semaforo);
		ejecutar_ciclo_cpu(entrenador);


		if(calcular_rafagas_necesarias(entrenador) == 0){
			sleep(RETARDO_CICLO_CPU); //TODO REVER
			sem_post(semaforo_termino_rafaga_cpu);
			switch(entrenador->tarea->tipo_tarea){
				case ATRAPAR:{

					if(TEAM->conectado_al_broker){

						cambiar_estado(entrenador,BLOCK);
						entrenador->esta_en_entrada_salida = true;
						pthread_mutex_lock(mutex_idsCatch);
						int id_mensaje = enviar_catch(entrenador->tarea->pokemon->especie,entrenador->tarea->pokemon->posicion_x,entrenador->tarea->pokemon->posicion_y,idsCatch,mutex_idsCatch);
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
							list_add(TEAM->entrenadores_desocupados,entrenador);
							planificar_entrenador(TEAM);
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
	return NULL;

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
		list_add(team->entrenadores_ready,entrenador);
		pthread_mutex_unlock(mutex_entrenadores_ready);
		sem_post(semaforo_entrenadores_ready);
		break;
		}

	case RR:{
		pthread_mutex_lock(mutex_entrenadores_ready);
		list_add(team->entrenadores_ready,entrenador);
		pthread_mutex_unlock(mutex_entrenadores_ready);
		sem_post(semaforo_entrenadores_ready);
		break;
		}

	case SJF_CD:{
		pthread_mutex_lock(mutex_entrenadores_ready);
		list_add(team->entrenadores_ready,entrenador);
		ordenar_entrenadores_planificados_por_estimacion(team);
		pthread_mutex_unlock(mutex_entrenadores_ready);
		sem_post(semaforo_entrenadores_ready);
		break;
		}

	case SJF_SD:{
		pthread_mutex_lock(mutex_entrenadores_ready);
		list_add(team->entrenadores_ready,entrenador);
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
		t_entrenador* entrenador = list_remove(team->entrenadores_ready,0);
		pthread_mutex_unlock(mutex_entrenadores_ready);

		if(!sem_post_algoritmo(entrenador,team->entrenadores_ready))
			agregar_entrenador_a_cola_ready(entrenador,team);

	}

	if(detectar_deadlocks()) //TODO DEADLOCK
		resolverDeadlocks();

	while (!team_cumplio_objetivo_global(team)) //TODO REVISAR
	{
		sem_wait(semaforo_entrenadores_ready);
		pthread_mutex_lock(mutex_entrenadores_ready);
		t_entrenador* entrenador = list_remove(team->entrenadores_ready,0);
		pthread_mutex_unlock(mutex_entrenadores_ready);

		if(!sem_post_algoritmo(entrenador,team->entrenadores_ready))
			agregar_entrenador_a_cola_ready(entrenador,team);

	}


}

void iniciar_estructuras_administrativas(){
	cola_localized = queue_create();
	sem_init(semaforo_contador_localized,0,0);
	pthread_mutex_init(mutex_cola_localized,NULL);

	cola_appeared = queue_create();
	sem_init(semaforo_contador_appeared,0,0);
	pthread_mutex_init(mutex_cola_appeared,NULL);

	cola_caught = queue_create();
	sem_init(semaforo_contador_caught,0,0);
	pthread_mutex_init(mutex_cola_caught,NULL);

	pthread_mutex_init(mutex_idsGet,NULL);
	idsGet = list_create();

	pthread_mutex_init(mutex_idsCatch,NULL);
	idsCatch  = list_create();

	pthread_mutex_init(mutex_entrenadores_ready,NULL);
	sem_init(semaforo_entrenadores_ready,0,0);

	especiesRecibidas = list_create();
	pthread_mutex_init(mutex_especiesRecibidas,NULL);

	sem_init(semaforo_termino_rafaga_cpu,0,0);
}

//----------------------------------------------------------GUION/funcion principal:


void planificar_team(t_team*team) {
	TEAM = team;
	iniciar_estructuras_administrativas();

	crear_hilos_entrenadores(TEAM);

	pthread_t* appeared_thread = malloc(sizeof(pthread_t));
	administracion_cola* administracion_appeared;
	administracion_appeared->cola_mensajes = cola_appeared;
	administracion_appeared->cola_suscriptor = APPEARED;
	administracion_appeared->mutex_cola = mutex_cola_appeared;
	administracion_appeared->semaforo_contador_cola = semaforo_contador_appeared;
	administracion_appeared->team = TEAM;
	pthread_create(appeared_thread, NULL,handler_broker, (void*) administracion_appeared);
	pthread_detach(*appeared_thread);


	pthread_t* localized_thread = malloc(sizeof(pthread_t));
	administracion_cola* administracion_localized;
	administracion_localized->cola_mensajes = cola_localized;
	administracion_localized->cola_suscriptor = LOCALIZED;
	administracion_localized->mutex_cola = mutex_cola_localized;
	administracion_localized->semaforo_contador_cola = semaforo_contador_localized;
	administracion_localized->team = TEAM;
	pthread_create(localized_thread, NULL, (void*) handler_broker, (void*) administracion_localized);
	pthread_detach(*localized_thread);


	pthread_t* caught_thread = malloc(sizeof(pthread_t));
	administracion_cola* administracion_caught;
	administracion_caught->cola_mensajes = cola_caught;
	administracion_caught->cola_suscriptor = CAUGHT;
	administracion_caught->mutex_cola = mutex_cola_caught;
	administracion_caught->semaforo_contador_cola = semaforo_contador_caught;
	administracion_caught->team = TEAM;
	pthread_create(caught_thread, NULL, (void*) handler_broker, (void*) administracion_caught);
	pthread_detach(*caught_thread);


	pthread_t* appeared_gameboy_thread = malloc(sizeof(pthread_t));
	administracion_gameboy* administracion_gameboy;
	administracion_gameboy->cola_mensajes = cola_appeared;
	administracion_gameboy->listener_socket = init_server(PUERTO_TEAM);
	administracion_gameboy->mutex_cola = mutex_cola_appeared;
	administracion_gameboy->semaforo_contador_cola = semaforo_contador_appeared;
	pthread_create(appeared_gameboy_thread, NULL, (void*) escuchar_mensajes_gameboy, (void*) administracion_gameboy);
	pthread_detach(*appeared_gameboy_thread);


	pthread_t* consumidor_cola_localized = malloc(sizeof(pthread_t));
	pthread_create(consumidor_cola_localized,NULL,procesar_localized,(void*) TEAM);
	pthread_detach(*consumidor_cola_localized);

	pthread_t* consumidor_cola_appeared = malloc(sizeof(pthread_t));
	pthread_create(consumidor_cola_appeared,NULL,procesar_appeared,(void*) TEAM);
	pthread_detach(*consumidor_cola_appeared);

	pthread_t* consumidor_cola_caught = malloc(sizeof(pthread_t));
	pthread_create(consumidor_cola_caught,NULL,procesar_caught,(void*) TEAM);
	pthread_detach(*consumidor_cola_caught);

	enviar_gets(team->objetivo_pokemones_restantes,idsGet,mutex_idsGet);

	while (algunos_pueden_atrapar(team)){

			sem_wait(semaforo_entrenadores_ready);

			pthread_mutex_lock(mutex_entrenadores_ready);
			t_entrenador* entrenador = list_remove(team->entrenadores_ready,0);
			pthread_mutex_unlock(mutex_entrenadores_ready);

			if(!sem_post_algoritmo(entrenador,team->entrenadores_ready))
				agregar_entrenador_a_cola_ready(entrenador,team);

		}

		if(detectar_deadlocks()) //TODO DEADLOCK
			resolverDeadlocks();

		while (!team_cumplio_objetivo_global(team)) //TODO REVISAR
		{
			sem_wait(semaforo_entrenadores_ready);
			pthread_mutex_lock(mutex_entrenadores_ready);
			t_entrenador* entrenador = list_remove(team->entrenadores_ready,0);
			pthread_mutex_unlock(mutex_entrenadores_ready);

			if(!sem_post_algoritmo(entrenador,team->entrenadores_ready))
				agregar_entrenador_a_cola_ready(entrenador,team);

		}


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

