
#ifndef MODOSUSCRIPTOR_H_
#define MODOSUSCRIPTOR_H_

#include "Gameboy.h"
#include "protocol.h"
#include "Cliente.h"
#include "MensajesSuscripcion.h"
#include "Scanner.h"
#include "stdbool.h"
#include <time.h>
#include <unistd.h>
#include <pthread.h>



void ejecutarModoSuscriptor();
t_message obtenerMensajeSuscripcion(id_cola id);
int obtenerTiempo();
void* handler_envios(void* socket);

#endif /* MODOSUSCRIPTOR_H_ */
