
#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include "Cliente.h"
#include "commons/config.h"
#include "commons/log.h"
#include "Impresora.h"
#include "MensajesBroker.h"
#include "protocol.h"
#include "Scanner.h"
#include "MensajesGamecard.h"
#include "time.h"
#define MAX_CLIENTS 128
#include "MensajesSuscripcion.h"

t_message obtenerMensajeBroker(int opcionMensaje);
t_message obtenerMensajeTeam();
t_message obtenerMensajeGamecard(int opcionMensaje);
t_message obtenerMensajeSuscripcion(id_cola id);
int obtenerTiempo();
#endif /* GAMEBOY_H_ */
