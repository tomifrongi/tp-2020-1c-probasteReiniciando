#include "Gameboy.h"

//TODO caso de error de conexion con alguno de los modulos.
 int main(int argc, char *argv[]) {

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
