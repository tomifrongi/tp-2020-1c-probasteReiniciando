

#ifndef MODOGAMECARD_H_
#define MODOGAMECARD_H_

#include "Gameboy.h"
#include "protocol.h"
#include "MensajesGamecard.h"
#include "Cliente.h"
#include "Impresora.h"
#include "Scanner.h"

void ejecutarModoGamecard();
t_message obtenerMensajeGamecard(int opcionMensaje);

#endif /* MODOGAMECARD_H_ */
