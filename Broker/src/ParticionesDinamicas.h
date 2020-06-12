

#ifndef PARTICIONESDINAMICAS_H_
#define PARTICIONESDINAMICAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <commons/string.h>
#include <commons/collections/queue.h>

#include "tiposMensajesEnMemoria.h"
#include "Configuracion.h"


typedef struct {
	uint32_t libre; //1 si esta libre, 0 si no.
	uint32_t tamanio;
}heapDinamico;

typedef struct {
	heapDinamico heap;
	void* datos;
}particionDinamica;

typedef struct {
	void* posicionParticion;
	uint32_t tamanio;
}particionLibre;


void* principioMemoria;
t_queue* colaMensajesMemoria;
t_list* ultimasReferencias; // la ultima particion usada se agrega al principio de la lista, la menos usada esta al final.


bool buscarParticionLibre(uint32_t tamanioHeapYMensaje);
void almacenarMensajeNew(new_pokemon_memoria mensaje);
void eliminarParticion();
void actualizarBusquedasFallidas(int* busquedasFallidas);
//TODO compactarMemoria
void compactarMemoria();
void* buscarPrimerParticionLibre(uint32_t tamanioHeapYMensaje);
void* buscarMejorParticionLibre(uint32_t tamanioHeapYMensaje);


void agregarHeapAlFinalDeParticion(void* particionNueva,uint32_t tamanioParticionAntigua,uint32_t tamanioMensaje);

void ejecutarCicloNormal(new_pokemon_memoria mensaje);
void ejecutarCicloAlternativo(new_pokemon_memoria mensaje);
bool particionesOcupadas();
void cachearMensaje(new_pokemon_memoria mensaje);
void inicializarMemoria();

#endif /* PARTICIONESDINAMICAS_H_ */
