

#ifndef MODOBROKER_H_
#define MODOBROKER_H_
#include "Gameboy.h"
#include "protocol.h"
#include "MensajesBroker.h"
#include "Cliente.h"
#include "Impresora.h"
#include "Scanner.h"

t_message obtenerMensajeBroker(int opcionMensaje);
void ejecutarModoBroker();

#endif /* MODOBROKER_H_ */
