
#ifndef MODOSUSCRIPTOR_H_
#define MODOSUSCRIPTOR_H_

#include "protocol.h"
#include "Cliente.h"
#include "MensajesSuscripcion.h"
#include "Scanner.h"
#include "stdbool.h"
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "gameboy.h"
#include "commons/collections/list.h"

typedef struct {
	uint32_t id_mensaje;
	pid_t idSuscriptor;
}mensajeACK;

void ejecutarModoSuscriptor(t_list* argumentos);
t_message obtenerMensajeSuscripcion(id_cola id);
int obtenerTiempo();
void* handler_envios(void* socket);
void enviarACK(t_message* mensajeRecibido, int socket);
#endif /* MODOSUSCRIPTOR_H_ */
