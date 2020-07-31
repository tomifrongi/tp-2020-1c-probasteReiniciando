#include "team.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>





/*-----------------------------------------------------------CARGA E INICIALIZACION DE PROCESOS TEAM ----------------------*/

t_list * team_get_objetivo_global(t_team*team) {
	t_list *objetivo = list_create();
	void foo(t_entrenador*entrenador) {
		list_add_all(objetivo, objetivo_personal(entrenador));
	}
	list_iterate(team->entrenadores, foo);
	return objetivo;
}



/*-----------------------------------------------------------VERIFICACION SI TEAM PUEDE PLANIFICAR ATRAPAR UN POKEMON  ----------------------*/

int team_cantidad_maxima_especie(t_team*team, char*especie) { // team no puede atrapar mas de una especie de los requerido globalmente

	int cantidad = 0;
	void sumar_cantidad_por_entrenador(t_entrenador*entrenador) {
		cantidad += cantidad_pokemones_especie(entrenador->pokemones_buscados, especie);
	}
	list_iterate(team->entrenadores, sumar_cantidad_por_entrenador);

	return cantidad;
}
int team_cantidad_total_especie(t_team*team, char*especie) { //cantidad de especie que ttiene el team globalmente
	int cantidad = 0;
	void sumar_cantidad_por_entrenador(t_entrenador*entrenador) {
		cantidad += cantidad_pokemones_especie(entrenador->pokemones_capturados, especie);
	}
	list_iterate(team->entrenadores, sumar_cantidad_por_entrenador);

	return cantidad;
}
bool team_puede_capturar(t_team*team, t_pokemon*pokemon) {
	return team_cantidad_maxima_especie(team, pokemon->especie) > team_cantidad_total_especie(team, pokemon->especie);
}
/*------------------------------------------------BUSQUEDA DE ENTRENADOR QUE VA A ATRAPAR------------------------------------------------*/

t_list*team_entrenadores_disponibles(t_team*team) {

	return list_filter(team->entrenadores, esta_disponible); // todo el esta_disponible
}
/*
 t_entrenador*team_buscar_entrenador_cercano(t_team*team) {
 list_sorted(team_entrenadores_disponibles(team))
 }*/ //todo
/*------------------------------------------------VERIFICACION DE OBJETIVO CUMPLIDO DE TEAM------------------------------------------------*/

bool team_cumplio_objetivo_global(t_team*team) {

	//return( pokemon_suelto(team->mapa)==false && list_is_empty(team_get_objetivo_global(team))==true);//me pa que no va, si cumplio objetivos estaria en exit
	bool entrenador_finalizado(t_entrenador*entrenador) {
		return entrenador->estado == EXIT;
	}
	return list_all_satisfy(team->entrenadores, entrenador_finalizado);

}
void team_verificar_finalizacion(t_team*team) { //la idea es que se lopee  despues de una cambio de estado a exit de un entrenador
	if (team_cumplio_objetivo_global(team)) {
		printf("finalizo el proceso team"); //mejorar
		//todo log_finalizacion;
	}
}




void ordenar_entrenadores_planificados_por_estimacion(t_team* team){
	bool comparador_estimacion(void* e1,void* e2){
		t_entrenador*  entrenador1 = e1;
		t_entrenador*  entrenador2 = e2;
		return entrenador1->estimado_rafaga_proxima < entrenador2->estimado_rafaga_proxima;
	}
	list_sort(team->entrenadores_planificados,comparador_estimacion);
}







//
void consumir_ciclos_cpu(t_team*team,int cantidad_ciclos){
	team->cantidad_ciclos_cpu_ejecutados+=cantidad_ciclos;
	//sleep ? TODO
}



bool algunos_pueden_atrapar(t_team* team){
	bool alcanzo_capacidad_maxima(void* e){
		t_entrenador* entrenador = e;
		return  list_size(entrenador->pokemones_capturados) < list_size(entrenador->pokemones_buscados);
	}
	return list_any_satisfy(team->entrenadores,alcanzo_capacidad_maxima);
}


t_list* buscar_especie_objetivo_en_mapa(t_team* team){

	char* especie_pokemon;
	t_list* posiciones_pokemon_mapa = NULL;
	int i = 0;
	int size_objetivo_pokemones_restantes = list_size(team->objetivo_pokemones_restantes);

	while(i<size_objetivo_pokemones_restantes){
		especie_pokemon = list_get(team->objetivo_pokemones_restantes,i);
		posiciones_pokemon_mapa = pokemones_misma_especie(team->mapa_pokemones,especie_pokemon); //seria una lista de t_pokemon

		if(!list_is_empty(posiciones_pokemon_mapa))
			return posiciones_pokemon_mapa;
		else
		{
			list_destroy(posiciones_pokemon_mapa);
			posiciones_pokemon_mapa = NULL;
		}
		i++;
	}

	return posiciones_pokemon_mapa;

}








