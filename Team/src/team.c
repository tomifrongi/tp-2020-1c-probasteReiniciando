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

	return list_filter(team->entrenadores, esta_disponible);
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

//
void consumir_ciclos_cpu(t_team*team,int cantidad_ciclos){
	team->cantidad_ciclos_cpu_ejecutados+=cantidad_ciclos;
	//sleep ? TODO
}















