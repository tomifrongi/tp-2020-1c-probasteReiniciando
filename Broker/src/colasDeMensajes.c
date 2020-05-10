#include "colasDeMensajes.h"


void crearEstructurasAdministrativas (){
	get_admin.queue = queue_create();
	localized_admin.queue = queue_create();
	catch_admin.queue = queue_create();
	caught_admin.queue = queue_create();
	new_admin.queue = queue_create();
	appeared_admin.queue = queue_create();

	get_admin.suscriptores = list_create();
	localized_admin.suscriptores = list_create();
	catch_admin.suscriptores = list_create();
	caught_admin.suscriptores = list_create();
	new_admin.suscriptores = list_create();
	appeared_admin.suscriptores = list_create();


}

void destruirEstructurasAdministrativas(){
	queue_destroy(get_admin.queue);
	queue_destroy(localized_admin.queue);
	queue_destroy(catch_admin.queue);
	queue_destroy(new_admin.queue);
	queue_destroy(appeared_admin.queue);
	queue_destroy(caught_admin.queue);

	list_destroy(get_admin.suscriptores);
	list_destroy(localized_admin.suscriptores);
	list_destroy(catch_admin.suscriptores);
	list_destroy(new_admin.suscriptores);
	list_destroy(appeared_admin.suscriptores);
	list_destroy(caught_admin.suscriptores);
}

