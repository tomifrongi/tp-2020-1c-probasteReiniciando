
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

//TODO crear estructura suscriptor y sus funciones
//TODO que pasa si el mensaje es mas grande que la memoria
//TODO antes de tomar un mensaje en memoria deberia buscar si esta en memoria (talvez lo borraron)
//TODO cuando elimino un mensaje solo cambio el bit de libre, quiza deberia borrar toodo (podria buscar un mensaje, encontrarlo pero esta marcado como libre. Nose si puede darse esta situacion)
//TODO sacar barra cero o caracter nulo. No se guarda en memoria

void borrar_suscriptor(void* algo){}

//int main(){
//	ejecutarPruebaCompactacionMio();
//}

void ejecutarPruebaBaseBroker(){

	printf("Corriendo particiones dinamicas\n");
	TAMANO_MEMORIA = 4096;
	TAMANO_MINIMO_PARTICION = 4;
	ALGORITMO_REEMPLAZO = "FIFO";
	ALGORITMO_PARTICION_LIBRE = "FF";
	FRECUENCIA_COMPACTACION = 3;

	inicializarMemoria();

////	./gameboy BROKER GET_POKEMON Pikachu
	get_pokemon_enviar menGet1;
	menGet1.nombrePokemon = "Pikach";
	menGet1.sizeNombre = 7;
	menGet1.id_mensaje = 1;
	cachearMensaje(&menGet1,GET);
////	./gameboy BROKER GET_POKEMON Charmander
	get_pokemon_enviar menGet2;
	menGet2.nombrePokemon = "Charmande";
	menGet2.sizeNombre = 10;
	menGet2.id_mensaje = 2;
	cachearMensaje(&menGet2,GET);

//	./gameboy BROKER CATCH_POKEMON Pikachu 9 3 #19b
	catch_pokemon_enviar menCatch1;
	menCatch1.nombrePokemon = "Pikach";
	menCatch1.sizeNombre = 7;
	menCatch1.posicionEjeX = 9;
	menCatch1.posicionEjeY = 3;
	menCatch1.id_mensaje = 3;
	cachearMensaje(&menCatch1,CATCH);
//	./gameboy BROKER CATCH_POKEMON Squirtle 9 3 #20b
	catch_pokemon_enviar menCatch2;
	menCatch2.nombrePokemon = "Squirtl";
	menCatch2.sizeNombre = 8;
	menCatch2.posicionEjeX = 9;
	menCatch2.posicionEjeY = 3;
	menCatch2.id_mensaje = 4;
	cachearMensaje(&menCatch2,CATCH);
//	./gameboy BROKER CAUGHT_POKEMON 10 OK
	caught_pokemon_enviar menCaught1;
	menCaught1.pokemonAtrapado = 1;
	menCaught1.idCorrelativo = 10;
	menCaught1.id_mensaje = 5;
	cachearMensaje(&menCaught1,CAUGHT);
//	./gameboy BROKER CAUGHT_POKEMON 11 FAIL
	caught_pokemon_enviar menCaught2;
	menCaught2.pokemonAtrapado = 0;
	menCaught2.idCorrelativo = 11;
	menCaught2.id_mensaje = 6;
	cachearMensaje(&menCaught2,CAUGHT);
//	./gameboy BROKER CATCH_POKEMON Bulbasaur 1 7 #21
	catch_pokemon_enviar menCatch3;
	menCatch3.nombrePokemon = "Bulbasau";
	menCatch3.sizeNombre = 9;
	menCatch3.posicionEjeX = 1;
	menCatch3.posicionEjeY = 7;
	menCatch3.id_mensaje = 7;
	cachearMensaje(&menCatch3,CATCH);
//	./gameboy BROKER CATCH_POKEMON Charmander 1 7 #22
	catch_pokemon_enviar menCatch4;
	menCatch4.nombrePokemon = "Charmande";
	menCatch4.sizeNombre = 10;
	menCatch4.posicionEjeX = 1;
	menCatch4.posicionEjeY = 7;
	menCatch4.id_mensaje = 8;
	cachearMensaje(&menCatch4,CATCH);

//	./gameboy BROKER GET_POKEMON Pichu #9
	get_pokemon_enviar menGet3;
	menGet3.nombrePokemon = "Pich";
	menGet3.sizeNombre = 5;
	menGet3.id_mensaje = 9;
	cachearMensaje(&menGet3,GET);
//	./gameboy BROKER GET_POKEMON Raichu #10
	get_pokemon_enviar menGet4;
	menGet4.nombrePokemon = "Raich";
	menGet4.sizeNombre = 6;
	menGet4.id_mensaje = 10;
	cachearMensaje(&menGet4,GET);

	int i = 1;
	while(i<=list_size(particionesEnMemoria)){
		particion_dinamica_memoria* part = list_get(particionesEnMemoria,i-1);
		int idPart = part->idMensaje;
		int posPart =part->posicionParticion;
		int tamanioPart = part->tamanio;
		printf("id mensaje particion: %d \n",idPart);
		printf("posicion particion: %d \n",posPart);
		printf("tamanio particion: %d \n",tamanioPart);
		void* posicionPart = principioMemoria + posPart;
		uint32_t indice;
		memcpy(&indice,posicionPart,sizeof(indice));
		printf("indice: %d \n",indice);
		printf("------------------ \n");
		i++;
	}

	printf("Finalizado particiones\n");
}

