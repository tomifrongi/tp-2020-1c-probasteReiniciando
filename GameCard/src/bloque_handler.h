#ifndef BLOQUE_HANDLER_H
#define BLOQUE_HANDLER_H

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
#include "gm_metadata.h"
#include <stdint.h>

//--------------------------------ESTRUCTURAS---------------------------------------//

typedef struct {
	uint32_t cantidad;
	uint32_t posX;
	uint32_t posY;
} blockLine;


//--------------------------------FUNCIONES------------------------------------------//

int blocks_ocupados(char* value); /*cuantos bloques ocupa*/
int obtener_blocks(int tamanio); /*calcualar bloques*/
blockLine* crear_block_line(int intPosX, int intPosY, int intCantidad);
char* block_lines_string(t_list* pokemonLines);
t_list* string_blocks_list(char* blocks);
blockLine* estructura_block_line(char* blockline);
char* formatear_block_lines(int posX, int posY, int cantidad);
bool entra_en_blocks(char* stringToWrite, t_list* listBlocks);
void mostrar_lista_pokemons(t_list* pokemonLines);
void escribir_blocks(char* value, t_list* bloques);
t_list* leer_pokemons(t_list* blocks);

#endif //BLOQUE_HANDLER_H