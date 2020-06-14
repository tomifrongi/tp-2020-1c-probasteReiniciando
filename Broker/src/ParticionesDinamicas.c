
#include "ParticionesDinamicas.h"

/*
 * 1) Se buscará una partición libre que tenga suficiente memoria continua como para contener el valor. En
 * caso de no encontrarla, se pasará al paso siguiente (si corresponde, en caso contrario se pasará al
 * paso 3 directamente).
 * 2) Se compactará la memoria y se realizará una nueva búsqueda. En caso de no encontrarla, se pasará al
 * paso siguiente.
 * 3) Se procederá a eliminar una partición de datos. Luego, si no se pudo encontrar una partición con
 * suficiente memoria como para contener el valor, se volverá al paso 2 o al 3 según corresponda.
 */

// Se deberá poder configurar la frecuencia de compactación (en la unidad “cantidad de búsquedas fallidas”).
// El valor -1 indicará compactar solamente cuando se hayan eliminado todas las particiones.

//OTRA EXPLICACION

/*
 * 1) Se busca alguna partición libre de tamaño suficiente.
 * 2) Si no la hay, se vacía una partición (según LRU o FIFO).
 * 3) Se busca nuevamente alguna partición libre de tamaño suficiente.
 * Los pasos 2 y 3 se repiten tantas veces como indique el valor de config.
 * Si es -1, solo al vaciarse todas las particiones se procede al paso 4.
 * 4) Si aun no hay lugar, se compacta la memoria. Compactar implica dos cosas:
 *		- Poner todas las particiones ocupadas de forma contigua, al principio de la memoria.
 *		- Eliminar particiones vacías contiguas, o sea convertirlas en una misma partición de
 *		  mayor tamaño.
 * 5) Volver al paso 1)
 */

//TODO tener en cuenta TAMANO_MINIMO_PARTICION


int main(){












	printf("Corriendo particiones dinamicas\n");
	TAMANO_MEMORIA = 2048;
	TAMANO_MINIMO_PARTICION = 64;
	ALGORITMO_REEMPLAZO = "FIFO";
	ALGORITMO_PARTICION_LIBRE = "FF";
	FRECUENCIA_COMPACTACION = 3;
	colaMensajesMemoria = queue_create();
	ultimasReferencias = list_create();


	principioMemoria = malloc(TAMANO_MEMORIA);
	particion_dinamica_memoria particionInicial;
	particionInicial.posicionParticion = 0;
	particionInicial.libre = 1;
	particionInicial.tamanio = TAMANO_MEMORIA;
	particionInicial.cola = -1;
	particionInicial.idCorrelativo = -1;
	particionInicial.idMensaje = -1;
	particionInicial.tamanioMensaje = -1;

	particionesEnMemoria = list_create();
	list_add(particionesEnMemoria,&particionInicial);




	particion_dinamica_memoria* p =list_get(particionesEnMemoria,0);
	int libre = p->libre;
	int tamanio = p->tamanio;
	printf("libre: %d \n",libre);
	printf("tamanio: %d \n",tamanio);
	bool busqueda = buscarParticionLibre(23);
	printf("busqueda: %d \n",busqueda);
//	new_pokemon_memoria menNew;
//	menNew.sizeNombre = 9;
//	menNew.nombrePokemon = "federico";
//	menNew.cantidad = 4;
//	menNew.posicionEjeX = 25;
//	menNew.posicionEjeY = 56;
//	cachearMensaje(&menNew,NEW);
//	printf("Finalizado particiones\n");
}


void inicializarMemoria(){
	principioMemoria = malloc(TAMANO_MEMORIA);
	particion_dinamica_memoria particionInicial;
	particionInicial.posicionParticion = 0;
	particionInicial.libre = 1;
	particionInicial.tamanio = TAMANO_MEMORIA;
	particionInicial.cola = -1;
	particionInicial.idCorrelativo = -1;
	particionInicial.idMensaje = -1;
	particionInicial.tamanioMensaje = -1;

	particionesEnMemoria = list_create();
	list_add(particionesEnMemoria,&particionInicial);

}

