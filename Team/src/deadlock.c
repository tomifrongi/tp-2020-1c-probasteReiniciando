

#include "deadlock.h"


/*------------------------------------------------FUNCIONES DE DEADLOCK------------------------------------------------*/
bool detectar_deadlock(t_team* team){
	log_info(log_team_oficial,"INICIO EL ALGORITMO DE DETECCION DE DEADLOCKS");
	bool esta_bloqueado(void* e){
		t_entrenador* entrenador = e;
		return entrenador->estado != EXIT;
	}
	if(list_any_satisfy(team->entrenadores,esta_bloqueado)){
		log_info(log_team_oficial,"SE DETECTARON DEADLOCKS");
		team->hubo_deadlocks = true;
		return true;
	}
	else{
		log_info(log_team_oficial,"NO SE DETECTARON DEADLOCKS");
		return false;
	}

	//return list_any_satisfy(team->entrenadores,esta_bloqueado);
}

void resolver_deadlock(t_team* team,pthread_mutex_t* mutex,sem_t*semaforo_cola_ready){
	bool esta_bloqueado_y_no_esta_esperando_un_intercambio(void* e){
		t_entrenador* entrenador = e;
		return (entrenador->estado == BLOCK) && (!(entrenador->esperando_intercambio));
	}
	t_list* entrenadores_bloqueados = list_filter(team->entrenadores,esta_bloqueado_y_no_esta_esperando_un_intercambio);

	int i = 0;
	while(i<list_size(entrenadores_bloqueados) && list_size(entrenadores_bloqueados)>1){
		t_entrenador* entrenador1 = list_remove(entrenadores_bloqueados,i);
		t_entrenador* entrenador2 = buscar_mejor_entrenador_para_intercambio(entrenadores_bloqueados,entrenador1);
		if(entrenador2 != NULL){
			remover_entrenador(entrenadores_bloqueados,entrenador2);
			asignar_mejor_tarea_intercambio(entrenador1,entrenador2,team,mutex,semaforo_cola_ready);
			i = -1;

		}
		i++;
	}
	list_destroy(entrenadores_bloqueados);

	entrenadores_bloqueados = list_filter(team->entrenadores,esta_bloqueado_y_no_esta_esperando_un_intercambio);
	i = 0;
	while(i<list_size(entrenadores_bloqueados) && list_size(entrenadores_bloqueados)>1){
		t_entrenador* entrenador3 = list_get(entrenadores_bloqueados,i);
		t_entrenador* entrenador4 = buscar_entrenador_para_intercambio(entrenadores_bloqueados,entrenador3);
		if(entrenador4 != NULL){
			remover_entrenador(entrenadores_bloqueados,entrenador3);
			remover_entrenador(entrenadores_bloqueados,entrenador4);
			asignar_tarea_intercambio(entrenador3,entrenador4,team,mutex,semaforo_cola_ready);
			i = -1;

		}
		i++;
	}
	list_destroy(entrenadores_bloqueados);

}

t_entrenador* buscar_mejor_entrenador_para_intercambio(t_list* entrenadores,t_entrenador* entrenador){
	int i = 0;
	int size = list_size(entrenadores);
	while(i<size){
		t_entrenador* entrenador1 = list_get(entrenadores,i);
		if(hay_ciclo(entrenador,entrenador1))
			return entrenador1;

		i++;
	}
	return NULL;
}

bool hay_ciclo(t_entrenador* entrenador1,t_entrenador* entrenador2){
	return quiere_algo_de(entrenador1,entrenador2) && quiere_algo_de(entrenador2,entrenador1);
}

t_list* obtener_pokemones_faltantes(t_entrenador* entrenador){
	t_list* pokemones_faltantes = list_create();
	list_add_all(pokemones_faltantes,entrenador->pokemones_buscados);
	t_list* pokemones_capturados_aux = list_create();
	list_add_all(pokemones_capturados_aux,entrenador->pokemones_capturados);

	pokemones_faltantes= intersect_listas_pokemones(pokemones_faltantes,pokemones_capturados_aux);
	list_destroy(pokemones_capturados_aux);
	return pokemones_faltantes;
}

t_list* obtener_pokemones_sobrantes(t_entrenador* entrenador){
	t_list* pokemones_sobrantes = list_create();
	list_add_all(pokemones_sobrantes,entrenador->pokemones_capturados);
	t_list* pokemones_objetivos_aux = list_create();
	list_add_all(pokemones_objetivos_aux,entrenador->pokemones_buscados);

	pokemones_sobrantes = intersect_listas_pokemones(pokemones_sobrantes,pokemones_objetivos_aux);
	list_destroy(pokemones_objetivos_aux);
	return pokemones_sobrantes;
}

