#include "entrenadores.h"

/*-----------------------------------------------------------INICIALIZACION----------------------*/

t_entrenador *iniciar_entrenador(int id, int posicion_x, int posicion_y, t_list*pokemones_capturados, t_list*pokemones_buscados) {
	t_entrenador*entrenador = malloc(sizeof(t_entrenador)); //cuidado con el malloc el tamaño de entrenador no es fijo
	entrenador->id = id;
	entrenador->posicion_x = posicion_x;
	entrenador->posicion_y = posicion_y;
	entrenador->esta_en_entrada_salida = false;
	entrenador->estimado_rafaga_proxima = ESTIMACION_INICIAL;
	entrenador->estimado_rafaga_anterior = -1;
	entrenador->real_rafaga_anterior = -1;
	entrenador->id_correlativo_esperado = -1;
	entrenador->semaforo = malloc(sizeof(sem_t));
	sem_init(entrenador->semaforo,0,0);
	entrenador->rafagas_intercambio_realizadas = 0;
	entrenador->tarea = NULL;
	entrenador->pokemones_capturados = pokemones_capturados;
	entrenador->pokemones_buscados = pokemones_buscados;
	entrenador->estado = NEW;


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
		t_list* pokemones_capturados = get_pokemones(team, i, 0);
		t_list* pokemones_objetivo = get_pokemones(team, i, 1);
		t_entrenador *entrenador = iniciar_entrenador(id, posicion_x, posicion_y,pokemones_capturados , pokemones_objetivo); //el estado lo hago sin pasar parametro por ahora

		list_add(entrenadores, entrenador);
		//printf("se cargo el entrenador: %d \n\n", i);

	}

	return entrenadores;
}


void ejecutar_ciclo_cpu(t_entrenador* entrenador){
	cambiar_estado(entrenador,EXEC);
	switch(entrenador->tarea->tipo_tarea){
		case ATRAPAR:{
			mover_entrenador_una_posicion(entrenador,entrenador->tarea->pokemon->posicion_x,entrenador->tarea->pokemon->posicion_y);
			break;
		}
		case INTERCAMBIO:{
			if(distancia_entrenador_entrenador(entrenador,entrenador->tarea->entrenador_intercambio) > 0)
				mover_entrenador_una_posicion(entrenador,entrenador->tarea->entrenador_intercambio->posicion_x,entrenador->tarea->entrenador_intercambio->posicion_y);
			else
				(entrenador->rafagas_intercambio_realizadas)++;
		}
	}
	cambiar_estado(entrenador,READY);
}

/*-----------------------------------------------------------PLANIFICACION----------------------*/

//funciones de estado
void cambiar_estado(t_entrenador* entrenador, t_state estado) {
	entrenador->estado = estado;

}
void entrenador_bloquear(t_entrenador *entrenador) { //para mas claridad
	cambiar_estado(entrenador, BLOCK);
}
bool entrenador_cumplio_objetivos(t_entrenador *entrenador) {
	return list_size(entrenador_pokemones_faltantes(entrenador)) == 0;
}
int entrenador_finalizar(t_entrenador *entrenador) {
	if (entrenador_cumplio_objetivos(entrenador)) {
		cambiar_estado(entrenador, EXIT);
		return 1;
	}
	return 0;
}

bool entrenador_finalizo(t_entrenador*entrenador) {
	return entrenador->estado == EXIT;
}


//funciones de objetivos
t_list * entrenador_pokemones_faltantes(t_entrenador*entrenador) { //los que le faltan por encontrar a cada entrenador

	return intersect_listas_pokemones(entrenador->pokemones_buscados, entrenador->pokemones_capturados);
}

t_list* entrenador_pokemones_sobrantes(t_entrenador*entrenador) {
	t_list*lista = entrenador->pokemones_capturados; //a los capturados les saco los que son el objetivo personal y me da los que no necesito...?mmm
	intersect_listas_pokemones(lista, entrenador_pokemones_faltantes(entrenador));

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
	return list_size(entrenador->pokemones_buscados)>list_size(entrenador->pokemones_capturados);
}


void mover_entrenador_una_posicion(t_entrenador*entrenador, int pos_dest_x, int pos_dest_y) {//si lo tengo que mover de a un paso cambiar los while por if


	if (entrenador->posicion_x != pos_dest_x) {//primero lo pongo a la coorod en eje horizontal
		if (entrenador->posicion_x < pos_dest_x) {
			entrenador->posicion_x++;
		} else {
			entrenador->posicion_x--;
		}
	}
	else if(entrenador->posicion_y != pos_dest_y) {//ahora en el eje vertical
		if (entrenador->posicion_y < pos_dest_y) {
			entrenador->posicion_y++;
		} else {
			entrenador->posicion_y--;
		}

	}
}



