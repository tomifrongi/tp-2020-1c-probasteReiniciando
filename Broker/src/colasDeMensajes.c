#include "colasDeMensajes.h"


void crearColas (){
	get_pokemon_queue = queue_create();
	localized_pokemon_queue = queue_create();
	catch_pokemon_queue = queue_create();
	caught_pokemon_queue = queue_create();
	new_pokemon_queue = queue_create();
	appeared_pokemon_queue = queue_create();
}

void destruirColas(){
	queue_destroy(get_pokemon_queue);
	queue_destroy(localized_pokemon_queue);
	queue_destroy(catch_pokemon_queue);
	queue_destroy(new_pokemon_queue);
	queue_destroy(appeared_pokemon_queue);
	queue_destroy(caught_pokemon_queue);
}
