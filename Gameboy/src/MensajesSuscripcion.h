
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MENSAJESSUSCRIPCION_H_
#define MENSAJESSUSCRIPCION_H_

typedef enum {
	NEW,
	APPEARED,
	GET,
	LOCALIZED,
	CATCH,
	CAUGHT
}id_cola;

typedef struct {
	id_cola idCola;
} suscripcion;
//t_header = SUSCRIPCION

void* serializarSuscripcionContent(id_cola id);
char* obtenerNombreCola(id_cola id);

#endif /* MENSAJESSUSCRIPCION_H_ */
