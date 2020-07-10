

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
	}particion_dinamica_memoria;

t_list* particionesEnMemoria;
void* principioMemoria;
t_queue* colaMensajesMemoria;
int CONTADORLRU;

void ejecutarPruebaBaseBroker();
void ejecutarPruebaBaseBroker2();
void ejecutarPruebaConsolidacionBroker();
void ejecutarPruebaConsolidacionBroker2();
void ejecutarPruebaCompactacionMio();
particion_dinamica_memoria* crear_particion_dinamica_memoria(particion_dinamica_memoria particion);
void borrar_particion_dinamica_memoria(particion_dinamica_memoria* particion);

void inicializarMemoria();

bool buscarParticionLibre(uint32_t tamanioMensaje);
void almacenarMensaje(void* mensaje,id_cola id);
void eliminarParticion();
void actualizarBusquedasFallidas(int* busquedasFallidas);
void compactarMemoria();
void consolidarMemoria();
void ordenarParticionesPorPosicion();
particion_dinamica_memoria* buscarPrimerParticionLibre(uint32_t tamanioMensaje);
particion_dinamica_memoria* buscarMejorParticionLibre(uint32_t tamanioMensaje);



void agregarParticionContigua(particion_dinamica_memoria* particion,uint32_t tamanioParticionAntigua);

void ejecutarCicloNormal(void* mensaje,id_cola id);
void ejecutarCicloAlternativo(void* mensaje,id_cola id);
bool particionesOcupadas();
void cachearMensaje(void* mensaje,id_cola id);

particion_dinamica_memoria* cargarDatosParticion(particion_dinamica_memoria* particion,void* mensaje,id_cola id);
t_list* sacarParticionesLibres();
int* crear_elemento_colaMensajesMemoria(int idMensaje);
particion_dinamica_memoria* removerPorPosicion(int posicion);
void borrar_elemento_colaMensajesMemoria(int* idMensaje);

#endif /* PARTICIONESDINAMICAS_H_ */
