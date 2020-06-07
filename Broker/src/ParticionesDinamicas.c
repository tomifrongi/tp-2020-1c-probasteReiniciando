
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

void cachearMensaje(int mensaje){ //no es un int pero pongo algo para que funque y creo que tambien iria memoria como parametro
	int busquedasFallidas = 0;
	bool busqueda = buscarParticionLibre();

	while(1)
	{
		busqueda = buscarParticionLibre();
		if(busqueda)
		{
			almacenarMensaje(mensaje); //uno por cada mensaje o podre inferir cual es?
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
			{
				compactarMemoria();
			}
		}
	}
}

void actualizarBusquedasFallidas(int* busquedasFallidas){
	busquedasFallidas++;
}

bool buscarParticionLibre(){
	return true; //para que compile nomas
}

void almacenarMensajeNew(new_pokemon_memoria mensaje){
	uint32_t tamanioMensaje = sizeof(uint32_t)*4 + mensaje.sizeNombre;
	void* mensajeSerializado = serializarMensajeNew(mensaje);
	void* particion;
	uint32_t tamanioHeapYMensaje = sizeof(heapDinamico)+tamanioMensaje;

	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE,"FF"))
		particion = buscarPrimerParticionLibre(tamanioHeapYMensaje);
	if(string_equals_ignore_case(ALGORITMO_PARTICION_LIBRE,"BF"))
		particion = buscarMejorParticionLibre(tamanioHeapYMensaje);

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
	agregarHeapAlFinalDeParticion(particion,bytesEscritos); //nose si poner esta funcion

}

void eliminarParticion(){

}

void* inicializarMemoria(){
	TAMANO_MEMORIA = config_get_int_value(config,"TAMANO_MEMORIA");
	principioMemoria = malloc(TAMANO_MEMORIA);
	heapDinamico heapInicial;
	heapInicial.libre = 1;
	heapInicial.tamanio = TAMANO_MEMORIA-sizeof(heapInicial.libre)-sizeof(heapInicial.tamanio);
	return principioMemoria;
}

void compactarMemoria(){

}

void* buscarPrimerParticionLibre(uint32_t tamanioHeapYMensaje){
	int recorrido = 0;
	int recorridoAuxiliar;
	while(recorrido<TAMANO_MEMORIA)
	{
		heapDinamico heap;
		memcpy(&heap.libre,principioMemoria+recorrido,sizeof(heap.libre));
		recorrido+=sizeof(heap.libre);
		memcpy(&heap.tamanio,principioMemoria+recorrido,sizeof(heap.tamanio));
		recorrido+=sizeof(heap.tamanio);
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

void* buscarMejorParticionLibre(uint32_t tamanioHeapYMensaje){
	int recorrido = 0;
	int recorridoAuxiliar;
	t_list* particiones = list_create();

	while(recorrido<TAMANO_MEMORIA)
	{
		heapDinamico heap;
		memcpy(&heap.libre,principioMemoria+recorrido,sizeof(heap.libre));
		recorrido+=sizeof(heap.libre);
		memcpy(&heap.tamanio,principioMemoria+recorrido,sizeof(heap.tamanio));
		recorrido+=sizeof(heap.tamanio);
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

	return (mejorParticion->posicionParticion);
}