void cachearMensaje(void* mensaje,id_cola id){

	if(FRECUENCIA_COMPACTACION == -1)
		ejecutarCicloAlternativo(mensaje,id);
	else
		ejecutarCicloNormal(mensaje,id);
}

void ejecutarCicloNormal(void* mensaje,id_cola id){
	uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
	int busquedasFallidas = 0;
	while(1)
	{

		bool busqueda = buscarParticionLibre(tamanioMensaje);
		if(busqueda)
		{
			almacenarMensaje(mensaje,id); //uno por cada mensaje o podre inferir cual es?
			break;
		}
		else
		{
			if(busquedasFallidas<FRECUENCIA_COMPACTACION)
			{
				eliminarParticion();
				actualizarBusquedasFallidas(&busquedasFallidas);
			}
			else
				compactarMemoria();
				busquedasFallidas = 0;
		}
	}
}

void ejecutarCicloAlternativo(void* mensaje,id_cola id){
	uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
	while(particionesOcupadas())
	{
		bool busqueda = buscarParticionLibre(tamanioMensaje);
		if(busqueda)
		{
			almacenarMensaje(mensaje,id); //uno por cada mensaje o podre inferir cual es?
			break;
		}
		else
			eliminarParticion();
	}
	compactarMemoria();
	almacenarMensaje(mensaje,id);

}

bool particionesOcupadas(){

	bool particionOcupada(void* particion){
		particion_dinamica_memoria* particionCasteada = particion;
		return !(particionCasteada->libre);
	}
	return list_any_satisfy(particionesEnMemoria,particionOcupada);
}

void actualizarBusquedasFallidas(int* busquedasFallidas){
	(*busquedasFallidas)++;
}

void almacenarMensaje(void* mensaje,id_cola id){

	uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
	void* mensajeSerializado = serializarMensaje(mensaje,id);
	particion_dinamica_memoria* particion;

	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE,"FF"))
		particion = buscarPrimerParticionLibre(tamanioMensaje);
	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE,"BF"))
		particion = buscarMejorParticionLibre(tamanioMensaje);

	int posicionParticion = particion->posicionParticion;
	uint32_t tamanioParticionAntigua = particion->tamanio;
	particion = cargarDatosParticion(particion,mensaje,id);
	list_add(particionesEnMemoria,particion);

	if((particion->tamanio) < tamanioParticionAntigua)
		agregarParticionContigua(particion,tamanioParticionAntigua);

	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"FIFO"))
		queue_push(colaMensajesMemoria,particion->posicionParticion);
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"LRU"))
		list_add_in_index(ultimasReferencias,0,particion->posicionParticion);

	memcpy(principioMemoria+posicionParticion,mensajeSerializado,tamanioMensaje);
	free(mensajeSerializado);

}




void eliminarParticion(){
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"FIFO"))
	{
		void* posicionParticion = queue_pop(colaMensajesMemoria);
		void cambiarALibre(void* particion){
			particion_dinamica_memoria* particionCasteada = particion;
			if((particionCasteada->posicionParticion) == posicionParticion)
				particionCasteada->libre = true;

		}
		list_iterate(particionesEnMemoria, cambiarALibre);
	}
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"LRU"))
	{
		int posicionUltimoElemeneto= list_size(ultimasReferencias)-1;
		void* posicionParticion = list_remove(ultimasReferencias,posicionUltimoElemeneto);
		void cambiarALibre(void* particion){
			particion_dinamica_memoria* particionCasteada = particion;
			if((particionCasteada->posicionParticion) == posicionParticion)
				particionCasteada->libre = true;

		}
		list_iterate(particionesEnMemoria, cambiarALibre);
	}

}



void compactarMemoria(){

}

