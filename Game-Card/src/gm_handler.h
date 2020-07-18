#ifndef GM_HANDLER_H
#define GM_HANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include <commons/collections/list.h>
#include "Gamecard.h"

//-----------------------------------------------------ESTRUCTURAS-------------------------------------------------//
typedef struct {
	uint32_t blockSize;
	char* blocks;
	char* isOpen;
} t_pokemon_metadata;

//-----------------------------------------------------FUNCIONES--------------------------------------------------//

int es_char(const char* str, char chr);
int split_path(const char* path, char** super_path, char** name);
int _mkpath(char* file_path, mode_t mode);
char* obtener_path_nro_bloque(int numeroDeBloque);
t_pokemon_metadata leer_metadata_pokemon(char* pokemonPath);

#endif
