
#ifndef MODOSUSCRIPTOR_H_
#define MODOSUSCRIPTOR_H_

#include "Gameboy.h"
#include "protocol.h"
#include "Cliente.h"
#include "MensajesSuscripcion.h"
#include "Scanner.h"
#include "stdbool.h"
#include "commons/temporal.h"

void ejecutarModoSuscriptor();
t_message obtenerMensajeSuscripcion(id_cola id);
int obtenerTiempo();
bool compararTiempo(int duracion,char*tiempoInicio);

#endif /* MODOSUSCRIPTOR_H_ */