void mover_entrenador(t_entrenador*entrenador, int pos_dest_x, int pos_dest_y) {//si lo tengo que mover de a un paso cambiar los while por if


	while (entrenador->posicion_x != pos_dest_x) {//primero lo pongo a la coorod en eje horizontal
		if (entrenador->posicion_x < pos_dest_x) {
			entrenador->posicion_x++;
		} else {
			entrenador->posicion_x--;
		}
	}
	while (entrenador->posicion_y != pos_dest_y) {//ahora en el eje vertical
		if (entrenador->posicion_y < pos_dest_y) {
			entrenador->posicion_y++;
		} else {
			entrenador->posicion_y--;
		}

	}
}

int calcular_rafagas_necesarias(t_entrenador* entrenador){
	switch(entrenador->tarea->tipo_tarea){

	case ATRAPAR: {
		return distancia_entrenador_pokemon(entrenador,entrenador->tarea->pokemon);
	}

	case INTERCAMBIO:{
		return distancia_entrenador_pokemon(entrenador,entrenador->tarea->pokemon) + 5 - entrenador->rafagas_intercambio_realizadas;
	}
	default:
		return 0;
	}
}



void actualizar_estimados(t_entrenador* entrenador,int real_rafaga_anterior,int estimado_rafaga_anterior){
	entrenador->real_rafaga_anterior = real_rafaga_anterior;

	entrenador->estimado_rafaga_anterior = estimado_rafaga_anterior;

	//Est(n+1) = 𝝰 R(n) + ( 1 - 𝝰 ) Est(n)
	int estimado = ALPHA*real_rafaga_anterior + (1-ALPHA) * estimado_rafaga_anterior;
	entrenador->estimado_rafaga_proxima = estimado;
}

t_entrenador* buscar_entrenador_por_id_correlativo(t_list* entrenadores,int id_correlativo){
	bool mismo_id_correlativo(void* e){
		t_entrenador* entrenador = e;
		return ((entrenador->esta_en_entrada_salida) && (entrenador->id_correlativo_esperado == id_correlativo));
	}

	void* entrenador_encontrado = list_find(entrenadores,mismo_id_correlativo);
	t_entrenador* entrenador_encontrado_casteado = entrenador_encontrado;
	return entrenador_encontrado_casteado;
}

t_entrenador* buscar_entrenador_mas_cercano(t_list* entrenadores,t_pokemon* pokemon){
	bool menor_distancia(void* e1,void* e2){
		t_entrenador* entrenador1 = e1;
		t_entrenador* entrenador2 = e2;
		return distancia_entrenador_pokemon(entrenador1,pokemon) < distancia_entrenador_pokemon(entrenador2,pokemon);
	}
	list_sort(entrenadores,menor_distancia);
	return list_get(entrenadores,0);
}

void asignar_tarea_atrapar(t_entrenador* entrenador,t_team* team,t_pokemon* pokemon,sem_t* semaforo_readys){
	//TODO CADA VEZ QUE FINALIZA UNA TAREA TENGO QUE HACER FREE
	struct t_tarea* tarea = malloc(sizeof(struct t_tarea));
	tarea->pokemon = pokemon;
	tarea->tipo_tarea = ATRAPAR;
	tarea->entrenador_intercambio = NULL;
	tarea->pokemon_a_otorgar = NULL;
	tarea->pokemon_a_pedir = NULL;
	entrenador->tarea =tarea;
	entrenador->estado = READY;
	list_add(team->entrenadores_ready,entrenador);
	sem_post(semaforo_readys);
}

void remover_entrenador(t_list* entrenadores,t_entrenador* entrenador){
	bool mismo_id_entrenador(void* e){
		t_entrenador* e_1 = e;
		return e_1->id == entrenador->id;
	}

	list_remove_by_condition(entrenadores,mismo_id_entrenador);
}

t_list* obtener_objetivo_pokemones_restantes(t_list* entrenadores){
	t_list* pokemones_capturados = list_create();
	t_list* pokemones_objetivos = list_create();

	void agregar_pokemones_a_listas(void* e){
		t_entrenador* entrenador = e;
		list_add_all(pokemones_capturados,entrenador->pokemones_capturados);
		list_add_all(pokemones_objetivos,entrenador->pokemones_buscados);
	}
	list_iterate(entrenadores,agregar_pokemones_a_listas);

	intersect_listas_pokemones(pokemones_objetivos,pokemones_capturados);
	list_destroy(pokemones_capturados);
	return pokemones_objetivos;
}

