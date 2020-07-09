#include "BuddySystem.h"

void borrar_suscriptorBuddy(void* algo){}


void cachearMensajeBuddy(void* mensaje,id_cola id){
	while(1)
	{
		if(almacenarMensajeBuddy(mensaje,id))
			break;
		else{
			eliminarParticionBuddy();
			consolidarMemoriaBuddy();
		}
	}
}

void sacarBarraCeroBuddy(void* mensaje,id_cola id){
	switch(id){
		case NEW: {
			new_pokemon_enviar* np = mensaje;
			np->sizeNombre -=1;
			break;
		}
		case APPEARED: {
			appeared_pokemon_enviar* ap = mensaje;
			ap->sizeNombre -=1;
			break;
		}
		case GET: {
			get_pokemon_enviar* gp = mensaje;
			gp->sizeNombre -=1;
			break;
		}
		case LOCALIZED: {
			localized_pokemon_enviar* lp = mensaje;
			lp->sizeNombre -=1;
			break;
		}
		case CATCH:{
			catch_pokemon_enviar* catchp = mensaje;
			catchp->sizeNombre -=1;
			break;
		}
		case CAUGHT:{
			break;
		}
		}

}

bool almacenarMensajeBuddy(void* mensaje,id_cola id){

	sacarBarraCeroBuddy(mensaje,id);
	uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
	void* mensajeSerializado = serializarMensaje(mensaje,id);
	particion_buddy_memoria* particion;

	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE,"FF"))
		particion = buscarPrimerParticionLibreBuddy(tamanioMensaje);
	else if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE,"BF"))
		particion = buscarMejorParticionLibreBuddy(tamanioMensaje);
	if(particion==NULL)
		return false;
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
		int* idMensaje = crear_elemento_colaMensajesMemoriaBuddy(particion->idMensaje);
		queue_push(colaMensajesMemoria,idMensaje);
	}

	memcpy(principioMemoria+posicionParticion,mensajeSerializado,tamanioMensaje);
	free(mensajeSerializado);

	return true;




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
	borrar_particion_buddy_memoria(mejorParticionAuxiliar);

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

void agregarBuddy(particion_buddy_memoria* particion){

	particion_buddy_memoria particionBuddy;
	particionBuddy.libre = true;
	particionBuddy.tamanio = particion->tamanio;
	particionBuddy.posicionParticion = particion->posicionParticion + particion->tamanio;
	particion_buddy_memoria* particionBuddyCreada = crear_particion_buddy_memoria(particionBuddy);
	list_add(particionesEnMemoria,particionBuddyCreada);

}

particion_buddy_memoria* crear_particion_buddy_memoria(particion_buddy_memoria particion){
	particion_buddy_memoria* nuevaParticion = malloc(sizeof(particion_buddy_memoria));

	nuevaParticion->posicionParticion = particion.posicionParticion;
	nuevaParticion->libre = particion.libre;
	nuevaParticion->tamanio = particion.tamanio;
	nuevaParticion->tamanioMensaje = particion.tamanioMensaje;
	nuevaParticion->idMensaje = particion.idMensaje;
	nuevaParticion->idCorrelativo = particion.idCorrelativo;
	nuevaParticion->cola = particion.cola;
	nuevaParticion->contadorLRU = particion.contadorLRU;
	nuevaParticion->suscriptoresMensajeEnviado = list_create();
	nuevaParticion->suscriptoresACK = list_create();

	return nuevaParticion;
}

void borrar_particion_buddy_memoria(particion_buddy_memoria* particion){


	list_destroy_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptorBuddy);
	list_destroy_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptorBuddy);

	free(particion);
}


