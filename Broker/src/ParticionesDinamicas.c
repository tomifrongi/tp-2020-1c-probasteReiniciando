
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

	inicializarMemoria();
	colaMensajesMemoria = queue_create();
	ultimasReferencias = list_create();

	new_pokemon_memoria menNew;
	menNew.sizeNombre = 9;
	menNew.nombrePokemon = "federico";
	menNew.cantidad = 4;
	menNew.posicionEjeX = 25;
	menNew.posicionEjeY = 56;

	int i = 1;
	while(i <= 50){
		cachearMensaje(menNew);
		i++;
	}
	eliminarParticion();
	cachearMensaje(menNew);
	printf("finalizacion\n");
	return 0;
}



void cachearMensaje(new_pokemon_memoria mensaje){ //no es un int pero pongo algo para que funque y creo que tambien iria memoria como parametro

	if(FRECUENCIA_COMPACTACION == -1)
		ejecutarCicloAlternativo(mensaje);
	else
		ejecutarCicloNormal(mensaje);
}

void ejecutarCicloNormal(new_pokemon_memoria mensaje){
	uint32_t tamanioMensaje = sizeof(uint32_t)*4 + mensaje.sizeNombre;
	uint32_t tamanioHeapYMensaje = sizeof(heapDinamico)+tamanioMensaje;
	while(1)
	{
		int busquedasFallidas = 0;
		bool busqueda = buscarParticionLibre(tamanioMensaje);
		if(busqueda)
		{
			almacenarMensajeNew(mensaje); //uno por cada mensaje o podre inferir cual es?
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
		}
	}
}

void ejecutarCicloAlternativo(new_pokemon_memoria mensaje){
	uint32_t tamanioMensaje = sizeof(uint32_t)*4 + mensaje.sizeNombre;
	uint32_t tamanioHeapYMensaje = sizeof(heapDinamico)+tamanioMensaje;
	while(particionesOcupadas())
	{
		bool busqueda = buscarParticionLibre(tamanioHeapYMensaje);
		if(busqueda)
		{
			almacenarMensajeNew(mensaje); //uno por cada mensaje o podre inferir cual es?
			break;
		}
		else
			eliminarParticion();
	}
	compactarMemoria();
	almacenarMensajeNew(mensaje);

}

bool particionesOcupadas(){

	heapDinamico heap;

	int recorrido = 0;
	while(recorrido<TAMANO_MEMORIA){

		memcpy(&heap.libre,principioMemoria+recorrido,sizeof(heap.libre));
		if(heap.libre == 0)
			return true;
		recorrido+=sizeof(heap.libre);
		memcpy(&heap.tamanio,principioMemoria+recorrido,sizeof(heap.tamanio));
		recorrido+=sizeof(heap.tamanio)+heap.tamanio;

	}
	return false;
}

void actualizarBusquedasFallidas(int* busquedasFallidas){
	busquedasFallidas++;
}

void almacenarMensajeNew(new_pokemon_memoria mensaje){
	uint32_t tamanioMensaje = sizeof(uint32_t)*4 + mensaje.sizeNombre;
	void* mensajeSerializado = serializarMensajeNew(mensaje);
	void* particion;
	uint32_t tamanioHeapYMensaje = sizeof(heapDinamico)+tamanioMensaje;

	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE,"FF"))
		particion = buscarPrimerParticionLibre(tamanioMensaje);
	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE,"BF"))
		particion = buscarMejorParticionLibre(tamanioMensaje);

	uint32_t tamanioParticionAntigua;
	int desplazamiento = sizeof(uint32_t);
	memcpy(&tamanioParticionAntigua,particion+desplazamiento,sizeof(uint32_t));

	heapDinamico heapParticion;
	heapParticion.libre = 0;
	heapParticion.tamanio = tamanioMensaje;
	int bytesEscritos = 0;

	memcpy(particion+bytesEscritos,&heapParticion.libre,sizeof(heapParticion.libre));
	bytesEscritos+=sizeof(heapParticion.libre);
	memcpy(particion+bytesEscritos,&heapParticion.tamanio,sizeof(heapParticion.tamanio));
	bytesEscritos+=sizeof(heapParticion.tamanio);
	memcpy(particion+bytesEscritos,mensajeSerializado,tamanioMensaje);
	bytesEscritos+=tamanioMensaje;
	free(mensajeSerializado);
	queue_push(colaMensajesMemoria,particion);
	void* particionContigua = particion+bytesEscritos;
	if(tamanioParticionAntigua != tamanioMensaje)
		agregarHeapAlFinalDeParticion(particionContigua,tamanioParticionAntigua,tamanioMensaje); //nose si poner esta funcion

}

void eliminarParticion(){
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"FIFO"))
	{
		void* particion = queue_pop(colaMensajesMemoria);
		uint32_t libre = 1;
		memcpy(particion,&libre,sizeof(libre));
	}
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"LRU"))
	{
		int posicionUltimoElemeneto= list_size(ultimasReferencias)-1;
		void* particion = list_remove(ultimasReferencias,posicionUltimoElemeneto);
		uint32_t libre = 1;
		memcpy(particion,&libre,sizeof(libre));
	}
}

