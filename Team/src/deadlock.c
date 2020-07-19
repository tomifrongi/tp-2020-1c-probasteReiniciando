

#include <deadlock.h>


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
	int igual_al_pokemon1(t_pokemon*pokemon){
		return pokemones_es_misma_especie(pokemon,intercambio->pokemon1);
	}
	int igual_al_pokemon2(t_pokemon*pokemon){
			return pokemones_es_misma_especie(pokemon,intercambio->pokemon2);
		}
	list_remove_by_condition(intercambio->entrenador1->pokemones_capturados,igual_al_pokemon1);//ver si no conviene usar el and_destroy
	list_remove_by_condition(intercambio->entrenador2->pokemones_capturados,igual_al_pokemon2);
	//substituyo los entrenadores por su copia

	int igual_al_entrenador1(t_entrenador*entrenador){
		return intercambio->entrenador1->id==entrenador->id;
	}
	int igual_al_entrenador2(t_entrenador*entrenador){
		return intercambio->entrenador2->id==entrenador->id;
	}
	list_remove_by_condition(team->entrenadores,igual_al_entrenador1);
	list_remove_by_condition(team->entrenadores,igual_al_entrenador2);
	list_add(team->entrenadores,intercambio->entrenador1);
	list_add(team->entrenadores,intercambio->entrenador2);
}

int quiere_algo_de(t_entrenador* entrenador1,t_entrenador* entrenador2){
	int i,j = 0;
	t_pokemon* pokemon1,pokemon2;
	while(i<list_size(entrenador1->pokemones_buscados)){
		while(j<list_size(entrenador2->pokemones_capturados)){
			pokemon1 = list_get(entrenador1->pokemones_buscados,i);
			pokemon2 = list_get(entrenador1->pokemones_capturados,j);
			if(pokemon1->especie == pokemon2->especie)
				return 1;
			j++;
		}
		j=0;
		i++;
	}
	return 0;
}


void* matriz_adyacencia(t_team* team){
	 int n = list_size(team->entrenadores);
	 int matriz[n][n];
	 for(int i=0;i<n;i++){
		 for(int j=0;j<n;j++){
			 if(i==j)
				 continue;
			 matriz[i][j] = quiere_algo_de(&list_get(team->entrenadores,i),&list_get(team->entrenadores,j));
		 }
	 }
	 return matriz;
}

int detectar_ciclos(t_team* team){
	int matriz[][] = matriz_adyacencia(team);
	for(int i = 0 ; i<list_size(team->entrenadores);i++){
		for(int j = 0 ; j<list_size(team->entrenadores);j++){
			if(i==j)
				continue;
			if(matriz[i][j] == matriz[j][i])
				return 1;
		}
	}
	return 0;
}
