bool quiere_algo_de(t_entrenador* entrenador1,t_entrenador* entrenador2){
	int i,j = 0;
	i = 0;
	t_list* pokemones_faltantes_entrenador1 = obtener_pokemones_faltantes(entrenador1);
	t_list* pokemones_sobrantes_entrenador2 = obtener_pokemones_sobrantes(entrenador2);


	int size_i = list_size(pokemones_faltantes_entrenador1);
	int size_j = list_size(pokemones_sobrantes_entrenador2);

	while(i<size_i){
		t_pokemon* pokemon_i = list_get(pokemones_faltantes_entrenador1,i);
		while(j<size_j){
			t_pokemon* pokemon_j = list_get(pokemones_sobrantes_entrenador2,j);
			if(strcmp(pokemon_i->especie,pokemon_j->especie)==0){
				list_destroy(pokemones_faltantes_entrenador1);
				list_destroy(pokemones_sobrantes_entrenador2);
				return true;
			}
			j++;
		}
		i++;
	}
	list_destroy(pokemones_faltantes_entrenador1);
	list_destroy(pokemones_sobrantes_entrenador2);
	return false;
}

t_entrenador* buscar_entrenador_para_intercambio(t_list* entrenadores,t_entrenador* entrenador){
	int i = 0;
	int size = list_size(entrenadores);
	while(i<size){
		t_entrenador* entrenador1 = list_get(entrenadores,i);
		if(quiere_algo_de(entrenador,entrenador1))
			return entrenador1;

		i++;
	}
	return NULL;
}

void asignar_tarea_intercambio(t_entrenador* entrenador1, t_entrenador* entrenador2,t_team* team,pthread_mutex_t* mutex,sem_t*semaforo_cola_ready){
	int i,j = 0;
	i = 0;
	entrenador2->esperando_intercambio = true;
	struct t_tarea* tarea = malloc(sizeof(struct t_tarea));
	tarea->tipo_tarea = INTERCAMBIO;
	tarea->pokemon = NULL;
	tarea->entrenador_intercambio = entrenador2;
	t_list* pokemones_faltantes_entrenador1 = obtener_pokemones_faltantes(entrenador1);
	t_list* pokemones_sobrantes_entrenador2 = obtener_pokemones_sobrantes(entrenador2);
	t_list* pokemones_sobrantes_entrenador1 = obtener_pokemones_sobrantes(entrenador1);

	int size_i = list_size(pokemones_faltantes_entrenador1);
	int size_j = list_size(pokemones_sobrantes_entrenador2);
	int flag_i = false;
	while(i<size_i){
		t_pokemon* pokemon_i = list_get(pokemones_faltantes_entrenador1,i);
		while(j<size_j){
			t_pokemon* pokemon_j = list_get(pokemones_sobrantes_entrenador2,j);
			if(strcmp(pokemon_i->especie,pokemon_j->especie)==0){
				tarea->pokemon_a_pedir = pokemon_j;
				flag_i = true;
				break;
			}
			j++;
		}
		if(flag_i)
			break;
		i++;
	}

	tarea->pokemon_a_otorgar = list_get(pokemones_sobrantes_entrenador1,0);
	list_destroy(pokemones_sobrantes_entrenador1);
	list_destroy(pokemones_faltantes_entrenador1);
	list_destroy(pokemones_sobrantes_entrenador2);

	entrenador1->tarea = tarea;
	entrenador1->estado = READY;
	log_info(log_team_oficial,"EL ENTRENADOR %d SE MOVIO A LA COLA DE CORTO PLAZO PARA PODER IR A INTERCAMBIAR UN POKEMON CON EL ENTRENADOR %d",entrenador1->id,entrenador2->id);
	agregar_entrenador_a_cola_ready_deadlock_h(entrenador1,team,mutex,semaforo_cola_ready);
}

