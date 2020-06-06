
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

void cachearMensaje(int mensaje){ //no es un int pero pongo algo para que funque y creo que tambien iria memoria como parametro
	void* memoria = malloc(1024); //aca no seria el malloc
	int busquedasFallidas = 0;

	int frecuenciaCompactacion; //Cantidad de búsquedas fallidas previa compactación
	bool busqueda = buscarParticionLibre(memoria);

	while(1)
	{
		busqueda = buscarParticionLibre(memoria);
		if(busqueda)
		{
			almacenarMensaje(mensaje,memoria); //uno por cada mensaje o podre inferir cual es?
			break;
		}
		else
		{
			if(busquedasFallidas<frecuenciaCompactacion)
			{
				eliminarParticion(memoria);
				actualizarBusquedasFallidas(&busquedasFallidas);
			}
			else
			{
				compactarMemoria(memoria);
			}
		}
	}
}

void actualizarBusquedasFallidas(int* busquedasFallidas){
	busquedasFallidas++;
}

bool buscarParticionLibre(void* memoria){
	return true;
}

void almacenarMensaje(int mensaje,void* memoriamemoria){

}

void eliminarParticion(void* memoriamemoria){

}

void compactarMemoria(void* memoriamemoria){

}

