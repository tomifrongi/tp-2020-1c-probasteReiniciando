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
//TODO CORREGIR planificar_entrenador
void planificar_entrenador_aux(t_team * team){


	t_list* posiciones_pokemon_mapa = buscar_especie_objetivo_en_mapa(team);

	while(!list_is_empty(team->entrenadores_desocupados) && !list_is_empty(team->mapa_pokemones) && posiciones_pokemon_mapa != NULL){

		int j = 0;
		t_list* pares_entrenadores_mas_cercanos = list_create();
		int size_posiciones_pokemon_mapa = list_size(posiciones_pokemon_mapa);
		while(j<size_posiciones_pokemon_mapa){
			t_pokemon* pokemon = list_get(posiciones_pokemon_mapa,j);
			t_entrenador* entrenador_mas_cercano = buscar_entrenador_mas_cercano(team->entrenadores_desocupados,pokemon);
			t_distancia_pokemon_entrenador* distancia = malloc(sizeof(t_distancia_pokemon_entrenador));
			distancia->pokemon = pokemon;
			distancia->entrenador = entrenador_mas_cercano;
			distancia->distancia = distancia_entrenador_pokemon(entrenador_mas_cercano,pokemon);
			list_add(pares_entrenadores_mas_cercanos,distancia);
			j++;
		}


		ordenar_t_distancia(pares_entrenadores_mas_cercanos);
		t_distancia_pokemon_entrenador* mejor_distancia = list_remove(pares_entrenadores_mas_cercanos,0);
		asignar_tarea_atrapar(mejor_distancia->entrenador,TEAM,mejor_distancia->pokemon,mutex_entrenadores_ready,semaforo_entrenadores_ready);
		remover_entrenador(team->entrenadores_desocupados,mejor_distancia->entrenador);
		remover_pokemon(team->mapa_pokemones,mejor_distancia->pokemon);
		remover_especie_y_destruir(team->objetivo_pokemones_restantes,mejor_distancia->pokemon->especie);
		borrar_t_distancia_pokemon_entrenador(pares_entrenadores_mas_cercanos);

		list_destroy(posiciones_pokemon_mapa);
		posiciones_pokemon_mapa = buscar_especie_objetivo_en_mapa(team);


	}
}

void planificar_entrenador(t_team * team){


	t_list* posiciones_pokemon_mapa = buscar_especie_objetivo_en_mapa(team);
	while(!list_is_empty(team->entrenadores_desocupados) && !list_is_empty(team->mapa_pokemones) && posiciones_pokemon_mapa != NULL){
		int size = list_size(team->entrenadores_desocupados);
		int i = 0;
		t_list* pares_entrenadores_mas_cercanos = list_create();
		while(i<size){
			t_entrenador* entrenador = list_get(team->entrenadores_desocupados,i);
			t_pokemon* pokemon_mas_cercano = buscar_pokemon_mas_cercano(team->mapa_pokemones,entrenador);
			t_distancia_pokemon_entrenador* distancia = malloc(sizeof(t_distancia_pokemon_entrenador));
			distancia->pokemon = pokemon_mas_cercano;
			distancia->entrenador = entrenador;
			distancia->distancia = distancia_entrenador_pokemon(entrenador,pokemon_mas_cercano);
			list_add(pares_entrenadores_mas_cercanos,distancia);
			i++;
		}
		ordenar_t_distancia(pares_entrenadores_mas_cercanos);
		t_distancia_pokemon_entrenador* mejor_distancia = list_remove(pares_entrenadores_mas_cercanos,0);
		asignar_tarea_atrapar(mejor_distancia->entrenador,TEAM,mejor_distancia->pokemon,mutex_entrenadores_ready,semaforo_entrenadores_ready);
		remover_entrenador(team->entrenadores_desocupados,mejor_distancia->entrenador);
		remover_pokemon(team->mapa_pokemones,mejor_distancia->pokemon);
		remover_especie_y_destruir(team->objetivo_pokemones_restantes,mejor_distancia->pokemon->especie);
		borrar_t_distancia_pokemon_entrenador(pares_entrenadores_mas_cercanos);
		//TODO free(mejor_distancia);?
		list_destroy(posiciones_pokemon_mapa);
		posiciones_pokemon_mapa = buscar_especie_objetivo_en_mapa(team);

	}
}