particion_buddy_memoria* cargarDatosParticionBuddy(particion_buddy_memoria* particion,void* mensaje,id_cola id){
	switch(id){
		case NEW: {
			new_pokemon_enviar* npe = mensaje;
			particion->cola = NEW;
			particion->idCorrelativo = -1;
			particion->idMensaje = npe->id_mensaje;
			particion->libre = false;
			uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
			particion->tamanioMensaje = tamanioMensaje;
			if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
				particion->tamanio = TAMANO_MINIMO_PARTICION;
			else
				particion->tamanio = buscarPotenciaDeDosMasCercana(particion->tamanioMensaje);
			CONTADORLRU++;
			particion->contadorLRU = CONTADORLRU;
			list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptorBuddy);
			list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptorBuddy);
			break;
		}
		case APPEARED: {
			appeared_pokemon_enviar* ape = mensaje;
			particion->cola = APPEARED;
			particion->idCorrelativo = ape->idCorrelativo;
			particion->idMensaje = ape->id_mensaje;
			particion->libre = false;
			uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
			particion->tamanioMensaje = tamanioMensaje;
			if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
				particion->tamanio = TAMANO_MINIMO_PARTICION;
			else
				particion->tamanio = buscarPotenciaDeDosMasCercana(particion->tamanioMensaje);
			CONTADORLRU++;
			particion->contadorLRU = CONTADORLRU;
			list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptorBuddy);
			list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptorBuddy);
			break;
		}
		case GET: {
			get_pokemon_enviar* gpe = mensaje;
			particion->cola = GET;
			particion->idCorrelativo = -1;
			particion->idMensaje = gpe->id_mensaje;
			particion->libre = false;
			uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
			particion->tamanioMensaje = tamanioMensaje;
			if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
				particion->tamanio = TAMANO_MINIMO_PARTICION;
			else
				particion->tamanio = buscarPotenciaDeDosMasCercana(particion->tamanioMensaje);
			CONTADORLRU++;
			particion->contadorLRU = CONTADORLRU;
			list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptorBuddy);
			list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptorBuddy);
			break;
		}
		case LOCALIZED: {
			localized_pokemon_enviar* lpe = mensaje;
			particion->cola = LOCALIZED;
			particion->idCorrelativo = lpe->idCorrelativo;
			particion->idMensaje = lpe->id_mensaje;
			particion->libre = false;
			uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
			particion->tamanioMensaje = tamanioMensaje;
			if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
				particion->tamanio = TAMANO_MINIMO_PARTICION;
			else
				particion->tamanio = buscarPotenciaDeDosMasCercana(particion->tamanioMensaje);
			CONTADORLRU++;
			particion->contadorLRU = CONTADORLRU;
			list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptorBuddy);
			list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptorBuddy);
			break;
		}
		case CATCH:{
			catch_pokemon_enviar* cpe = mensaje;
			particion->cola = CATCH;
			particion->idCorrelativo = -1;
			particion->idMensaje = cpe->id_mensaje;
			particion->libre = false;
			uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
			particion->tamanioMensaje = tamanioMensaje;
			if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
				particion->tamanio = TAMANO_MINIMO_PARTICION;
			else
				particion->tamanio = buscarPotenciaDeDosMasCercana(particion->tamanioMensaje);
			CONTADORLRU++;
			particion->contadorLRU = CONTADORLRU;
			list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptorBuddy);
			list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptorBuddy);
			break;
		}
		case CAUGHT:{
			caught_pokemon_enviar* cpe = mensaje;
			particion->cola = CAUGHT;
			particion->idCorrelativo = cpe->idCorrelativo;
			particion->idMensaje = cpe->id_mensaje;
			particion->libre = false;
			uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
			particion->tamanioMensaje = tamanioMensaje;
			if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
				particion->tamanio = TAMANO_MINIMO_PARTICION;
			else
				particion->tamanio = particion->tamanioMensaje;
			CONTADORLRU++;
			particion->contadorLRU = CONTADORLRU;
			list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptorBuddy);
			list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptorBuddy);
			break;
		}
		}
		return particion;

}
//TODO corregir gameboy

