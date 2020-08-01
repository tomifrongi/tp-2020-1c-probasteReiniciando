#include "team.h"

/*-----------------------------------------------------------CARGA E INICIALIZACION DE PROCESOS TEAM ----------------------*/

//t_list * team_get_objetivo_global(t_team*team) {
//	t_list *objetivo = list_create();
//	void foo(t_entrenador*entrenador) {
//		list_add_all(objetivo, objetivo_personal(entrenador));
//	}
//	list_iterate(team->entrenadores, foo);
//	return objetivo;
//}



/*-----------------------------------------------------------VERIFICACION SI TEAM PUEDE PLANIFICAR ATRAPAR UN POKEMON  ----------------------*/


/*------------------------------------------------BUSQUEDA DE ENTRENADOR QUE VA A ATRAPAR------------------------------------------------*/


/*

/*------------------------------------------------VERIFICACION DE OBJETIVO CUMPLIDO DE TEAM------------------------------------------------*/

bool team_cumplio_objetivo_global(t_team*team) {

	//return( pokemon_suelto(team->mapa)==false && list_is_empty(team_get_objetivo_global(team))==true);//me pa que no va, si cumplio objetivos estaria en exit
	bool entrenador_finalizado(t_entrenador*entrenador) {
		return entrenador->estado == EXIT;
	}
	return list_all_satisfy(team->entrenadores, entrenador_finalizado);

}




void ordenar_entrenadores_planificados_por_estimacion(t_team* team){
	bool comparador_estimacion(void* e1,void* e2){
		t_entrenador*  entrenador1 = e1;
		t_entrenador*  entrenador2 = e2;
		return entrenador1->estimado_rafaga_proxima < entrenador2->estimado_rafaga_proxima;
	}
	list_sort(team->entrenadores_ready,comparador_estimacion);
}







//




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








