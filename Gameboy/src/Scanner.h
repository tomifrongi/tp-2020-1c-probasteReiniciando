
#ifndef SCANNER_H_
#define SCANNER_H_
#include "MensajesBroker.h"
#include "MensajesGamecard.h"
#include "MensajesSuscripcion.h"

new_pokemon_broker obtenerParametrosNewBroker();
appeared_pokemon_broker obtenerParametrosAppearedBroker();
catch_pokemon_broker obtenerParametrosCatchBroker();
caught_pokemon_broker obtenerParametrosCaughtBroker();
get_pokemon_broker obtenerParametrosGetBroker();

new_pokemon_gamecard obtenerParametrosNewGamecard();
catch_pokemon_gamecard obtenerParametrosCatchGamecard();
get_pokemon_gamecard obtenerParametrosGetGamecard();

id_cola obtenerID();
#endif /* SCANNER_H_ */
