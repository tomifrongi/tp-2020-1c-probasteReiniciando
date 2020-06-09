#include<commons/config.h>
#include<commons/string.h>
#include<commons/collections/list.h>
#include "config.h"
#include "entrenadores.h"
#include "pokemones.h"
/*
void* intersect_listas2(t_list*buscados, t_list*capturados) //creo que no hace falta duplicar lista por que solo paso el valor
{
	void buscar_y_borrar(t_pokemon*p) {
		bool mismo_pokemon(t_pokemon*p2) {
			return string_equals_ignore_case(p->especie, p2->especie);

		}
		list_find(capturados, mismo_pokemon);
	list_remove_and_destroy_by_condition( list1,bool(*condition)(void*),
			void (*element_destroyer)(void*));

}
list_iterate(buscados, buscar_y_borrar);
}*/

t_list* intersect_listas2(t_list*buscados, t_list*capturados) //creo que no hace falta duplicar lista por que solo paso el valor
{
for (int i = 0; i < list_size(buscados); i++) {
	t_pokemon*poke_busc = list_get(buscados, i);

	for (int j = 0; j < list_size(capturados); j++) {
		t_pokemon*poke_cap = list_get(capturados, j);
		if (misma_especie(poke_busc, poke_cap)){
			list_remove(buscados, i);
			list_remove(capturados,j);

		}
	}
}
return buscados;
}

int main() {
/*
	t_list*pokes1=list_create();
	t_list*pokes2=list_create();
	t_pokemon*poke1=malloc(sizeof(t_pokemon));
	t_pokemon*poke2=malloc(sizeof(t_pokemon));
	t_pokemon*poke3=malloc(sizeof(t_pokemon));
	t_pokemon*poke4=malloc(sizeof(t_pokemon));

	poke1->especie="pelotudo";
	poke2->especie="cornudo";
	poke3->especie="cornudo";
	poke4->especie="mas cornudo";
	list_add(pokes1,poke1);
	list_add(pokes1,poke2);

	list_add(pokes2,poke3);

	list_add(pokes2,poke4);
	t_entrenador*entrenador=malloc(sizeof(t_entrenador));
	entrenador->pokemones_buscados=pokes1;
	entrenador->pokemones_capturados=pokes2;

	mostrar_pokemones(pokemones_sobrantes(entrenador));*/



return 0;

}
