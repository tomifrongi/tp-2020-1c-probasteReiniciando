#include "Scanner.h"

new_pokemon_broker obtenerParametrosNewBroker() {
	new_pokemon_broker parametros;

	printf("Ingrese el nombre del pokemon:\n");
	char nombrePokemon[30];
	scanf("%s", nombrePokemon);
	parametros.nombrePokemon = nombrePokemon;
	parametros.sizeNombre = strlen(parametros.nombrePokemon);

	printf("Ingrese la posicion X:\n");
	scanf("%d",&parametros.posicionEjeX);

	printf("Ingrese la posicion Y:\n");
	scanf("%d",&parametros.posicionEjeY);

	printf("Ingrese la cantidad:\n");
	scanf("%d", &parametros.cantidad);

	return parametros;

}

//TODO
appeared_pokemon_broker obtenerParametrosAppearedBroker(){
	appeared_pokemon_broker parametros;
	return parametros;
}

//TODO
catch_pokemon_broker obtenerParametrosCatchBroker(){
	catch_pokemon_broker parametros;
	return parametros;
}

//TODO
caught_pokemon_broker obtenerParametrosCaughtBroker(){
	caught_pokemon_broker parametros;
	return parametros;
}

//TODO
get_pokemon_broker obtenerParametrosGetBroker(){
	get_pokemon_broker parametros;
	return parametros;
}
