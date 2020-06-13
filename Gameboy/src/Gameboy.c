#include "Gameboy.h"



 int main(void) {

	config = config_create("Gameboy.config");
	logger =  log_create("Gameboy.log", "Gameboy", 1, LOG_LEVEL_INFO);
//	TODO Llegada de un nuevo mensaje a una cola de mensajes.

	imprimirOpciones();

	int opcionProceso;
	scanf("%d",&opcionProceso);

	switch(opcionProceso){
	case 1:{
		ejecutarModoBroker();
		break;
	}

	case 2:{
		ejecutarModoTeam();
		break;
	}

	case 3:{
		ejecutarModoGamecard();
		break;
	}
//TODO modoSuscriptor
	case 4: {
		ejecutarModoSuscriptor();
		break;
	}

	default:
		printf("%d no es una opcion valida\n",opcionProceso);
	}

	return 0;


}
