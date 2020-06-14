

#ifndef PARTICIONESDINAMICAS_H_
#define PARTICIONESDINAMICAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include "ProtocoloDeMensajes.h"

#include "tiposMensajesEnMemoria.h"
#include "Configuracion.h"

typedef struct {
	void* posicionParticion;
	bool libre; //1 si esta libre, 0 si no.
	int tamanio;
	int tamanioMensaje;
	uint32_t idMensaje;
	uint32_t idCorrelativo;
	uint32_t cola;
	}particion_dinamica_memoria;

t_list* particionesEnMemoria;

void* principioMemoria;


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



t_queue* colaMensajesMemoria;
t_list* ultimasReferencias; // la ultima particion usada se agrega al principio de la lista, la menos usada esta al final.

void inicializarMemoria();

bool buscarParticionLibre(uint32_t tamanioMensaje);
void almacenarMensaje(void* mensaje,id_cola id);
void eliminarParticion();
void actualizarBusquedasFallidas(int* busquedasFallidas);
//TODO compactarMemoria
void compactarMemoria();
particion_dinamica_memoria* buscarPrimerParticionLibre(uint32_t tamanioMensaje);
particion_dinamica_memoria* buscarMejorParticionLibre(uint32_t tamanioMensaje);


void agregarParticionContigua(particion_dinamica_memoria* particion,uint32_t tamanioParticionAntigua);

void ejecutarCicloNormal(void* mensaje,id_cola id);
void ejecutarCicloAlternativo(void* mensaje,id_cola id);
bool particionesOcupadas();
void cachearMensaje(void* mensaje,id_cola id);

particion_dinamica_memoria* cargarDatosParticion(particion_dinamica_memoria* particion,void* mensaje,id_cola id);

#endif /* PARTICIONESDINAMICAS_H_ */