void inicializarMemoria(){
	principioMemoria = malloc(TAMANO_MEMORIA);
	heapDinamico heapInicial;
	heapInicial.libre = 1;
	heapInicial.tamanio = TAMANO_MEMORIA-sizeof(heapInicial.libre)-sizeof(heapInicial.tamanio);
	int recorrido = 0;
	memcpy(principioMemoria+recorrido,&heapInicial.libre,sizeof(heapInicial.libre));
	recorrido+=sizeof(heapInicial.libre);
	memcpy(principioMemoria+recorrido,&heapInicial.tamanio,sizeof(heapInicial.tamanio));
	recorrido+=sizeof(heapInicial.tamanio);
	//return principioMemoria;
}

void compactarMemoria(){

}

bool buscarParticionLibre(uint32_t tamanioMensaje){
	int recorrido = 0;
	int recorridoAuxiliar = 0;
	uint32_t tamanioHeapYMensaje = tamanioMensaje + sizeof(heapDinamico);
	while(recorrido<TAMANO_MEMORIA)
	{
		heapDinamico heap;
		memcpy(&heap.libre,principioMemoria+recorrido,sizeof(heap.libre));
		recorrido+=sizeof(heap.libre);
		memcpy(&heap.tamanio,principioMemoria+recorrido,sizeof(heap.tamanio));
		recorrido+=sizeof(heap.tamanio);
		if ((heap.libre == 1) && (tamanioMensaje==heap.tamanio))
			return true;
		if ((heap.libre == 1) && (tamanioHeapYMensaje<=heap.tamanio))
			return true;
		recorrido+=heap.tamanio;
		recorridoAuxiliar = recorrido;
	}
	return false;
}

void* buscarPrimerParticionLibre(uint32_t tamanioMensaje){
	int recorrido = 0;
	int recorridoAuxiliar = 0;
	uint32_t tamanioHeapYMensaje = tamanioMensaje + sizeof(heapDinamico);
	while(recorrido<TAMANO_MEMORIA)
	{
		heapDinamico heap;
		memcpy(&heap.libre,principioMemoria+recorrido,sizeof(heap.libre));
		recorrido+=sizeof(heap.libre);
		memcpy(&heap.tamanio,principioMemoria+recorrido,sizeof(heap.tamanio));
		recorrido+=sizeof(heap.tamanio);
		if ((heap.libre == 1) && (tamanioMensaje==heap.tamanio))
		{
			void* particion = principioMemoria + recorridoAuxiliar;
			return particion;
		}
		if ((heap.libre == 1) && (tamanioHeapYMensaje<=heap.tamanio))
		{
			void* particion = principioMemoria + recorridoAuxiliar;
			return particion;
		}
		recorrido+=heap.tamanio;
		recorridoAuxiliar = recorrido;
	}
	return NULL;
}

void* buscarMejorParticionLibre(uint32_t tamanioMensaje){
	int recorrido = 0;
	int recorridoAuxiliar = 0;
	t_list* particiones = list_create();
	uint32_t tamanioHeapYMensaje = tamanioMensaje+sizeof(heapDinamico);

	while(recorrido<TAMANO_MEMORIA)
	{
		heapDinamico heap;
		memcpy(&heap.libre,principioMemoria+recorrido,sizeof(heap.libre));
		recorrido+=sizeof(heap.libre);
		memcpy(&heap.tamanio,principioMemoria+recorrido,sizeof(heap.tamanio));
		recorrido+=sizeof(heap.tamanio);

		if ((heap.libre == 1)&&(tamanioMensaje==heap.tamanio))
		{
			void* particionMasPequenia = principioMemoria + recorridoAuxiliar;
			return particionMasPequenia;
		}

		if ((heap.libre == 1)&&(tamanioHeapYMensaje<=heap.tamanio))
		{
			particionLibre particion;
			particion.posicionParticion = principioMemoria+recorridoAuxiliar;
			particion.tamanio = heap.tamanio;
			list_add(particiones,&particion);
		}
		recorrido+=heap.tamanio;
		recorridoAuxiliar = recorrido;

	}

	bool comparadorParticionesLibres(particionLibre* particion1,particionLibre* particion2){
		return (particion1->tamanio)<(particion2->tamanio);
	}
	list_sort(particiones, (void*) comparadorParticionesLibres);

	particionLibre* mejorParticion = list_get(particiones,0);
	list_destroy(particiones);
	return (mejorParticion->posicionParticion);
}

void agregarHeapAlFinalDeParticion(void* particionNueva,uint32_t tamanioParticionAntigua,uint32_t tamanioMensaje){
	heapDinamico heapParticionContigua;
	heapParticionContigua.libre = 1;
	heapParticionContigua.tamanio = tamanioParticionAntigua-tamanioMensaje-sizeof(heapDinamico);

	int bytesEscritos = 0;
	memcpy(particionNueva+bytesEscritos,&heapParticionContigua.libre,sizeof(heapParticionContigua.libre));
	bytesEscritos+=sizeof(heapParticionContigua.libre);
	memcpy(particionNueva+bytesEscritos,&heapParticionContigua.tamanio,sizeof(heapParticionContigua.tamanio));
	bytesEscritos+=sizeof(heapParticionContigua.tamanio);


}












