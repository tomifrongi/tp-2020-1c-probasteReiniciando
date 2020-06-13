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
	t_config *config=leer_config();
	printf("cantidad teams:%d\n",cantidad_teams(config));

return 0;

}