void ejecutarPruebaBaseBroker2(){

	printf("Corriendo particiones dinamicas\n");
	TAMANO_MEMORIA = 4096;
	TAMANO_MINIMO_PARTICION = 16;
	ALGORITMO_REEMPLAZO = "FIFO";
	ALGORITMO_PARTICION_LIBRE = "FF";
	FRECUENCIA_COMPACTACION = 3;

	inicializarMemoria();

//	./gameboy BROKER CATCH_POKEMON Pikachu 9 3 #19b
	catch_pokemon_enviar menCatch1;
	menCatch1.nombrePokemon = "Pikach";
	menCatch1.sizeNombre = 7;
	menCatch1.posicionEjeX = 9;
	menCatch1.posicionEjeY = 3;
	menCatch1.id_mensaje = 3;
	cachearMensaje(&menCatch1,CATCH);
//	./gameboy BROKER CATCH_POKEMON Squirtle 9 3 #20b
	catch_pokemon_enviar menCatch2;
	menCatch2.nombrePokemon = "Squirtl";
	menCatch2.sizeNombre = 8;
	menCatch2.posicionEjeX = 9;
	menCatch2.posicionEjeY = 3;
	menCatch2.id_mensaje = 4;
	cachearMensaje(&menCatch2,CATCH);
//	./gameboy BROKER CAUGHT_POKEMON 10 OK
	caught_pokemon_enviar menCaught1;
	menCaught1.pokemonAtrapado = 1;
	menCaught1.idCorrelativo = 10;
	menCaught1.id_mensaje = 5;
	cachearMensaje(&menCaught1,CAUGHT);
//	./gameboy BROKER CAUGHT_POKEMON 11 FAIL
	caught_pokemon_enviar menCaught2;
	menCaught2.pokemonAtrapado = 0;
	menCaught2.idCorrelativo = 11;
	menCaught2.id_mensaje = 6;
	cachearMensaje(&menCaught2,CAUGHT);
//	./gameboy BROKER CATCH_POKEMON Bulbasaur 1 7 #21
	catch_pokemon_enviar menCatch3;
	menCatch3.nombrePokemon = "Bulbasau";
	menCatch3.sizeNombre = 9;
	menCatch3.posicionEjeX = 1;
	menCatch3.posicionEjeY = 7;
	menCatch3.id_mensaje = 7;
	cachearMensaje(&menCatch3,CATCH);
//	./gameboy BROKER CATCH_POKEMON Charmander 1 7 #22
	catch_pokemon_enviar menCatch4;
	menCatch4.nombrePokemon = "Charmande";
	menCatch4.sizeNombre = 10;
	menCatch4.posicionEjeX = 1;
	menCatch4.posicionEjeY = 7;
	menCatch4.id_mensaje = 8;
	cachearMensaje(&menCatch4,CATCH);

//	./gameboy BROKER GET_POKEMON Pichu #9
	get_pokemon_enviar menGet3;
	menGet3.nombrePokemon = "Pich";
	menGet3.sizeNombre = 5;
	menGet3.id_mensaje = 9;
	cachearMensaje(&menGet3,GET);
//	./gameboy BROKER GET_POKEMON Raichu #10
	get_pokemon_enviar menGet4;
	menGet4.nombrePokemon = "Raich";
	menGet4.sizeNombre = 6;
	menGet4.id_mensaje = 10;
	cachearMensaje(&menGet4,GET);

	int i = 1;
	while(i<=list_size(particionesEnMemoria)){
		particion_dinamica_memoria* part = list_get(particionesEnMemoria,i-1);
		int idPart = part->idMensaje;
		int posPart =part->posicionParticion;
		int tamanioPart = part->tamanio;
		printf("id mensaje particion: %d \n",idPart);
		printf("posicion particion: %d \n",posPart);
		printf("tamanio particion: %d \n",tamanioPart);
		void* posicionPart = principioMemoria + posPart;
		uint32_t indice;
		memcpy(&indice,posicionPart,sizeof(indice));
		printf("indice: %d \n",indice);
		printf("------------------ \n");
		i++;
	}

	printf("Finalizado particiones\n");
}

