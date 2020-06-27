#include "BuddySystem.h"


void cachearMensajeBuddy(void* mensaje,id_cola id){
	uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
	bool busqueda;
	while(1)
	{
		busqueda = buscarParticionLibreBuddy(tamanioMensaje);
		if(busqueda)
		{
			almacenarMensajeBuddy(mensaje,id);
			break;
		}
		else
		{
			eliminarParticionBuddy();
			consolidarMemoriaBuddy();
		}
	}
}

bool buscarParticionLibreBuddy(uint32_t tamanioMensaje){
	bool particionLibre(particion_buddy_memoria* particion){
		if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
			return (TAMANO_MINIMO_PARTICION<=(particion->tamanio) && particion->libre);
		else
			return ((tamanioMensaje<=(particion->tamanio)) && particion->libre);
	}
	return list_any_satisfy(particionesEnMemoria,(void*)particionLibre);
}

void almacenarMensajeBuddy(void* mensaje,id_cola id){

	uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
	void* mensajeSerializado = serializarMensaje(mensaje,id);
	particion_buddy_memoria* particion;

	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE,"FF"))
		particion = buscarPrimerParticionLibreBuddy(tamanioMensaje);
	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE,"BF"))
		particion = buscarMejorParticionLibreBuddy(tamanioMensaje);
//--------------------------------------------------------------------------
	int potenciaDeDosMasCercana = buscarPotenciaDeDosMasCercana(tamanioMensaje);

	while(particion->tamanio>potenciaDeDosMasCercana){
		particion->tamanio = (particion->tamanio)/2;
		agregarBuddy(particion);
	}



	int posicionParticion = particion->posicionParticion;
	particion = cargarDatosParticionBuddy(particion,mensaje,id);
	list_add(particionesEnMemoria,particion);



	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"FIFO")){
		int* idMensaje = crear_elemento_colaMensajesMemoria(particion->idMensaje);
		queue_push(colaMensajesMemoria,idMensaje);
	}

	memcpy(principioMemoria+posicionParticion,mensajeSerializado,tamanioMensaje);
	free(mensajeSerializado);

}

particion_buddy_memoria* buscarPrimerParticionLibreBuddy(uint32_t tamanioMensaje){

	bool particionLibre(void* particion){
		particion_buddy_memoria* particionCasteada = particion;
		if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
			return (TAMANO_MINIMO_PARTICION<=(particionCasteada->tamanio) && particionCasteada->libre);
		else
			return (tamanioMensaje<=(particionCasteada->tamanio) && particionCasteada->libre);
	}
	return list_remove_by_condition(particionesEnMemoria,particionLibre);
}

particion_buddy_memoria* buscarMejorParticionLibreBuddy(uint32_t tamanioMensaje){
	bool particionLibre(void* particion){
		particion_buddy_memoria* particionCasteada = particion;
		if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
			return (TAMANO_MINIMO_PARTICION<=(particionCasteada->tamanio) && particionCasteada->libre);
		else
			return (tamanioMensaje<=(particionCasteada->tamanio) && particionCasteada->libre);
		}

	t_list* particionesLibres = list_filter(particionesEnMemoria,particionLibre);

	bool comparadorParticionesLibres(void* particion1,void* particion2){
		particion_buddy_memoria* particion1Casteada = particion1;
		particion_buddy_memoria* particion2Casteada = particion2;
		return (particion1Casteada->tamanio)<(particion2Casteada->tamanio);
	}
	list_sort(particionesLibres, comparadorParticionesLibres);

	particion_buddy_memoria* mejorParticionAuxiliar = list_remove(particionesLibres,0);
	list_destroy(particionesLibres);
	int idMejorParticion = mejorParticionAuxiliar->idMensaje;
	borrar_particion_dinamica_memoria(mejorParticionAuxiliar);

	bool particionMismoIdMensaje(void* particion){
		particion_buddy_memoria* particionCasteada = particion;
		return (particionCasteada->idMensaje) == idMejorParticion;
	}
	particion_buddy_memoria* mejorParticion = list_remove_by_condition(particionesEnMemoria,particionMismoIdMensaje);
	return mejorParticion;

}

int buscarPotenciaDeDosMasCercana(uint32_t tamanio){

	int potencia;
	int exponente = 0;
	while(1){
		potencia = pow(2,exponente);
		if(tamanio<=potencia)
			return potencia;
		exponente++;
	}
}

void agregarBuddy(particion_buddy_memoria* particion){}


particion_buddy_memoria* cargarDatosParticionBuddy(particion_buddy_memoria* particion,void* mensaje,id_cola id){
	return NULL;
}

void eliminarParticionBuddy(){}
void consolidarMemoriaBuddy(){}
