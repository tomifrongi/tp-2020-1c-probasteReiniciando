#include "team.h"
#include <stdbool.h>
#include <commons/string.h>

t_list * objetivo_global(t_team*team) {
t_list *objetivo = list_create();
void foo(t_entrenador*entrenador) {
	list_add_all(objetivo, objetivo_personal(entrenador));
}
list_iterate(team->entrenadores, foo);
return objetivo;
}

void cargar_objetivo_global(t_team*team) {
team->objetivo_global = objetivo_global(team);
}

t_list*mapa [MAP_SIZE][MAP_SIZE];//mapa bidimencional de listas

/*
t_list*pokemones_sueltos(t_list*mapa[][]){ //por si termino iterando todoo el mapa
	t_list*sueltos=list_create();

	return sueltos;
}*/
t_list *pokemones_sueltos=list_create();
// es una lista pero seran pocos  lugares,por que  que si sirve se lo captura y que si no sirve se borra
//deberia tener la logica de ser mas una cola para el tema de las prioridades

bool pokemon_suelto(void *mapa[][]){
	return list_size(pokemones_sueltos)>0;
}
bool cumplio_objetivo_global(t_team*team){
	return( pokemon_suelto(team->mapa)==false && list_is_empty(objetivo_global(team))==true);//no se que onda el bool
}

/*
bool estado_deadlock(t_team*team){
	return ( pokemon_suelto(team->mapa)==false && list_is_empty(objetivo_global(team))==true&&)

}*/ //no es tan facil como parece
