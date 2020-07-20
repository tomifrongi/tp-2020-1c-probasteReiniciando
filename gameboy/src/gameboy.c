#include "gameboy.h"

//TODO caso de error de conexion con alguno de los modulos.
 int main(int argc, char *argv[]) {

	config = config_create("Gameboy.config");
	logger =  log_create("Gameboy.log", "Gameboy", 1, LOG_LEVEL_INFO);

//	imprimirOpciones();
	int contador = 0;
	t_list* argumentosMain = list_create();
	while(contador<argc){
		int size = strlen(argv[contador]);
		char* argumento = malloc(size+1);
		strcpy(argumento,argv[contador]);
		list_add(argumentosMain,argumento);
		contador++;
	}

	int opcionProceso;
	char* NombreProceso = list_get(argumentosMain,1);
	if(strcmp(NombreProceso,"BROKER") == 0)
		opcionProceso = 1;
	if (strcmp(NombreProceso,"TEAM") == 0)
		opcionProceso = 2;
	if (strcmp(NombreProceso,"GAMECARD") == 0)
		opcionProceso = 3;
	if (strcmp(NombreProceso,"SUSCRIPTOR") == 0)
		opcionProceso = 4;

//	scanf("%d",&opcionProceso);

	switch(opcionProceso){
	case 1:{
		ejecutarModoBroker(argumentosMain);
		break;
	}

	case 2:{
		ejecutarModoTeam(argumentosMain);
		break;
	}

	case 3:{
		ejecutarModoGamecard(argumentosMain);
		break;
	}

	case 4: {
		ejecutarModoSuscriptor(argumentosMain);
		break;
	}

	default:
		printf("%d no es una opcion valida\n",opcionProceso);
	}

	list_destroy_and_destroy_elements(argumentosMain,free);
	return 0;


}