t_list* sacarParticionesLibresBuddy(){
	bool particionLibre(void* particion){
		particion_buddy_memoria* particionCasteada = particion;
		return !(particionCasteada->libre);
	}
	 return list_filter(particionesEnMemoria,particionLibre);

};


void eliminarParticionBuddy(){

	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"FIFO"))
		{
			int* idMensaje = queue_pop(colaMensajesMemoria);
			int idMensajeAuxiliar = *idMensaje;
			borrar_elemento_colaMensajesMemoriaBuddy(idMensaje);

			void cambiarALibre(void* particion){
				particion_buddy_memoria* particionCasteada = particion;
				if((particionCasteada->idMensaje) == idMensajeAuxiliar)
					particionCasteada->libre = true;
			}
			list_iterate(particionesEnMemoria, cambiarALibre);
		}
	else if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"LRU"))
		{

			t_list* particionesOcupadas = sacarParticionesLibresBuddy();

			bool comparadorParticionesLibresPorLRU(void* particion1,void* particion2){
				particion_buddy_memoria* particion1Casteada = particion1;
				particion_buddy_memoria* particion2Casteada = particion2;
				return (particion1Casteada->contadorLRU)<(particion2Casteada->contadorLRU);
			}
			list_sort(particionesOcupadas,comparadorParticionesLibresPorLRU);

			particion_buddy_memoria* particionMenosUsada = list_get(particionesOcupadas,0);
			int idMensaje = particionMenosUsada->idMensaje;
			list_destroy(particionesOcupadas);
			void cambiarALibre(void* particion){
				particion_buddy_memoria* particionCasteada = particion;
				if((particionCasteada->idMensaje) == idMensaje)
					particionCasteada->libre = true;

			}
			list_iterate(particionesEnMemoria, cambiarALibre);
		}

}

void consolidarMemoriaBuddy(){

    ordenarParticionesPorPosicionBuddy();
        int sizeLista = list_size(particionesEnMemoria);
        int index = 0;
        int indexAdyacente = index+1;
        while(indexAdyacente<sizeLista){
            particion_buddy_memoria* particion = list_get(particionesEnMemoria,index);
            particion_buddy_memoria* particionAdyacente = list_get(particionesEnMemoria,indexAdyacente);

            if(particion->libre && particionAdyacente->libre){

                if(particion->tamanio == particionAdyacente->tamanio){
                    particion->tamanio+=particionAdyacente->tamanio;
                    int posicion = particionAdyacente->posicionParticion;
                    particionAdyacente = removerPorPosicionBuddy(posicion);

                    borrar_particion_buddy_memoria(particionAdyacente);

                    sizeLista = list_size(particionesEnMemoria);
                    index --;
                    indexAdyacente --;
                }

            }
            else if(particion->tamanio == particionAdyacente->tamanio){
                index ++;
                indexAdyacente ++;
            }
            index ++;
            indexAdyacente ++;
        }
}


int* crear_elemento_colaMensajesMemoriaBuddy(int idMensaje){
	int* newIdMensaje = malloc(sizeof(int));
	*newIdMensaje = idMensaje;
	return newIdMensaje;
}

void borrar_elemento_colaMensajesMemoriaBuddy(int* idMensaje){
	free(idMensaje);
}

particion_buddy_memoria* removerPorPosicionBuddy(int posicion){
	bool compararPorId(void* particion){
		particion_buddy_memoria* particionCasteada = particion;
		return particionCasteada->posicionParticion == posicion;
	}

	return list_remove_by_condition(particionesEnMemoria,compararPorId);
}

void ordenarParticionesPorPosicionBuddy(){
	bool comparadorParticionesPorPosicion(void* particion1,void* particion2){
		particion_buddy_memoria* particion1Casteada = particion1;
		particion_buddy_memoria* particion2Casteada = particion2;
		return (particion1Casteada->posicionParticion)<(particion2Casteada->posicionParticion);
	}
	list_sort(particionesEnMemoria, comparadorParticionesPorPosicion);
}
