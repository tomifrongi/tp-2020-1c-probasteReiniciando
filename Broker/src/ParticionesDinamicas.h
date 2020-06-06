

#ifndef PARTICIONESDINAMICAS_H_
#define PARTICIONESDINAMICAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tiposMensajesEnMemoria.h"

//TODO buscarParticionLibre
bool buscarParticionLibre(void* memoriamemoria);
//TODO almacenarMensaje
void almacenarMensaje(int mensaje,void* memoriamemoria);
//TODO eliminarParticion
void eliminarParticion(void* memoriamemoria);
void actualizarBusquedasFallidas(int* busquedasFallidas);
//TODO compactarMemoria
void compactarMemoria(void* memoriamemoria);



#endif /* PARTICIONESDINAMICAS_H_ */
