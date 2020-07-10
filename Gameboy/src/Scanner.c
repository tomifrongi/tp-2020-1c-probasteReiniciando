#include "Scanner.h"

new_pokemon_broker obtenerParametrosNewBroker() {
	new_pokemon_broker parametros;

	printf("Ingrese el nombre del pokemon:\n");
	char nombrePokemon[30];
	scanf("%s", nombrePokemon);
	parametros.nombrePokemon = &nombrePokemon[0];
	//parametros.nombrePokemon = nombrePokemon;
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

id_cola obtenerID(){
	printf("Escriba el numero de cola:\n");
	printf("1. NEW\n");
	printf("2. APPEARED\n");
	printf("3. GET\n");
	printf("4. LOCALIZED\n");
	printf("5. CATCH\n");
	printf("6. CAUGHT\n");

	id_cola id;
	scanf("%d",&id);
	id--;

	return id;
}

appeared_pokemon_team obtenerParametrosAppearedTeam(){
	appeared_pokemon_team parametros;
	parametros.id_mensaje = 10100;
	parametros.idCorrelativo = 0;

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