void ejecutarPruebaConsolidacionBroker(){

	printf("Corriendo particiones dinamicas\n");
	TAMANO_MEMORIA = 64;
	TAMANO_MINIMO_PARTICION = 4;
	ALGORITMO_REEMPLAZO = "FIFO";
	ALGORITMO_PARTICION_LIBRE = "FF";
	FRECUENCIA_COMPACTACION = 10;

	inicializarMemoria();

//	./gameboy BROKER CAUGHT_POKEMON 1 OK //TAMANIO 4
	caught_pokemon_enviar menCaught1;
	menCaught1.pokemonAtrapado = 1;
	menCaught1.idCorrelativo = 11;
	menCaught1.id_mensaje = 1;
	cachearMensaje(&menCaught1,CAUGHT);

//	./gameboy BROKER CAUGHT_POKEMON 1 FAIL //TAMANIO 4
	caught_pokemon_enviar menCaught2;
	menCaught2.pokemonAtrapado = 0;
	menCaught2.idCorrelativo = 12;
	menCaught2.id_mensaje = 2;
	cachearMensaje(&menCaught2,CAUGHT);

//	./gameboy BROKER CATCH_POKEMON Pikachu 2 3  //TAMANIO 19
	catch_pokemon_enviar menCatch1;
	menCatch1.nombrePokemon = "Pikach";
	menCatch1.sizeNombre = 7;
	menCatch1.posicionEjeX = 2;
	menCatch1.posicionEjeY = 3;
	menCatch1.id_mensaje = 3;
	cachearMensaje(&menCatch1,CATCH);

//	./gameboy BROKER CATCH_POKEMON Squirtle 5 2  //TAMANIO 20
	catch_pokemon_enviar menCatch2;
	menCatch2.nombrePokemon = "Squirtl";
	menCatch2.sizeNombre = 8;
	menCatch2.posicionEjeX = 5;
	menCatch2.posicionEjeY = 2;
	menCatch2.id_mensaje = 4;
	cachearMensaje(&menCatch2,CATCH);

//	./gameboy BROKER NEW_POKEMON Onyx 4 5 1 //TAMANIO 20
	new_pokemon_enviar menNew1;
	menNew1.nombrePokemon = "Ony";
	menNew1.sizeNombre = 4;
	menNew1.posicionEjeX = 4;
	menNew1.posicionEjeY = 5;
	menNew1.cantidad = 1;
	menNew1.id_mensaje = 5;
	cachearMensaje(&menNew1,NEW);

	ordenarParticionesPorPosicion();

//	./gameboy SUSCRIPTOR CAUGHT_POKEMON 10
//	//aca no se agrega nada porque es FIFO

//	./gameboy BROKER NEW_POKEMON Charmander 4 5 2  //TAMANIO 26
	new_pokemon_enviar menNew2;
	menNew2.nombrePokemon = "Charmande";
	menNew2.sizeNombre = 10;
	menNew2.posicionEjeX = 4;
	menNew2.posicionEjeY = 5;
	menNew2.cantidad = 1;
	menNew2.id_mensaje = 6;
	cachearMensaje(&menNew2,NEW);

	int i = 1;
	while(i<=list_size(particionesEnMemoria)){
		particion_dinamica_memoria* part = list_get(particionesEnMemoria,i-1);
		int idPart = part->idMensaje;
		int posPart =part->posicionParticion;
		int tamanioPart = part->tamanio;
		int librePart = part->libre;
		if(librePart)
			printf("Particion Libre\n");
		else
			printf("Particion Ocupada\n");
		printf("id mensaje particion: %d \n",idPart);
		printf("posicion particion: %d \n",posPart);
		printf("tamanio particion: %d \n",tamanioPart);
		void* posicionPart = principioMemoria + posPart;
		uint32_t indice;
		memcpy(&indice,posicionPart,sizeof(indice));
		printf("indice: %d \n",indice);
		printf("------------------ \n");
		i++;
	}

}

