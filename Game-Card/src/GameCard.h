#ifndef GAMECARD_H__
#define GAMECARD_H__

#include "Cliente.h"
#include "commons/config.h"
#include "commons/log.h"
#include <commons/collections/list.h>
#include "protocol.h"
#include "MensajesGamecard.h"
#include "time.h"

#define MAX_CLIENTS 128

typedef struct{
	int cord_x;
	char guion;
	int cord_y;
	char igual;
	int cantidad;
}t_linea;



int obtenerTiempo();
int cantidad_pokemones(FILE* archivo_pokemon);
void existen_posiciones_pokemon_nuevo(FILE* archivo_pokemon,new_pokemon pokemon_nuevo);
void existen_posiciones_pokemon_atrapado(FILE* archivo_pokemon,catch_pokemon pokemon_atrapado);
void decrementar_cantidad(FILE* archivo_pokemon,catch_pokemon pokemon_atrapado);
void eliminarLinea(FILE* archivo_pokemon,t_linea linea_a_borrar);
t_list* obtener_posiciones_y_cantidades(FILE* archivo_pokemon);




#endif /* GAMECARD_H_ */
