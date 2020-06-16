#include "team.h"

typedef struct {
	t_entrenador*entrenador1;
	t_pokemon*pokemon1; //donante ,pasara a estar en entrenador 2
	t_entrenador*entrenador2;
	t_pokemon*pokemon2;

} t_intercambio;
/*-----------------------------------------------------------CARGA E INICIALIZACION DE PROCESOS TEAM ----------------------*/

t_list * team_get_objetivo_global(t_team*team) {
	t_list *objetivo = list_create();
	void foo(t_entrenador*entrenador) {
		list_add_all(objetivo, objetivo_personal(entrenador));
	}
	list_iterate(team->entrenadores, foo);
	return objetivo;
}

void team_cargar_objetivo_global(t_team*team) {
	team->objetivo_global = team_get_objetivo_global(team);
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
/*------------------------------------------------FUNCIONES DE DEADLOCK------------------------------------------------*/
//si se agranda mucho mas hacer un source aparte
/*
 * HAY deadlock cuando no se haya finalizado todos los objetivos individuales:no haya pokemones sueltos en mapa,
 * algun/os entrenadores tengan pokemones sobrantes y a otros le falten capturar pokemones
 * */

//detectar deadlock:
bool team_tiene_pokemones_sobrantes(t_team*team) {
	return list_size(list_filter(team->entrenadores, entrenador_pokemones_sobrantes));
}
bool estado_deadlock(t_team*team) {	//al.h
	return (team_tiene_pokemones_sobrantes(team) == false && team_cumplio_objetivo_global(team) == false && mapa_hay_pokemon_suelto(team) == false);

}
//salvar deadlock:

t_list*candidatos_intercambio(t_team*team) {
	bool es_candidato_intercambio(t_entrenador*entrenador) {	//si le sobra o le falta
		return !entrenador_cumplio_objetivos(entrenador);
	}
	return list_filter(team->entrenadores, es_candidato_intercambio);
}
int cantidad_intercambios(t_list*l1, t_list*l2) {
	int cant_l1 = list_size(l1);
	int cant_l2 = list_size(l2);
	if (l1 == l2) {
		return l1;
	} else {
		if (l1 < l2) {
			return l1;
		} else {
			return l2;
		}

	}

}
bool get_intercambio(t_entrenador *entrenador1, t_entrenador *entrenador2, t_list*intercambios) {	//devuelve true si es un intercambio posible,y por parametro devuelve el intercambio a realizarse
	bool hay_intercambio = false;
	t_list*intersect1 = intersect_listas_pokemones(entrenador_pokemones_faltantes(entrenador1), entrenador_pokemones_sobrantes(entrenador2));
	t_list*intersect2 = intersect_listas_pokemones(entrenador_pokemones_faltantes(entrenador2), entrenador_pokemones_sobrantes(entrenador1));
	t_intercambio *intercambio = malloc(sizeof(t_intercambio));	//ver que onda esto
	if (list_is_empty(intersect1) == false && list_is_empty(intersect1) == false) {	//va a haber intercambio
		hay_intercambio = true;
		cantidad_intercambios=cantidad_intercambios(intersect1,intersect2);
		for (int i = 0; i < cantidad_intercambios; i++) {
			intercambio->entrenador1 = entrenador1;
			intercambio->entrenador2 = entrenador2;
			intercambio->pokemon1=list_get(intersect2,i);
			intercambio->pokemon2=list_get(intersect1,i);//el que va a donar entrenador 2
			list_add(intercambios, intercambio);
		}

	}
	return hay_intercambio;
}
t_list*buscar_intercambios(t_team*team) {	//lista de intercambios(struct) a realizarse
	t_list*candidatos = candidatos_intercambio(team);
	int cantidad_candidatos = list_size(candidatos);
	t_list*intercambios_a_realizar = list_create();
	for (int i = 0; i < cantidad_candidatos; i++) {	//ver de hacer de una forma mas linda despues
		for (int j = 0; j < cantidad_candidatos; j++) {
			if (i != j && get_intercambio(i, j, &intercambios) == true) {	//lo del i!=j es para que no busque intercambios con el mismo entrenador sin sentido
				list_add(intercambios_a_realizar, intercambios);
			}

		}
	}
	return intercambios_a_realizar;
}




void intercambiar_pokemones(t_team*team,t_intercambio *intercambio ){
	//agrego los pokemones nuevos a la copia de los entrenadores
	list_add(intercambio->entrenador1->pokemones_capturados,intercambio->pokemon2);
	list_add(intercambio->entrenador2->pokemones_capturados,intercambio->pokemon1);
  //retiro los pokemones viejos
	bool igual_al_pokemon1(t_pokemon*pokemon){
		return pokemones_es_misma_especie(pokemon,intercambio->pokemon1);
	}
	bool igual_al_pokemon2(t_pokemon*pokemon){
			return pokemones_es_misma_especie(pokemon,intercambio->pokemon2);
		}
	list_remove_by_condition(intercambio->entrenador1->pokemones_capturados,igual_al_pokemon1);//ver si no conviene usar el and_destroy
	list_remove_by_condition(intercambio->entrenador2->pokemones_capturados,igual_al_pokemon2);
	//substituyo los entrenadores por su copia

	bool igual_al_entrenador1(t_entrenador*entrenador){
		return intercambio->entrenador1->id==entrenador->id;
	}
	bool igual_al_entrenador2(t_entrenador*entrenador){
		return intercambio->entrenador2->id==entrenador->id;
	}
	list_remove_by_condition(team->entrenadores,igual_al_entrenador1);
	list_remove_by_condition(team->entrenadores,igual_al_entrenador1);
	list_add(team->entrenadores,intercambio->entrenador1);
	list_add(team->entrenadores,intercambio->entrenador2);
}
