bool buscarParticionLibre(uint32_t tamanioMensaje){
	bool particionLibre(particion_dinamica_memoria* particion){
		if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
			return (TAMANO_MINIMO_PARTICION<=(particion->tamanio) && particion->libre);
		else
			return ((tamanioMensaje<=(particion->tamanio)) && particion->libre);
	}
	return list_any_satisfy(particionesEnMemoria,(void*)particionLibre);


}

particion_dinamica_memoria* buscarPrimerParticionLibre(uint32_t tamanioMensaje){

	bool particionLibre(void* particion){
		particion_dinamica_memoria* particionCasteada = particion;
		if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
			return (TAMANO_MINIMO_PARTICION<=(particionCasteada->tamanio) && particionCasteada->libre);
		else
			return (tamanioMensaje<=(particionCasteada->tamanio) && particionCasteada->libre);
	}
	return list_find(particionesEnMemoria,particionLibre);


}

particion_dinamica_memoria* buscarMejorParticionLibre(uint32_t tamanioMensaje){
	bool particionLibre(void* particion){
		particion_dinamica_memoria* particionCasteada = particion;
		if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
			return (TAMANO_MINIMO_PARTICION<=(particionCasteada->tamanio) && particionCasteada->libre);
		else
			return (tamanioMensaje<=(particionCasteada->tamanio) && particionCasteada->libre);
		}

	t_list* particionesLibres = list_filter(particionesEnMemoria,particionLibre);

	bool comparadorParticionesLibres(void* particion1,void* particion2){
		particion_dinamica_memoria* particion1Casteada = particion1;
		particion_dinamica_memoria* particion2Casteada = particion2;
		return (particion1Casteada->tamanio)<(particion2Casteada->tamanio);
	}
	list_sort(particionesLibres, comparadorParticionesLibres);

	particion_dinamica_memoria* mejorParticion = list_get(particionesLibres,0);
	list_destroy(particionesLibres);
	return mejorParticion;


}

void agregarParticionContigua(particion_dinamica_memoria* particion,uint32_t tamanioParticionAntigua){
	particion_dinamica_memoria particionContigua;

	particionContigua.cola = -1;
	particionContigua.idCorrelativo = -1;
	particionContigua.idMensaje = -1;
	particionContigua.libre = true;
	particionContigua.posicionParticion = (particion->posicionParticion) + (particion->tamanio);
	particionContigua.tamanio = tamanioParticionAntigua - (particion->tamanio);
	particionContigua.tamanioMensaje = 0;

	list_add(particionesEnMemoria,&particionContigua);



}

particion_dinamica_memoria* cargarDatosParticion(particion_dinamica_memoria* particion,void* mensaje,id_cola id){
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
			particion->tamanio = particion->tamanioMensaje;
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
			particion->tamanio = particion->tamanioMensaje;
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
			particion->tamanio = particion->tamanioMensaje;
		break;
	}
	case LOCALIZED: {
		localized_pokemon_enviar* lpe = mensaje;
		particion->cola = APPEARED;
		particion->idCorrelativo = lpe->idCorrelativo;
		particion->idMensaje = lpe->id_mensaje;
		particion->libre = false;
		uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
		particion->tamanioMensaje = tamanioMensaje;
		if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
			particion->tamanio = TAMANO_MINIMO_PARTICION;
		else
			particion->tamanio = particion->tamanioMensaje;
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
			particion->tamanio = particion->tamanioMensaje;
		break;
	}
	case CAUGHT:{
		caught_pokemon_enviar* cpe = mensaje;
		particion->cola = APPEARED;
		particion->idCorrelativo = cpe->idCorrelativo;
		particion->idMensaje = cpe->id_mensaje;
		particion->libre = false;
		uint32_t tamanioMensaje = obtenerTamanioMensaje(mensaje,id);
		particion->tamanioMensaje = tamanioMensaje;
		if(tamanioMensaje<TAMANO_MINIMO_PARTICION)
			particion->tamanio = TAMANO_MINIMO_PARTICION;
		else
			particion->tamanio = particion->tamanioMensaje;
		break;
	}
	}
	return particion;
}