void ejecutarPruebaConsolidacionBroker2(){

	printf("Corriendo particiones dinamicas\n");
	TAMANO_MEMORIA = 64;
	TAMANO_MINIMO_PARTICION = 4;
	ALGORITMO_REEMPLAZO = "LRU";
	ALGORITMO_PARTICION_LIBRE = "FF";
	FRECUENCIA_COMPACTACION = 10;

	inicializarMemoria();

//	./gameboy BROKER CAUGHT_POKEMON 1 OK //TAMANIO 4
	caught_pokemon_enviar menCaught1;
	menCaught1.pokemonAtrapado = 1;
	menCaught1.idCorrelativo = 11;
	menCaught1.id_mensaje = 1;
	cachearMensaje(&menCaught1,CAUGHT);

//	./gameboy BROKER CAUGHT_POKEMON 1 FAIL //TAMANIO 4
	caught_pokemon_enviar menCaught2;
	menCaught2.pokemonAtrapado = 0;
	menCaught2.idCorrelativo = 12;
	menCaught2.id_mensaje = 2;
	cachearMensaje(&menCaught2,CAUGHT);

//	./gameboy BROKER CATCH_POKEMON Pikachu 2 3  //TAMANIO 19
	catch_pokemon_enviar menCatch1;
	menCatch1.nombrePokemon = "Pikach";
	menCatch1.sizeNombre = 7;
	menCatch1.posicionEjeX = 2;
	menCatch1.posicionEjeY = 3;
	menCatch1.id_mensaje = 3;
	cachearMensaje(&menCatch1,CATCH);

//	./gameboy BROKER CATCH_POKEMON Squirtle 5 2  //TAMANIO 20
	catch_pokemon_enviar menCatch2;
	menCatch2.nombrePokemon = "Squirtl";
	menCatch2.sizeNombre = 8;
	menCatch2.posicionEjeX = 5;
	menCatch2.posicionEjeY = 2;
	menCatch2.id_mensaje = 4;
	cachearMensaje(&menCatch2,CATCH);

//	./gameboy SUSCRIPTOR CAUGHT_POKEMON 10

	void sumarLRUCaught(void* particion){
		particion_dinamica_memoria* particionCasteada = particion;
		if(particionCasteada->cola == CAUGHT){
			CONTADORLRU++;
			particionCasteada->contadorLRU = CONTADORLRU;
		}
	}
	list_iterate(particionesEnMemoria,sumarLRUCaught);

//	./gameboy BROKER NEW_POKEMON Onyx 4 5 1 //TAMANIO 20
	new_pokemon_enviar menNew1;
	menNew1.nombrePokemon = "Ony";
	menNew1.sizeNombre = 4;
	menNew1.posicionEjeX = 4;
	menNew1.posicionEjeY = 5;
	menNew1.cantidad = 1;
	menNew1.id_mensaje = 5;
	cachearMensaje(&menNew1,NEW);

	ordenarParticionesPorPosicion();

//	./gameboy BROKER NEW_POKEMON Charmander 4 5 2  //TAMANIO 26
	new_pokemon_enviar menNew2;
	menNew2.nombrePokemon = "Charmande";
	menNew2.sizeNombre = 10;
	menNew2.posicionEjeX = 4;
	menNew2.posicionEjeY = 5;
	menNew2.cantidad = 1;
	menNew2.id_mensaje = 6;
	cachearMensaje(&menNew2,NEW);

	int i = 1;
	while(i<=list_size(particionesEnMemoria)){
		particion_dinamica_memoria* part = list_get(particionesEnMemoria,i-1);
		int idPart = part->idMensaje;
		int posPart =part->posicionParticion;
		int tamanioPart = part->tamanio;
		int librePart = part->libre;
		id_cola idColaPart = part->cola;
		int contadorLRUpart = part->contadorLRU;
		if(librePart)
			printf("Particion Libre\n");
		else
			printf("Particion Ocupada\n");
		printf("id cola: %d \n",idColaPart);
		printf("contador LRU: %d \n",contadorLRUpart);
		printf("id mensaje particion: %d \n",idPart);
		printf("posicion particion: %d \n",posPart);
		printf("tamanio particion: %d \n",tamanioPart);
		void* posicionPart = principioMemoria + posPart;
		uint32_t indice;
		memcpy(&indice,posicionPart,sizeof(indice));
		printf("indice: %d \n",indice);
		printf("------------------ \n");
		i++;
	}

}

