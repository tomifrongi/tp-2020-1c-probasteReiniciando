

#ifndef BUDDYSYSTEM_H_
#define BUDDYSYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include "ProtocoloDeMensajes.h"

#include "tiposMensajesEnMemoria.h"
#include "Configuracion.h"
#include <math.h>

typedef struct {
	int posicionParticion; //es como el bytes escritos de serializacion. la primer posicion es 0. la ultima posicion es TAMANO_PARTICION - 1
	bool libre; //1 si esta libre, 0 si no.
	int tamanio;
	int tamanioMensaje;
	int idMensaje;
	int idCorrelativo;
	int cola;
	int contadorLRU; //Se actualiza cuando lo agregas a memoria y cuando lo envias
	t_list* suscriptoresMensajeEnviado;
	t_list* suscriptoresACK;
}particion_buddy_memoria;

t_list* particionesEnMemoria;
void* principioMemoria;
t_queue* colaMensajesMemoria;
int CONTADORLRU;


void cachearMensajeBuddy(void* mensaje,id_cola id);
bool almacenarMensajeBuddy(void* mensaje,id_cola id);
particion_buddy_memoria* buscarPrimerParticionLibreBuddy(uint32_t tamanioMensaje);
particion_buddy_memoria* buscarMejorParticionLibreBuddy(uint32_t tamanioMensaje);
void agregarBuddy(particion_buddy_memoria* particion);
particion_buddy_memoria* cargarDatosParticionBuddy(particion_buddy_memoria* particion,void* mensaje,id_cola id);
int buscarPotenciaDeDosMasCercana(uint32_t tamanio);
void eliminarParticionBuddy();
void consolidarMemoriaBuddy();
t_list* sacarParticionesLibresBuddy();

particion_buddy_memoria* crear_particion_buddy_memoria(particion_buddy_memoria particion);
void borrar_particion_buddy_memoria(particion_buddy_memoria* particion);

int* crear_elemento_colaMensajesMemoriaBuddy(int idMensaje);
void borrar_elemento_colaMensajesMemoriaBuddy(int* idMensaje);

void ordenarParticionesPorPosicionBuddy();
particion_buddy_memoria* removerPorPosicionBuddy(int posicion);
void sacarBarraCeroBuddy(void* mensaje,id_cola id);

#endif /* BUDDYSYSTEM_H_ */
