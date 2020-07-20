

#ifndef MODOGAMECARD_H_
#define MODOGAMECARD_H_

#include "protocol.h"
#include "MensajesGamecard.h"
#include "Cliente.h"
#include "gameboy.h"
#include "Impresora.h"
#include "Scanner.h"
#include "commons/collections/list.h"
void ejecutarModoGamecard(t_list* argumentos);
t_message obtenerMensajeGamecard(int opcionMensaje, t_list* argumentos);

#endif /* MODOGAMECARD_H_ */
