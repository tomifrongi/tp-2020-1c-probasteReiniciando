#include "colasDeMensajes.h"


void crearEstructurasAdministrativas (){
	estructuraAdministrativa* get_admin = malloc(sizeof(estructuraAdministrativa));
	estructuraAdministrativa* localized_admin  = malloc(sizeof(estructuraAdministrativa));
	estructuraAdministrativa* catch_admin  = malloc(sizeof(estructuraAdministrativa));
	estructuraAdministrativa* caught_admin  = malloc(sizeof(estructuraAdministrativa));
	estructuraAdministrativa* new_admin = malloc(sizeof(estructuraAdministrativa));
	estructuraAdministrativa* appeared_admin = malloc(sizeof(estructuraAdministrativa));

	get_admin->queue = list_create();
	localized_admin->queue = list_create();
	catch_admin->queue = list_create();
	caught_admin->queue = list_create();
	new_admin->queue = list_create();
	appeared_admin->queue = list_create();

	get_admin->suscriptores = list_create();
	localized_admin->suscriptores = list_create();
	catch_admin->suscriptores = list_create();
	caught_admin->suscriptores = list_create();
	new_admin->suscriptores = list_create();
	appeared_admin->suscriptores = list_create();


}

void destruirEstructurasAdministrativas(){
	list_destroy(get_admin.queue);
	list_destroy(localized_admin.queue);
	list_destroy(catch_admin.queue);
	list_destroy(new_admin.queue);
	list_destroy(appeared_admin.queue);
	list_destroy(caught_admin.queue);

	list_destroy(get_admin.suscriptores);
	list_destroy(localized_admin.suscriptores);
	list_destroy(catch_admin.suscriptores);
	list_destroy(new_admin.suscriptores);
	list_destroy(appeared_admin.suscriptores);
	list_destroy(caught_admin.suscriptores);
}

suscriptor* crearSuscriptor(suscriptor suscriptorNuevo){
	suscriptor* suscriptorNuevoCreado = malloc(sizeof(suscriptor));
	suscriptorNuevoCreado->idSuscriptor = suscriptorNuevo.idSuscriptor;
	suscriptorNuevoCreado->socket = suscriptorNuevo.socket;
	return suscriptorNuevo;
}
void borrarSuscriptor(suscriptor* suscriptor){
	free(suscriptor);
}