void ejecutarPruebaCompactacionMio(){
	printf("Corriendo particiones dinamicas\n");
	TAMANO_MEMORIA = 64;
	TAMANO_MINIMO_PARTICION = 4;
	ALGORITMO_REEMPLAZO = "LRU";
	ALGORITMO_PARTICION_LIBRE = "FF";
	FRECUENCIA_COMPACTACION = 10;
	inicializarMemoria();

	//	./gameboy BROKER CAUGHT_POKEMON 1 OK //TAMANIO 4
		caught_pokemon_enviar menCaught1;
		menCaught1.pokemonAtrapado = 1;
		menCaught1.idCorrelativo = 11;
		menCaught1.id_mensaje = 1;
		cachearMensaje(&menCaught1,CAUGHT);

	//	./gameboy BROKER CAUGHT_POKEMON 1 FAIL //TAMANIO 4
		caught_pokemon_enviar menCaught2;
		menCaught2.pokemonAtrapado = 0;
		menCaught2.idCorrelativo = 12;
		menCaught2.id_mensaje = 2;
		cachearMensaje(&menCaught2,CAUGHT);

	//	./gameboy BROKER CATCH_POKEMON Pikachu 2 3  //TAMANIO 19
		catch_pokemon_enviar menCatch1;
		menCatch1.nombrePokemon = "Pikach";
		menCatch1.sizeNombre = 7;
		menCatch1.posicionEjeX = 2;
		menCatch1.posicionEjeY = 3;
		menCatch1.id_mensaje = 3;
		cachearMensaje(&menCatch1,CATCH);

	//	./gameboy BROKER CATCH_POKEMON Squirtle 5 2  //TAMANIO 20
		catch_pokemon_enviar menCatch2;
		menCatch2.nombrePokemon = "Squirtl";
		menCatch2.sizeNombre = 8;
		menCatch2.posicionEjeX = 5;
		menCatch2.posicionEjeY = 2;
		menCatch2.id_mensaje = 4;
		cachearMensaje(&menCatch2,CATCH);

		eliminarParticion();
		eliminarParticion();
		consolidarMemoria();
		ordenarParticionesPorPosicion();
		compactarMemoria();

		int i = 1;
		while(i<=list_size(particionesEnMemoria)){
			particion_dinamica_memoria* part = list_get(particionesEnMemoria,i-1);
			int idPart = part->idMensaje;
			int posPart =part->posicionParticion;
			int tamanioPart = part->tamanio;
			int librePart = part->libre;
			id_cola idColaPart = part->cola;
			int contadorLRUpart = part->contadorLRU;
			if(librePart)
				printf("Particion Libre\n");
			else
				printf("Particion Ocupada\n");
			printf("id cola: %d \n",idColaPart);
			printf("contador LRU: %d \n",contadorLRUpart);
			printf("id mensaje particion: %d \n",idPart);
			printf("posicion particion: %d \n",posPart);
			printf("tamanio particion: %d \n",tamanioPart);
			void* posicionPart = principioMemoria + posPart;
			uint32_t indice;
			memcpy(&indice,posicionPart,sizeof(indice));
			printf("indice: %d \n",indice);
			printf("------------------ \n");
			i++;
		}


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
	particionInicial.contadorLRU = -1;
	particion_dinamica_memoria* particionInicialCreada = crear_particion_dinamica_memoria(particionInicial);

	particionesEnMemoria = list_create();
	list_add(particionesEnMemoria,particionInicialCreada);

	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"FIFO"))
		colaMensajesMemoria = queue_create();
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"LRU"))
		CONTADORLRU = 0;
}

