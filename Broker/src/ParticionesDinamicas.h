

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

//TODO falta agregar suscriptores a la particion.
//Los suscriptores a los cuales ya se envió el mensaje.
//Los suscriptores que retornaron el ACK del mismo.

typedef struct {
	//TODO actualizar posicionParticion en la implementacion
	int posicionParticion; //es como el bytes escritos de serializacion. la primer posicion es 0. la ultima posicion es TAMANO_PARTICION - 1
	bool libre; //1 si esta libre, 0 si no.
	int tamanio;
	int tamanioMensaje;
	int idMensaje;
	int idCorrelativo;
	int cola;
	}particion_dinamica_memoria;

t_list* particionesEnMemoria;

void* principioMemoria;

t_queue* colaMensajesMemoria;
t_list* ultimasReferencias; // la ultima particion usada se agrega al principio de la lista, la menos usada esta al final.

//TODO crear y borrar particion dinamica
particion_dinamica_memoria* crear_particion_dinamica_memoria(particion_dinamica_memoria particion);
void borrar_particion_dinamica_memoria(particion_dinamica_memoria* particion);

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
