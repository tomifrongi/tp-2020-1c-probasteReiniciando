/*
 * Configuracion.h
 *
 *  Created on: 7 jun. 2020
 *      Author: utnso
 */

#ifndef CONFIGURACION_H_
#define CONFIGURACION_H_

#include <commons/config.h>

t_config* config;

int TAMANO_MEMORIA;
int TAMANO_MINIMO_PARTICION;
char* ALGORITMO_MEMORIA;
char* ALGORITMO_REEMPLAZO;
char* ALGORITMO_PARTICION_LIBRE;
char* IP_BROKER;
int PUERTO_BROKER;
int FRECUENCIA_COMPACTACION;
char* LOG_FILE;

void cargarConfiguracion();

#endif /* CONFIGURACION_H_ */
