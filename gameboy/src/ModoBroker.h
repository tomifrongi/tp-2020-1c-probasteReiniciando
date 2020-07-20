

#ifndef MODOBROKER_H_
#define MODOBROKER_H_
#include "protocol.h"
#include "MensajesBroker.h"
#include "Cliente.h"
#include "gameboy.h"
#include "Impresora.h"
#include "Scanner.h"
#include "commons/collections/list.h"
t_message obtenerMensajeBroker(int opcionMensaje,t_list* argumentos);
void ejecutarModoBroker(t_list* argumentos);

#endif /* MODOBROKER_H_ */