void cachearMensaje(void* mensaje,id_cola id){
	sacarBarraCero(mensaje,id);
	if(obtenerTamanioMensaje(mensaje,id)<=TAMANO_MEMORIA)
	{
		if(FRECUENCIA_COMPACTACION == -1)
			ejecutarCicloAlternativo(mensaje,id);
		else
			ejecutarCicloNormal(mensaje,id);
	}
	else
		log_info(logger,"MENSAJE NO CACHEADO DEBIDO A QUE LA LONGITUD SUPERA EL TAMAÑO DE LA MEMORIA");
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
				busquedasFallidas++;
			}
			else{
				compactarMemoria();
				busquedasFallidas = 0;
			}
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

void sacarBarraCero(void* mensaje,id_cola id){
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

	log_info(logger,"POSICION NUEVO MENSAJE: %d",particion->posicionParticion);

	memcpy(principioMemoria+posicionParticion,mensajeSerializado,tamanioMensaje);
	free(mensajeSerializado);

}


t_list* sacarParticionesLibres(){
	bool particionLibre(void* particion){
		particion_dinamica_memoria* particionCasteada = particion;
		return !(particionCasteada->libre);
	}
	 return list_filter(particionesEnMemoria,particionLibre);

}

t_list* sacarParticionesOcupadas(){
	bool particionLibre(void* particion){
		particion_dinamica_memoria* particionCasteada = particion;
		return (particionCasteada->libre);
	}
	 return list_filter(particionesEnMemoria,particionLibre);

}

