
#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include "Cliente.h"
#include "commons/config.h"
#include "Impresora.h"
#include "MensajesBroker.h"
#include "protocol.h"
#include "Scanner.h"
#include "MensajesGamecard.h"
#define MAX_CLIENTS 128


t_message obtenerMensajeBroker(int opcionMensaje);
t_message obtenerMensajeTeam();
t_message obtenerMensajeGamecard(int opcionMensaje);
#endif /* GAMEBOY_H_ */