void asignar_mejor_tarea_intercambio(t_entrenador* entrenador1, t_entrenador* entrenador2,t_team* team,pthread_mutex_t* mutex,sem_t*semaforo_cola_ready){
	int i,j = 0;
	i = 0;
	entrenador2->esperando_intercambio = true;
	struct t_tarea* tarea = malloc(sizeof(struct t_tarea));
	tarea->tipo_tarea = INTERCAMBIO;
	tarea->pokemon = NULL;
	tarea->entrenador_intercambio = entrenador2;
	t_list* pokemones_faltantes_entrenador1 = obtener_pokemones_faltantes(entrenador1);
	t_list* pokemones_sobrantes_entrenador2 = obtener_pokemones_sobrantes(entrenador2);

	t_list* pokemones_sobrantes_entrenador1 = obtener_pokemones_sobrantes(entrenador1);
	t_list* pokemones_faltantes_entrenador2 = obtener_pokemones_faltantes(entrenador2);

	int size_i = list_size(pokemones_faltantes_entrenador1);
	int size_j = list_size(pokemones_sobrantes_entrenador2);
	bool flag_i = false;
	while(i<size_i){
		t_pokemon* pokemon_i = list_get(pokemones_faltantes_entrenador1,i);
		while(j<size_j){
			t_pokemon* pokemon_j = list_get(pokemones_sobrantes_entrenador2,j);
			if(strcmp(pokemon_i->especie,pokemon_j->especie)==0){
				tarea->pokemon_a_pedir = pokemon_j;
				flag_i = true;
				break;
			}
			j++;
		}
		if(flag_i)
			break;
		i++;
	}


	int k = 0;
	int l = 0;

	int size_k = list_size(pokemones_faltantes_entrenador2);
	int size_l = list_size(pokemones_sobrantes_entrenador1);
	bool flag_k = false;
	while(k<size_k){
		t_pokemon* pokemon_k = list_get(pokemones_faltantes_entrenador2,k);
		while(l<size_l){
			t_pokemon* pokemon_l = list_get(pokemones_sobrantes_entrenador1,l);
			if(strcmp(pokemon_k->especie,pokemon_l->especie)==0){
				tarea->pokemon_a_otorgar = pokemon_l;
				flag_k = true;
				break;
			}
			l++;
		}
		if(flag_k)
			break;
		k++;
	}



	list_destroy(pokemones_faltantes_entrenador1);
	list_destroy(pokemones_sobrantes_entrenador2);
	list_destroy(pokemones_faltantes_entrenador2);
	list_destroy(pokemones_sobrantes_entrenador1);

	entrenador1->tarea = tarea;
	entrenador1->estado = READY;
	log_info(log_team_oficial,"EL ENTRENADOR %d SE MOVIO A LA COLA DE CORTO PLAZO PARA PODER IR A INTERCAMBIAR UN POKEMON CON EL ENTRENADOR %d",entrenador1->id,entrenador2->id);
	agregar_entrenador_a_cola_ready_deadlock_h(entrenador1,team,mutex,semaforo_cola_ready);

}

void realizar_intercambio(t_entrenador* entrenador1,t_entrenador* entrenador2){
	list_add(entrenador1->pokemones_capturados,entrenador1->tarea->pokemon_a_pedir);
	list_add(entrenador2->pokemones_capturados,entrenador1->tarea->pokemon_a_otorgar);

	remover_pokemon(entrenador1->pokemones_capturados,entrenador1->tarea->pokemon_a_otorgar);
	remover_pokemon(entrenador2->pokemones_capturados,entrenador1->tarea->pokemon_a_pedir);



}



void agregar_entrenador_a_cola_ready_deadlock_h(t_entrenador* entrenador,t_team* team,pthread_mutex_t* mutex,sem_t* semaforo){
	cambiar_estado(entrenador,READY);
	switch(team->planificador){
	case FIFO:{
		pthread_mutex_lock(mutex);
		list_add(team->entrenadores_ready,entrenador);
		pthread_mutex_unlock(mutex);
		sem_post(semaforo);
		break;
		}

	case RR:{
		pthread_mutex_lock(mutex);
		list_add(team->entrenadores_ready,entrenador);
		pthread_mutex_unlock(mutex);
		sem_post(semaforo);
		break;
		}

	case SJF_CD:{
		pthread_mutex_lock(mutex);
		list_add(team->entrenadores_ready,entrenador);
		ordenar_entrenadores_planificados_por_estimacion(team);
		pthread_mutex_unlock(mutex);
		sem_post(semaforo);
		break;
		}

	case SJF_SD:{
		pthread_mutex_lock(mutex);
		list_add(team->entrenadores_ready,entrenador);
		ordenar_entrenadores_planificados_por_estimacion(team);
		pthread_mutex_unlock(mutex);
		sem_post(semaforo);
		break;
		}

	default:{
		break;
		}

	}
}
























