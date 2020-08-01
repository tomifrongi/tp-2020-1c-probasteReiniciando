#include "colasDeMensajes.h"


void crearEstructurasAdministrativas (){
	get_admin = malloc(sizeof(estructuraAdministrativa));
	localized_admin  = malloc(sizeof(estructuraAdministrativa));
	catch_admin  = malloc(sizeof(estructuraAdministrativa));
	caught_admin  = malloc(sizeof(estructuraAdministrativa));
	new_admin = malloc(sizeof(estructuraAdministrativa));
	appeared_admin = malloc(sizeof(estructuraAdministrativa));

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
	*elementoCreado = elemento;
	return elementoCreado;
}
void borrarElementoCola(uint32_t* elemento){
	free(elemento);
}

void eliminarIdCola(uint32_t idMensaje,id_cola idCola){
	bool igualIdMensaje(void* elementoCola){
		uint32_t* idMensajeCola = elementoCola;
		return *idMensajeCola == idMensaje;
	}
	switch(idCola){
	case NEW:{
		pthread_mutex_lock(mutexQueueNew);
		list_remove_and_destroy_by_condition(new_admin->queue,igualIdMensaje,(void*) borrarElementoCola);
		pthread_mutex_unlock(mutexQueueNew);
		break;
	}
	case APPEARED:{
		list_remove_and_destroy_by_condition(appeared_admin->queue,igualIdMensaje,(void*) borrarElementoCola);
		break;
	}
	case GET: {
		pthread_mutex_lock(mutexQueueGet);
		list_remove_and_destroy_by_condition(get_admin->queue,igualIdMensaje,(void*) borrarElementoCola);
		pthread_mutex_unlock(mutexQueueGet);
		break;
	}
	case LOCALIZED: {
		list_remove_and_destroy_by_condition(localized_admin->queue,igualIdMensaje,(void*) borrarElementoCola);
		break;
	}
	case CATCH: {
		pthread_mutex_lock(mutexQueueCatch);
		list_remove_and_destroy_by_condition(catch_admin->queue,igualIdMensaje,(void*) borrarElementoCola);
		pthread_mutex_unlock(mutexQueueCatch);
		break;
	}
	case CAUGHT: {
		list_remove_and_destroy_by_condition(caught_admin->queue,igualIdMensaje,(void*) borrarElementoCola);
		break;
	}
	}
}

void iniciarMutexs(){
	mutexId = malloc(sizeof(pthread_mutex_t));
	mutexMemoria= malloc(sizeof(pthread_mutex_t));

	mutexQueueNew = malloc(sizeof(pthread_mutex_t));
	mutexQueueAppeared = malloc(sizeof(pthread_mutex_t));
	mutexQueueGet= malloc(sizeof(pthread_mutex_t));
	mutexQueueLocalized= malloc(sizeof(pthread_mutex_t));
	mutexQueueCatch= malloc(sizeof(pthread_mutex_t));
	mutexQueueCaught= malloc(sizeof(pthread_mutex_t));

	mutexSuscriptoresNew= malloc(sizeof(pthread_mutex_t));
	mutexSuscriptoresAppeared= malloc(sizeof(pthread_mutex_t));
	mutexSuscriptoresGet= malloc(sizeof(pthread_mutex_t));
	mutexSuscriptoresLocalized= malloc(sizeof(pthread_mutex_t));
	mutexSuscriptoresCatch= malloc(sizeof(pthread_mutex_t));
	mutexSuscriptoresCaught= malloc(sizeof(pthread_mutex_t));

	pthread_mutex_init(mutexId,NULL);
	pthread_mutex_init(mutexMemoria,NULL);

	pthread_mutex_init(mutexQueueNew,NULL);
	pthread_mutex_init(mutexQueueAppeared,NULL);
	pthread_mutex_init(mutexQueueGet,NULL);
	pthread_mutex_init(mutexQueueLocalized,NULL);
	pthread_mutex_init(mutexQueueCatch,NULL);
	pthread_mutex_init(mutexQueueCaught,NULL);

	pthread_mutex_init(mutexSuscriptoresNew,NULL);
	pthread_mutex_init(mutexSuscriptoresAppeared,NULL);
	pthread_mutex_init(mutexSuscriptoresGet,NULL);
	pthread_mutex_init(mutexSuscriptoresLocalized,NULL);
	pthread_mutex_init(mutexSuscriptoresCatch,NULL);
	pthread_mutex_init(mutexSuscriptoresCaught,NULL);

}
