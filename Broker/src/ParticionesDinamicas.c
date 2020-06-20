
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


void borrar_suscriptor(void* algo){}
int main(){

	printf("Corriendo particiones dinamicas\n");
	TAMANO_MEMORIA = 4096;
	TAMANO_MINIMO_PARTICION = 4;
	ALGORITMO_REEMPLAZO = "FIFO";
	ALGORITMO_PARTICION_LIBRE = "FF";
	FRECUENCIA_COMPACTACION = 3;

	inicializarMemoria();

	particion_dinamica_memoria* p =list_get(particionesEnMemoria,0);
	int libre = p->libre;
	int tamanio = p->tamanio;
	printf("libre: %d \n",libre);
	printf("tamanio: %d \n",tamanio);
	bool busqueda = buscarParticionLibre(23);
	printf("busqueda: %d \n",busqueda);
//	new_pokemon_enviar menNew;
//	menNew.sizeNombre = 9;
//	menNew.nombrePokemon = "federico";
//	menNew.cantidad = 4;
//	menNew.posicionEjeX = 25;
//	menNew.posicionEjeY = 56;
//	cachearMensaje(&menNew,NEW);
//	uint32_t sizeNombreAuxiliar;
//	memcpy(&sizeNombreAuxiliar,principioMemoria,sizeof(uint32_t));
//	printf("size primer mensaje: %d \n",sizeNombreAuxiliar);
	printf("------------------ \n");


//	./gameboy BROKER GET_POKEMON Pikachu
	get_pokemon_enviar menGet1;
	menGet1.nombrePokemon = "Pikachu";
	menGet1.sizeNombre = 8; //OCUPA 12
	menGet1.id_mensaje = 1;
	cachearMensaje(&menGet1,GET);
//	./gameboy BROKER GET_POKEMON Charmander
	get_pokemon_enviar menGet2;
	menGet2.nombrePokemon = "Charmander";
	menGet2.sizeNombre = 11; //OCUPA 15
	menGet2.id_mensaje = 2;
	cachearMensaje(&menGet2,GET);

	particion_dinamica_memoria* part1 = list_get(particionesEnMemoria,0);
	particion_dinamica_memoria* part2 = list_get(particionesEnMemoria,1);
	particion_dinamica_memoria* part3 = list_get(particionesEnMemoria,2);

	int idPart1 = part1->idMensaje;
	int posPart1 =part1->posicionParticion;
	printf("idPart1: %d \n",idPart1);
	printf("posPart1: %d \n",posPart1);
	printf("------------------ \n");

	int idPart2 = part2->idMensaje;
	int posPart2 =part2->posicionParticion;
	printf("idPart2: %d \n",idPart2);
	printf("posPart2: %d \n",posPart2);
	printf("------------------ \n");

	int idPart3 = part3->idMensaje;
	int posPart3 =part3->posicionParticion;
	int tamanioPart3 = part3->tamanio;
	printf("idPart3: %d \n",idPart3);
	printf("posPart3: %d \n",posPart3);
	printf("tamanioPart3: %d \n",tamanioPart3);
	printf("------------------ \n");

	printf("Finalizado particiones\n");
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
	particionInicial.contadorLRU = 0;
	particion_dinamica_memoria* particionInicialCreada = crear_particion_dinamica_memoria(particionInicial);

	particionesEnMemoria = list_create();
	list_add(particionesEnMemoria,particionInicialCreada);

	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"FIFO"))
		colaMensajesMemoria = queue_create();
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
				consolidarMemoria();
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
		else {
			eliminarParticion();
			consolidarMemoria();
		}

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

	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"FIFO")){
		int* idMensaje = crear_elemento_colaMensajesMemoria(particion->idMensaje);
		queue_push(colaMensajesMemoria,idMensaje);
	}

	memcpy(principioMemoria+posicionParticion,mensajeSerializado,tamanioMensaje);
	free(mensajeSerializado);

}




void eliminarParticion(){
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"FIFO"))
	{
		int* idMensaje = queue_pop(colaMensajesMemoria);
		int idMensajeAuxiliar = *idMensaje;
		borrar_elemento_colaMensajesMemoria(idMensaje);

		void cambiarALibre(void* particion){
			particion_dinamica_memoria* particionCasteada = particion;
			if((particionCasteada->idMensaje) == idMensajeAuxiliar)
				particionCasteada->libre = true;

		}
		list_iterate(particionesEnMemoria, cambiarALibre);
	}
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"LRU"))
	{
		bool comparadorParticionesLibresPorLRU(void* particion1,void* particion2){
			particion_dinamica_memoria* particion1Casteada = particion1;
			particion_dinamica_memoria* particion2Casteada = particion2;
			return (particion1Casteada->contadorLRU)<(particion2Casteada->contadorLRU);
		}
		list_sort(particionesEnMemoria,comparadorParticionesLibresPorLRU);

		particion_dinamica_memoria* particionLRU = list_get(particionesEnMemoria,0);
		int idMensaje = particionLRU->idMensaje;
		void cambiarALibre(void* particion){
			particion_dinamica_memoria* particionCasteada = particion;
			if((particionCasteada->idMensaje) == idMensaje)
				particionCasteada->libre = true;

		}
		list_iterate(particionesEnMemoria, cambiarALibre);
	}

}



void compactarMemoria(){

}

void consolidarMemoria(){

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
	return list_remove_by_condition(particionesEnMemoria,particionLibre);


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

	particion_dinamica_memoria* mejorParticionAuxiliar = list_remove(particionesLibres,0);
	list_destroy_and_destroy_elements(particionesLibres,borrar_particion_dinamica_memoria);
	int idMejorParticion = mejorParticionAuxiliar->idMensaje;
	borrar_particion_dinamica_memoria(mejorParticionAuxiliar);

	bool particionMismoIdMensaje(void* particion){
		particion_dinamica_memoria* particionCasteada = particion;
		return (particionCasteada->idMensaje) == idMejorParticion;
	}
	particion_dinamica_memoria* mejorParticion = list_remove_by_condition(particionesEnMemoria,particionMismoIdMensaje);
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
	particionContigua.contadorLRU = 0;
	particion_dinamica_memoria* NuevaParticionContigua = crear_particion_dinamica_memoria(particionContigua);

	list_add(particionesEnMemoria,NuevaParticionContigua);



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
		particion->contadorLRU = 1;
		list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptor);
		list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptor);
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
		particion->contadorLRU = 1;
		list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptor);
		list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptor);
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
		particion->contadorLRU = 1;
		list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptor);
		list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptor);
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
		particion->contadorLRU = 1;
		list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptor);
		list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptor);
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
		particion->contadorLRU = 1;
		list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptor);
		list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptor);
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
		particion->contadorLRU = 1;
		list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptor);
		list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptor);
		break;
	}
	}
	return particion;
}

particion_dinamica_memoria* crear_particion_dinamica_memoria(particion_dinamica_memoria particion){
	particion_dinamica_memoria* nuevaParticion = malloc(sizeof(particion_dinamica_memoria));

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

void borrar_particion_dinamica_memoria(particion_dinamica_memoria* particion){

	list_destroy_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptor);
	list_destroy_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptor);

	free(particion);
}

int* crear_elemento_colaMensajesMemoria(int idMensaje){
	int* newIdMensaje = malloc(sizeof(int));
	*newIdMensaje = idMensaje;
	return newIdMensaje;
}

void borrar_elemento_colaMensajesMemoria(int* idMensaje){
	free(idMensaje);
}






