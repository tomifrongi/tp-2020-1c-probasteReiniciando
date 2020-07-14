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
	list_destroy_and_destroy_elements(get_admin->queue,(void*)borrarElementoCola);
	list_destroy_and_destroy_elements(localized_admin->queue,(void*)borrarElementoCola);
	list_destroy_and_destroy_elements(catch_admin->queue,(void*)borrarElementoCola);
	list_destroy_and_destroy_elements(new_admin->queue,(void*)borrarElementoCola);
	list_destroy_and_destroy_elements(appeared_admin->queue,(void*)borrarElementoCola);
	list_destroy_and_destroy_elements(caught_admin->queue,(void*)borrarElementoCola);

	list_destroy_and_destroy_elements(get_admin->suscriptores,(void*)borrarSuscriptor);
	list_destroy_and_destroy_elements(localized_admin->suscriptores,(void*)borrarSuscriptor);
	list_destroy_and_destroy_elements(catch_admin->suscriptores,(void*)borrarSuscriptor);
	list_destroy_and_destroy_elements(new_admin->suscriptores,(void*)borrarSuscriptor);
	list_destroy_and_destroy_elements(appeared_admin->suscriptores,(void*)borrarSuscriptor);
	list_destroy_and_destroy_elements(caught_admin->suscriptores,(void*)borrarSuscriptor);
}

suscriptor* crearSuscriptor(suscriptor suscriptorNuevo){
	suscriptor* suscriptorNuevoCreado = malloc(sizeof(suscriptor));
	suscriptorNuevoCreado->idSuscriptor = suscriptorNuevo.idSuscriptor;
	suscriptorNuevoCreado->socket = suscriptorNuevo.socket;
	return suscriptorNuevoCreado;
}
void borrarSuscriptor(suscriptor* suscriptor){
	free(suscriptor);
}

uint32_t* crearElementoCola(uint32_t elemento){
	uint32_t* elementoCreado = malloc(sizeof(uint32_t));
	return elementoCreado;
}
void borrarElementoCola(uint32_t* elemento){
	free(elemento);
}
