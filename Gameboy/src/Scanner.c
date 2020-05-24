#include "Scanner.h"

new_pokemon_broker obtenerParametrosNewBroker() {
	new_pokemon_broker parametros;

	printf("Ingrese el nombre del pokemon:\n");
	char nombrePokemon[30];
	scanf("%s", nombrePokemon);
	parametros.nombrePokemon = nombrePokemon;
	parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;

	printf("Ingrese la posicion X:\n");
	scanf("%d",&parametros.posicionEjeX);

	printf("Ingrese la posicion Y:\n");
	scanf("%d",&parametros.posicionEjeY);

	printf("Ingrese la cantidad:\n");
	scanf("%d", &parametros.cantidad);

	return parametros;

}

appeared_pokemon_broker obtenerParametrosAppearedBroker(){
	appeared_pokemon_broker parametros;

	printf("Ingrese el nombre del pokemon:\n");
	char nombrePokemon[30];
	scanf("%s", nombrePokemon);
	parametros.nombrePokemon = nombrePokemon;
	parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;

	printf("Ingrese la posicion X:\n");
	scanf("%d",&parametros.posicionEjeX);

	printf("Ingrese la posicion Y:\n");
	scanf("%d",&parametros.posicionEjeY);

	printf("Ingrese el id_mensaje_correlativo:\n");
	scanf("%d", &parametros.idCorrelativo);

	return parametros;
}

catch_pokemon_broker obtenerParametrosCatchBroker(){
	catch_pokemon_broker parametros;

	printf("Ingrese el nombre del pokemon:\n");
	char nombrePokemon[30];
	scanf("%s", nombrePokemon);
	parametros.nombrePokemon = nombrePokemon;
	parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;

	printf("Ingrese la posicion X:\n");
	scanf("%d",&parametros.posicionEjeX);

	printf("Ingrese la posicion Y:\n");
	scanf("%d",&parametros.posicionEjeY);

	return parametros;
}

caught_pokemon_broker obtenerParametrosCaughtBroker(){
	caught_pokemon_broker parametros;

	printf("Ingrese el id_mensaje_correlativo:\n");
	scanf("%d", &parametros.idCorrelativo);

	printf("Ingrese 1 si fue atrapado y 0 si no:\n");
	scanf("%d", &parametros.pokemonAtrapado);

	return parametros;
}

get_pokemon_broker obtenerParametrosGetBroker(){
	get_pokemon_broker parametros;

	printf("Ingrese el nombre del pokemon:\n");
	char nombrePokemon[30];
	scanf("%s", nombrePokemon);
	parametros.nombrePokemon = nombrePokemon;
	parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;

	return parametros;
}


new_pokemon_gamecard obtenerParametrosNewGamecard(){
	new_pokemon_gamecard parametros;

	printf("Ingrese el nombre del pokemon:\n");
	char nombrePokemon[30];
	scanf("%s", nombrePokemon);
	parametros.nombrePokemon = nombrePokemon;
	parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;

	printf("Ingrese la posicion X:\n");
	scanf("%d",&parametros.posicionEjeX);

	printf("Ingrese la posicion Y:\n");
	scanf("%d",&parametros.posicionEjeY);

	printf("Ingrese la cantidad:\n");
	scanf("%d", &parametros.cantidad);

	printf("Ingrese el id_mensaje mayor a 10000:\n");
	scanf("%d", &parametros.id_mensaje);

	return parametros;
}

catch_pokemon_gamecard obtenerParametrosCatchGamecard(){
	catch_pokemon_gamecard parametros;

	printf("Ingrese el nombre del pokemon:\n");
	char nombrePokemon[30];
	scanf("%s", nombrePokemon);
	parametros.nombrePokemon = nombrePokemon;
	parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;

	printf("Ingrese la posicion X:\n");
	scanf("%d",&parametros.posicionEjeX);

	printf("Ingrese la posicion Y:\n");
	scanf("%d",&parametros.posicionEjeY);


	printf("Ingrese el id_mensaje mayor a 10000:\n");
	scanf("%d", &parametros.id_mensaje);
	return parametros;
}

get_pokemon_gamecard obtenerParametrosGetGamecard(){
	get_pokemon_gamecard parametros;

	printf("Ingrese el nombre del pokemon:\n");
	char nombrePokemon[30];
	scanf("%s", nombrePokemon);
	parametros.nombrePokemon = nombrePokemon;
	parametros.sizeNombre = strlen(parametros.nombrePokemon)+1;

	printf("Ingrese el id_mensaje mayor a 10000:\n");
	scanf("%d", &parametros.id_mensaje);

	return parametros;
}
