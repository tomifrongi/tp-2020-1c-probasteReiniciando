
 #include "entrenadores.h"



/*-----------------------------------------------------------INICIALIZACION----------------------*/

t_entrenador *iniciar_entrenador(int id, int posicion_x, int posicion_y, t_list*pokemones_capturados, t_list*pokemones_buscados) {
	t_entrenador*entrenador = malloc(sizeof(t_entrenador)); //cuidado con el malloc el tamaño de entrenador no es fijo
	entrenador->id = id;
	entrenador->posicion_x = posicion_x;
	entrenador->posicion_y = posicion_y;

	entrenador->estado = NEW;
	entrenador->pokemones_capturados = pokemones_capturados;
	entrenador->pokemones_buscados = pokemones_buscados;

	return entrenador;
}

t_list * inicializar_entrenadores(t_team *team) {
	int cantidad = cantidad_entrenadores(team);

	t_list *entrenadores = list_create();
	int posicion_x, posicion_y;
	char**posicion;

	int id;
	for (int i = 0; i < cantidad; i++) {
		id = i;

		posicion = leer_posicion(team, i); // coordenadas posicion, ejemplo  [1,2]
		posicion_x = atoi(posicion[0]);
		posicion_y = atoi(posicion[1]);

		t_entrenador *entrenador = iniciar_entrenador(id, posicion_x, posicion_y, get_pokemones(team, i, 0), get_pokemones(team, i, 1)); //el estado lo hago sin pasar parametro por ahora

		list_add(entrenadores, entrenador);
		printf("se cargo el entrenador: %d \n\n", i);

	}

	return entrenadores;
}

void mostrar_entrenador(t_entrenador*entrenador) { //closure
	printf("id: %d\n", entrenador->id);
	printf("estado: %d\n", entrenador->estado);
	printf("posicion x: %d\n", entrenador->posicion_x);
	printf("posicion y: %d\n", entrenador->posicion_y);
	printf("\npokemones capturados:\n\n");

	mostrar_pokemones(entrenador->pokemones_capturados);
	printf("\npokemones buscados:\n\n");

	mostrar_pokemones(entrenador->pokemones_buscados);

	printf("\n\n--fin entrenador\n\n");
}
void mostrar_entrenadores(t_list*entrenadores) {
	list_iterate(entrenadores, (void*) mostrar_entrenador);

}

/*-----------------------------------------------------------PLANIFICACION----------------------*/

//funciones de estado
void cambiar_estado(t_entrenador* entrenador, t_state estado) {
	entrenador->estado = estado;

}
void entrenador_bloquear(t_entrenador *entrenador) { //para mas claridad
	cambiar_estado(entrenador, BLOCK);
}
bool entrenador_cumplio_objetivos(t_entrenador *entrenador){
	return list_size(entrenador_objetivo_personal)==0;

}
int entrenador_finalizar(t_entrenador *entrenador) { //o si no finalizo,1 si finalizo ok //ver que onda por que esta funcion me hizo agregar el id en struct team y entrenador,si se usa solo aca,buscarle la vuelta por otro lado
	if(entrenador_cumplio_objetivos(entrenador)){
	cambiar_estado(entrenador, EXIT);
	team_verificar_finalizacion(entrenador->team);
	return 1;
	}
	return 0;
}

bool entrenador_finalizo(t_entrenador*entrenador) {
	return entrenador->estado == EXIT;
}
bool esta_disponible(t_entrenador*entrenador) { //y si esta new??
return entrenador->estado==READY ;
}

//funciones de objetivos
t_list * entrenador_objetivo_personal(t_entrenador*entrenador) { //los que le faltan por encontrar a cada entrenador

return  intersect_listas_pokemones(entrenador->pokemones_buscados, entrenador->pokemones_capturados);
}

t_list* entrenador_pokemones_sobrantes(t_entrenador*entrenador) {
t_list*lista = entrenador->pokemones_capturados; //a los capturados les saco los que son el objetivo personal y me da los que no necesito...?mmm
intersect_listas_pokemones(lista, entrenador_objetivo_personal(entrenador));

return lista;
}
int entrenador_cantidad_objetivos(t_entrenador *entrenador) {

return list_size(entrenador->pokemones_buscados);
}
//funciones de captura?
int entrenador_cantidad_capturados(t_entrenador *entrenador) {

return list_size(entrenador->pokemones_capturados);
}
bool entrenador_puede_capturar(t_entrenador*entrenador) {
return cantidad_objetivos(entrenador) > cantidad_capturados(entrenador); //si tenia que capturar 3 no puede mas de 3 por mas que no sean los suyos
}

/*//ver de que resiva el struct intercambio directamente
bool entrenador_intercambio_altera_objetivo(t_entrenador*entrenador, t_entrenador*entrenador2, t_pokemon*pokemon1,t_pokemon*pokemon1) { //los dos que van a intercambiar, y que tipo de pokemon
t_list * objetivo_previo_e1 = objetivo_personal(entrenador);
t_list * objetivo_previo_e2 = objetivo_personal(entrenador2); //todo ver si puedo usar una lista sin create
}*/

