#include "Gameboy.h"
int main(void) {
	printf("Escriba el numero del  proceso:\n");
	printf("1. BROKER\n");
	printf("2. TEAM\n");
	printf("3. GAMECARD\n");
	int opcionProceso;
	scanf("%d",&opcionProceso);
	switch(opcionProceso){
	case 1:
		printf("Escriba el numero de mensaje:\n");
		printf("1. NEW_POKEMON\n");
		printf("2. APPEARED_POKEMON\n");
		printf("3. CATCH_POKEMON\n");
		printf("4. CAUGHT_POKEMON\n");
		printf("5. GET_POKEMON\n");
		break;
	case 2:
		printf("Proceso elegido: TEAM\n");
		break;
	case 3:
		printf("Proceso elegido: GAMECARD\n");
		break;
	default:
		printf("%d no es una opcion valida\n",opcionProceso);
	}


//		printf("Escriba el numero de mensaje:\n");
	return 0;
}


int connect_to_server(char* host,int port, void*(*callback)()) {

	int sock;

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(host);
	server_addr.sin_port = htons(port);

	sock = create_socket();

	if(connect(sock,(struct sockaddr *)&server_addr, sizeof(server_addr))< 0){
		perror("ERROR CONECTAR SERVIDOR");
		return -errno;
	}

	if(callback != NULL)
		callback();

	return sock;
}

int create_socket(){
	return socket(AF_INET, SOCK_STREAM, 0);
}
