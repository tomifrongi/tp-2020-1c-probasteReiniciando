#include "Gameboy.h"

//TODO revisar que esten todos y bien todos los logs
//TODO caso de error de conexion con alguno de los modulos.
//TODO revisar configs
 int main(void) {

	config = config_create("Gameboy.config");
	logger =  log_create("Gameboy.log", "Gameboy", 1, LOG_LEVEL_INFO);

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

	case 4: {
		ejecutarModoSuscriptor();
		break;
	}

	default:
		printf("%d no es una opcion valida\n",opcionProceso);
	}

	return 0;


}
