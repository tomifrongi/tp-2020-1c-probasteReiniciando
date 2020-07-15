
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



void ejecutarModoSuscriptor();
t_message obtenerMensajeSuscripcion(id_cola id);
int obtenerTiempo();

#endif /* MODOSUSCRIPTOR_H_ */