void borrar_t_distancia_pokemon_entrenador(t_list* distancias){
	void eliminar_int(void* d){
		t_distancia_pokemon_entrenador* t_d = d;
		free(t_d);
	}
	list_destroy_and_destroy_elements(distancias,eliminar_int);
}

void ordenar_t_distancia(t_list* distancias){
	bool menor_t_distancia(void* d1,void*d2){
		t_distancia_pokemon_entrenador* distancia1 = d1;
		t_distancia_pokemon_entrenador* distancia2 = d2;
		return ((distancia1->distancia)< (distancia2->distancia));
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

bool verificar_nuevo_localized(t_team* team, t_pokemon* pokemon,uint32_t id_correlativo){

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
			t_pokemon pokemon_a_capturar_aux;
			pokemon_a_capturar_aux.especie = malloc(mensaje->sizeNombre);
			strcpy(pokemon_a_capturar_aux.especie,mensaje->nombrePokemon);
			pokemon_a_capturar_aux.posicion_x = posicionCasteada->posicionEjeX;
			pokemon_a_capturar_aux.posicion_x = posicionCasteada->posicionEjeY;
			t_pokemon* pokemon_a_capturar_creado_aux = crear_t_pokemon(pokemon_a_capturar_aux);
			list_add(pokemones_localized,pokemon_a_capturar_creado_aux);
			free(pokemon_a_capturar_aux.especie);
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

bool verificar_nuevo_appeared(t_team* team, t_pokemon* pokemon){

	if(!team_puede_capturar(team,pokemon))
		return false;
	else{
		char* especie_nueva_recibida = malloc(strlen(pokemon->especie)+1);
		strcpy(especie_nueva_recibida,pokemon->especie);
		pthread_mutex_lock(mutex_especiesRecibidas);
		list_add(especiesRecibidas,especie_nueva_recibida);
		pthread_mutex_unlock(mutex_especiesRecibidas);
		return true;
	}

}

void* procesar_appeared(void* t){
	t_team* team = t;
	while(1){
		sem_wait(semaforo_contador_appeared);
		pthread_mutex_lock(mutex_cola_appeared);
		appeared_pokemon* mensaje = queue_pop(cola_appeared);
		pthread_mutex_unlock(mutex_cola_appeared);

		t_pokemon pokemon_a_capturar;
		pokemon_a_capturar.especie = malloc(strlen(mensaje->nombrePokemon)+1);
		strcpy(pokemon_a_capturar.especie,mensaje->nombrePokemon);
		pokemon_a_capturar.posicion_x = mensaje->posicionEjeX;
		pokemon_a_capturar.posicion_y = mensaje->posicionEjeY;
		t_pokemon* pokemon_a_capturar_creado = crear_t_pokemon(pokemon_a_capturar);
		free(pokemon_a_capturar.especie);
		borrar_appeared_pokemon(mensaje);
		bool flag = verificar_nuevo_appeared(team,pokemon_a_capturar_creado);
		if(flag){
			pthread_mutex_lock(mutex_planificar_entrenador);
			list_add(team->mapa_pokemones,pokemon_a_capturar_creado);

			pthread_mutex_lock(mutex_cola_appeared);
			bool flag2 = queue_is_empty(cola_appeared);
			pthread_mutex_unlock(mutex_cola_appeared);
			if(flag2)
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
				log_info(log_team_oficial,"EL ENTRENADOR %d ATRAPO AL POKEMON %s EN LA POSICION (%d,%d)",entrenador->id,entrenador->tarea->pokemon->especie,entrenador->posicion_x,entrenador->posicion_y);
				entrenador->esta_en_entrada_salida =false;
				entrenador->id_correlativo_esperado = -1;
				if(entrenador_puede_capturar(entrenador)){
					pthread_mutex_lock(mutex_planificar_entrenador);
					list_add(TEAM->entrenadores_desocupados,entrenador);
					planificar_entrenador(TEAM);
					pthread_mutex_unlock(mutex_planificar_entrenador);
				}
				else
				{
					if(entrenador_cumplio_objetivos(entrenador))
					{
						cambiar_estado(entrenador,EXIT);
						entrenador->esta_en_entrada_salida =false;
					}
					else
					{
						cambiar_estado(entrenador,BLOCK);
						entrenador->esta_en_entrada_salida =false;
					}


					//planificar_entrenador(TEAM);

				}
			}
			else{
				log_info(log_team_oficial,"EL ENTRENADOR %d NO PUDO ATRAPAR AL POKEMON %s EN LA POSICION (%d,%d)",entrenador->id,entrenador->tarea->pokemon->especie,entrenador->posicion_x,entrenador->posicion_y);
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
			free(entrenador->tarea);

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
			sleep(RETARDO_CICLO_CPU);

			//cambiar_estado(entrenador,BLOCK);
			switch(entrenador->tarea->tipo_tarea){
				case ATRAPAR:{
					if(TEAM->conectado_al_broker){

						cambiar_estado(entrenador,BLOCK);
						log_info(log_team_oficial,"SE MOVIO AL ENTRENADOR %d A LA COLA DE LARGO PLAZO Y SE LO BLOQUEO A LA ESPERA DE UN MENSAJE CAUGHT",entrenador->id);
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
						log_info(log_team_oficial,"NO SE PUDO ENVIAR EL MENSAJE CATCH, SE PROCEDE POR DEFAULT");
						list_add(entrenador->pokemones_capturados,entrenador->tarea->pokemon);
						log_info(log_team_oficial,"EL ENTRENADOR %d ATRAPO AL POKEMON %s EN LA POSICION (%d,%d)",entrenador->id,entrenador->tarea->pokemon->especie,entrenador->posicion_x,entrenador->posicion_y);
						entrenador->esta_en_entrada_salida =false;
						free(entrenador->tarea);
						//entrenador->estado
						if(entrenador_puede_capturar(entrenador)){
							pthread_mutex_lock(mutex_planificar_entrenador);
							list_add(TEAM->entrenadores_desocupados,entrenador);
							planificar_entrenador(TEAM);
							pthread_mutex_unlock(mutex_planificar_entrenador);
						}
						else{
							if(entrenador_cumplio_objetivos(entrenador))
								cambiar_estado(entrenador,EXIT);
							else
								cambiar_estado(entrenador,BLOCK);
						}
					}
					break;

				}

				case INTERCAMBIO:{
					realizar_intercambio(entrenador,entrenador->tarea->entrenador_intercambio);
					t_entrenador* entrenador_intercambio = entrenador->tarea->entrenador_intercambio;
					log_info(log_team_oficial,"EL ENTRENADOR %d INTERCAMBIO SU POKEMON %s POR EL POKEMON %s AL ENTRENADOR %d",entrenador->id,entrenador->tarea->pokemon_a_otorgar->especie,entrenador->tarea->pokemon_a_pedir->especie,entrenador->tarea->entrenador_intercambio->id);
					if(entrenador_cumplio_objetivos(entrenador)){
						entrenador->esperando_intercambio = false;
						cambiar_estado(entrenador->tarea->entrenador_intercambio,EXIT);
						free(entrenador->tarea);
					}
					else{
						cambiar_estado(entrenador->tarea->entrenador_intercambio,BLOCK);
						entrenador->esperando_intercambio = false;
						log_info(log_team_oficial,"SE MOVIO AL ENTRENADOR %d A LA COLA DE LARGO PLAZO Y SE LO BLOQUEO A LA ESPERA DE UN INTERCAMBIO",entrenador->id);
						free(entrenador->tarea);
					}

					if(entrenador_cumplio_objetivos(entrenador_intercambio)){
						cambiar_estado(entrenador_intercambio,EXIT);
					}
					else{
						cambiar_estado(entrenador_intercambio,BLOCK);
						log_info(log_team_oficial,"SE MOVIO AL ENTRENADOR %d A LA COLA DE LARGO PLAZO Y SE LO BLOQUEO A LA ESPERA DE UN INTERCAMBIO",entrenador_intercambio->id);
					}

					if(!entrenador_cumplio_objetivos(entrenador) || !entrenador_cumplio_objetivos(entrenador_intercambio) )
						resolver_deadlock(TEAM,mutex_entrenadores_ready,semaforo_entrenadores_ready);


					break;
				}
			}
			sem_post(semaforo_termino_rafaga_cpu);
		}
		else{
			sleep(RETARDO_CICLO_CPU);
			sem_post(semaforo_termino_rafaga_cpu);
		}
	}
	return NULL;

}

//TODO REVISAR PROCESAR_CAUGHT LA PARTE DE INTERCAMBIO
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


void iniciar_estructuras_administrativas(){
	cola_localized = queue_create();
	semaforo_contador_localized = malloc(sizeof(sem_t));
	sem_init(semaforo_contador_localized,0,0);
	mutex_cola_localized = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_cola_localized,NULL);

	cola_appeared = queue_create();
	semaforo_contador_appeared = malloc(sizeof(sem_t));
	sem_init(semaforo_contador_appeared,0,0);
	mutex_cola_appeared = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_cola_appeared,NULL);

	cola_caught = queue_create();
	semaforo_contador_caught = malloc(sizeof(sem_t));
	sem_init(semaforo_contador_caught,0,0);
	mutex_cola_caught = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_cola_caught,NULL);

	mutex_idsGet = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_idsGet,NULL);
	idsGet = list_create();

	mutex_idsCatch = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_idsCatch,NULL);
	idsCatch  = list_create();

	mutex_entrenadores_ready = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_entrenadores_ready,NULL);
	semaforo_entrenadores_ready = malloc(sizeof(sem_t));
	sem_init(semaforo_entrenadores_ready,0,0);

	especiesRecibidas = list_create();
	mutex_especiesRecibidas = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_especiesRecibidas,NULL);

	semaforo_termino_rafaga_cpu = malloc(sizeof(sem_t));
	sem_init(semaforo_termino_rafaga_cpu,0,0);

	mutex_planificar_entrenador = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_planificar_entrenador,NULL);


}