void eliminarParticion(){
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"FIFO"))
	{
		int* idMensaje = queue_pop(colaMensajesMemoria);
		int idMensajeAuxiliar = *idMensaje;
		borrar_elemento_colaMensajesMemoria(idMensaje);

		void cambiarALibre(void* particion){
			particion_dinamica_memoria* particionCasteada = particion;
			if((particionCasteada->idMensaje) == idMensajeAuxiliar){
				particionCasteada->libre = true;
				log_info(logger,"POSICION VICTIMA: %d",particionCasteada->posicionParticion);

			}


		}
		list_iterate(particionesEnMemoria, cambiarALibre);
	}
	if(string_equals_ignore_case(ALGORITMO_REEMPLAZO,"LRU"))
	{

		t_list* particionesOcupadas = sacarParticionesLibres();

		bool comparadorParticionesLibresPorLRU(void* particion1,void* particion2){
			particion_dinamica_memoria* particion1Casteada = particion1;
			particion_dinamica_memoria* particion2Casteada = particion2;
			return (particion1Casteada->contadorLRU)<(particion2Casteada->contadorLRU);
		}
		list_sort(particionesOcupadas,comparadorParticionesLibresPorLRU);

		particion_dinamica_memoria* particionMenosUsada = list_get(particionesOcupadas,0);
		int idMensaje = particionMenosUsada->idMensaje;
		list_destroy(particionesOcupadas);
		void cambiarALibre(void* particion){
			particion_dinamica_memoria* particionCasteada = particion;
			if((particionCasteada->idMensaje) == idMensaje){
				particionCasteada->libre = true;
				log_info(logger,"POSICION VICTIMA: %d",particionCasteada->posicionParticion);
			}
		}
		list_iterate(particionesEnMemoria, cambiarALibre);
	}

}



void compactarMemoria(){
	ordenarParticionesPorPosicion();
	int sizeLista = list_size(particionesEnMemoria);
	int index = 0;
	int indexAdyacente = index+1;
	while(indexAdyacente<sizeLista){
		particion_dinamica_memoria* particionLibre = list_get(particionesEnMemoria,index);
		particion_dinamica_memoria* particionOcupada = list_get(particionesEnMemoria,indexAdyacente);

		if(particionLibre->libre){
			int posicion = particionLibre->posicionParticion;
			particionLibre = removerPorPosicion(posicion);
			posicion = particionOcupada->posicionParticion;
			particionOcupada = removerPorPosicion(posicion);
			//---------
			//ACA ESTA EL PROBLEMA
			particionOcupada->posicionParticion = particionLibre->posicionParticion; //la particion ocupada pasa a ser la no adyacente
			particionLibre->posicionParticion = particionOcupada->posicionParticion + particionOcupada->tamanio; //la particion libre pasa a ser la adyacente
			//---------
			memcpy(principioMemoria+(particionOcupada->posicionParticion),principioMemoria+(particionLibre->posicionParticion),particionOcupada->tamanioMensaje);

			list_add(particionesEnMemoria,particionLibre);
			list_add(particionesEnMemoria,particionOcupada);

			consolidarMemoria();
			ordenarParticionesPorPosicion();
			sizeLista = list_size(particionesEnMemoria);
		}
		index++;
		indexAdyacente++;
	}

	ordenarParticionesPorPosicion();
	void imprimirInfo(void* particion){
		particion_dinamica_memoria* particionCasteada = particion;
		log_info(logger,"POSICION: %d LIBRE: %d TAMANIO: %d",particionCasteada->posicionParticion,particionCasteada->libre,particionCasteada->tamanio);
	}
	log_info(logger,"ESTADO MEMORIA");
	list_iterate(particionesEnMemoria,imprimirInfo);



}

particion_dinamica_memoria* removerPorPosicion(int posicion){
	bool compararPorId(void* particion){
		particion_dinamica_memoria* particionCasteada = particion;
		return particionCasteada->posicionParticion == posicion;
	}

	return list_remove_by_condition(particionesEnMemoria,compararPorId);
}

