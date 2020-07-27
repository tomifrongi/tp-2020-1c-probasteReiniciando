#include "main.h"

int main(){
	cargar_configuracion();
	inicializar_logger();

	pthread_t appeared_thread;
	pthread_t localized_thread;
	pthread_t caught_thread;


	uint32_t a = APPEARED;
	uint32_t l = LOCALIZED;
	uint32_t c = CAUGHT;
	pthread_create(&appeared_thread, NULL, (void*) handler_broker, &a);
	pthread_detach(appeared_thread);
	pthread_create(&localized_thread, NULL, (void*) handler_broker, &l);
	pthread_detach(localized_thread);
	pthread_create(&caught_thread, NULL, (void*) handler_broker, &c);
	pthread_detach(caught_thread);

	int listener_socket = init_server(PUERTO_TEAM);
	escuchar_mensajes_gameboy(listener_socket);


	return 0;
}