//----------------------------------------------------------GUION/funcion principal:


void planificar_team(t_team*team) {
	TEAM = team;
	iniciar_estructuras_administrativas();

	crear_hilos_entrenadores(TEAM);

	pthread_t* appeared_thread = malloc(sizeof(pthread_t));
	administracion_cola administracion_appeared;
	administracion_appeared.cola_mensajes = cola_appeared;
	administracion_appeared.cola_suscriptor = APPEARED;
	administracion_appeared.mutex_cola = mutex_cola_appeared;
	administracion_appeared.semaforo_contador_cola = semaforo_contador_appeared;
	administracion_appeared.team = TEAM;
	pthread_create(appeared_thread, NULL,handler_broker, (void*) &administracion_appeared);
	pthread_detach(*appeared_thread);


	pthread_t* localized_thread = malloc(sizeof(pthread_t));
	administracion_cola administracion_localized;
	administracion_localized.cola_mensajes = cola_localized;
	administracion_localized.cola_suscriptor = LOCALIZED;
	administracion_localized.mutex_cola = mutex_cola_localized;
	administracion_localized.semaforo_contador_cola = semaforo_contador_localized;
	administracion_localized.team = TEAM;
	pthread_create(localized_thread, NULL, (void*) handler_broker, (void*) &administracion_localized);
	pthread_detach(*localized_thread);


	pthread_t* caught_thread = malloc(sizeof(pthread_t));
	administracion_cola administracion_caught;
	administracion_caught.cola_mensajes = cola_caught;
	administracion_caught.cola_suscriptor = CAUGHT;
	administracion_caught.mutex_cola = mutex_cola_caught;
	administracion_caught.semaforo_contador_cola = semaforo_contador_caught;
	administracion_caught.team = TEAM;
	pthread_create(caught_thread, NULL, (void*) handler_broker, (void*) &administracion_caught);
	pthread_detach(*caught_thread);


	pthread_t* appeared_gameboy_thread = malloc(sizeof(pthread_t));
	administracion_gameboy administracion_cola_gameboy;
	administracion_cola_gameboy.cola_mensajes = cola_appeared;
	administracion_cola_gameboy.listener_socket = init_server(PUERTO_TEAM);
	administracion_cola_gameboy.mutex_cola = mutex_cola_appeared;
	administracion_cola_gameboy.semaforo_contador_cola = semaforo_contador_appeared;
	pthread_create(appeared_gameboy_thread, NULL, (void*) escuchar_mensajes_gameboy, (void*) &administracion_cola_gameboy);
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

	t_entrenador* ultimo_entrenador_en_ejecutar = NULL;
	while (algunos_pueden_atrapar(team))
	{
		sem_wait(semaforo_entrenadores_ready);

		pthread_mutex_lock(mutex_entrenadores_ready);
		t_entrenador* entrenador = list_remove(team->entrenadores_ready,0);
		pthread_mutex_unlock(mutex_entrenadores_ready);

		if(ultimo_entrenador_en_ejecutar != entrenador)
			TEAM->cantidad_cambios_de_contexto+=1;
		ultimo_entrenador_en_ejecutar = entrenador;

		if(!sem_post_algoritmo(entrenador,team->entrenadores_ready))
			agregar_entrenador_a_cola_ready(entrenador,team);
	}

	mostrar_entrenadores(team->entrenadores);

	if(detectar_deadlock(TEAM))
		resolver_deadlock(TEAM,mutex_entrenadores_ready,semaforo_entrenadores_ready);

	while (!team_cumplio_objetivo_global(team))
	{
		sem_wait(semaforo_entrenadores_ready);
		pthread_mutex_lock(mutex_entrenadores_ready);
		t_entrenador* entrenador = list_remove(team->entrenadores_ready,0);
		pthread_mutex_unlock(mutex_entrenadores_ready);

		if(ultimo_entrenador_en_ejecutar != entrenador)
			TEAM->cantidad_cambios_de_contexto+=1;
		ultimo_entrenador_en_ejecutar = entrenador;

		if(!sem_post_algoritmo(entrenador,team->entrenadores_ready))
			agregar_entrenador_a_cola_ready(entrenador,team);
	}

	//finalizar todos los hilos
	log_info(log_team_oficial,"EL PROCESO TEAM VA A FINALIZAR");
	log_info(log_team_oficial,"RESULTADO DEL TEAM");
	int ciclos_totales = 0;
	void sumar_ciclos(void* e){
		t_entrenador* entrenador_ciclo = e;
		ciclos_totales+=entrenador_ciclo->ciclos_cpu_realizados;
	}
	list_iterate(TEAM->entrenadores,sumar_ciclos);
	log_info(log_team_oficial,"Cantidad de ciclos de CPU totales: %d",ciclos_totales);

	log_info(log_team_oficial,"Cantidad de cambios de contexto: %d",TEAM->cantidad_cambios_de_contexto);

	void mostrar_ciclos_por_entrenador(void* e){
			t_entrenador* entrenador_ciclo_individual = e;
			log_info(log_team_oficial,"El entrenador %d consumio %d ciclos",entrenador_ciclo_individual->id,entrenador_ciclo_individual->ciclos_cpu_realizados);
		}
	list_iterate(TEAM->entrenadores,mostrar_ciclos_por_entrenador);

	if(TEAM->hubo_deadlocks)
		log_info(log_team_oficial,"Ocurrieron deadlocks y fueron resueltos");
	else
		log_info(log_team_oficial,"No ocurrieron deadlocks");

	mostrar_entrenadores(team->entrenadores);

}