void consolidarMemoria(){
	int sizeLista = list_size(particionesEnMemoria);
	int index = 0;
	int indexAdyacente = index+1;
	ordenarParticionesPorPosicion();
	while(indexAdyacente<sizeLista){
		particion_dinamica_memoria* particion = list_get(particionesEnMemoria,index);
		particion_dinamica_memoria* particionAdyacente = list_get(particionesEnMemoria,indexAdyacente);

		if(particion->libre && particionAdyacente->libre){
			log_info(logger,"POSICION %d y POSICION %d CONSOLIDADA",particion->posicionParticion,particionAdyacente->posicionParticion);
			int posicion = particion->posicionParticion;
			particion = removerPorPosicion(posicion);
			posicion = particionAdyacente->posicionParticion;
			particionAdyacente = removerPorPosicion(posicion);

			particion_dinamica_memoria particionNuevaAuxiliar;
			particionNuevaAuxiliar.libre = 1;
			particionNuevaAuxiliar.posicionParticion = particion->posicionParticion;
			particionNuevaAuxiliar.tamanio = particion->tamanio + particionAdyacente->tamanio;
			particionNuevaAuxiliar.cola = -1;
			particionNuevaAuxiliar.contadorLRU = -1;
			particionNuevaAuxiliar.idCorrelativo = -1;
			particionNuevaAuxiliar.idMensaje = -1;
			particionNuevaAuxiliar.tamanioMensaje = -1;
			particion_dinamica_memoria* particionNueva = crear_particion_dinamica_memoria(particionNuevaAuxiliar);
			list_add(particionesEnMemoria,particionNueva);

			borrar_particion_dinamica_memoria(particion);
			borrar_particion_dinamica_memoria(particionAdyacente);

			ordenarParticionesPorPosicion();
			index--;
			indexAdyacente = index+1;
			sizeLista = list_size(particionesEnMemoria);

		}

		index++;
		indexAdyacente++;
	}


}

void ordenarParticionesPorPosicion(){
	bool comparadorParticionesPorPosicion(void* particion1,void* particion2){
		particion_dinamica_memoria* particion1Casteada = particion1;
		particion_dinamica_memoria* particion2Casteada = particion2;
		return (particion1Casteada->posicionParticion)<(particion2Casteada->posicionParticion);
	}
	list_sort(particionesEnMemoria, comparadorParticionesPorPosicion);
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
	ordenarParticionesPorPosicion();

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
	list_destroy(particionesLibres);
	int posicionMejorParticion = mejorParticionAuxiliar->posicionParticion;
//	borrar_particion_dinamica_memoria(mejorParticionAuxiliar);

	bool particionMismoIdMensaje(void* particion){
		particion_dinamica_memoria* particionCasteada = particion;
		return (particionCasteada->posicionParticion) == posicionMejorParticion;
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
	particionContigua.contadorLRU = -1;
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
		CONTADORLRU++;
		particion->contadorLRU = CONTADORLRU;
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
		CONTADORLRU++;
		particion->contadorLRU = CONTADORLRU;
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
		CONTADORLRU++;
		particion->contadorLRU = CONTADORLRU;
		list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptor);
		list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptor);
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
			particion->tamanio = particion->tamanioMensaje;
		CONTADORLRU++;
		particion->contadorLRU = CONTADORLRU;
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
		CONTADORLRU++;
		particion->contadorLRU = CONTADORLRU;
		list_clean_and_destroy_elements(particion->suscriptoresACK,borrar_suscriptor);
		list_clean_and_destroy_elements(particion->suscriptoresMensajeEnviado,borrar_suscriptor);
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

particion_dinamica_memoria* encontrarParticionDinamicaPorID(int idMensaje){
	bool particionIgualID(void* particion){
		particion_dinamica_memoria* particionCasteada = particion;

		return (particionCasteada->idMensaje == idMensaje) && !(particionCasteada->libre);
	}

	return list_find(particionesEnMemoria,particionIgualID);
}




