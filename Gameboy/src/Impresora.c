#include "Impresora.h"

void imprimirOpciones(){
	printf("Elige la opcion y escribe el numero:\n");
	printf("1. Enviar mensaje al BROKER\n");
	printf("2. Enviar mensaje al TEAM\n");
	printf("3. Enviar mensaje al GAMECARD\n");
	printf("4. Modo suscriptor\n");
}

void imprimirOpcionesMensajeBroker(){
	printf("Escribe el numero de mensaje:\n");
	printf("1. NEW_POKEMON\n");
	printf("2. APPEARED_POKEMON\n");
	printf("3. CATCH_POKEMON\n");
	printf("4. CAUGHT_POKEMON\n");
	printf("5. GET_POKEMON\n");
}

void imprimirOpcionesMensajeGamecard(){
	printf("Escribe el numero de mensaje:\n");
	printf("1. NEW_POKEMON\n");
	printf("2. CATCH_POKEMON\n");
	printf("3. GET_POKEMON\n");
}
