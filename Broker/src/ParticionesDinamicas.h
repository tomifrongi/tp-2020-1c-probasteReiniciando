

#ifndef PARTICIONESDINAMICAS_H_
#define PARTICIONESDINAMICAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tiposMensajesEnMemoria.h"
#include "Configuracion.h"
#include "commons/string.h"

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

//TODO buscarParticionLibre
bool buscarParticionLibre();
//TODO almacenarMensaje
void almacenarMensaje(int mensaje);
//TODO eliminarParticion
void eliminarParticion();
void actualizarBusquedasFallidas(int* busquedasFallidas);
//TODO compactarMemoria
void compactarMemoria();
void* buscarPrimerParticionLibre(uint32_t tamanioMensaje);
void* buscarMejorParticionLibre(uint32_t tamanioMensaje);





#endif /* PARTICIONESDINAMICAS_H_ */
