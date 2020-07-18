#ifndef FILE_SYSTEM_GAME_CARD_FILE_SYSTEM_H_
#define FILE_SYSTEM_GAME_CARD_FILE_SYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include "bloque_handler.h"
#include "gm_handler.h"
#include "Gamecard.h"
#include "gm_metadata.h"
#include "funcionesEnvio.c"

//------------------------ESTRUCTURAS-----------------------------------------------------------//

pthread_mutex_t MUTEX_METADATA;

//--------------------------------------FUNCIONES---------------------------------------------------------------------//

void gm_structs_fs();
void init_semaphore();
int directorio_recursivo(const char* path);
int crear_archivo(const char* fullPath);
void actualizar_pokemon_metadata(const char* fullPath, const char* directory, const char* size, const char* blocks, const char* open);
void updateOpenFileState(const char* fullPath, const char* open);
int coordinateExists(unsigned int posX, unsigned int posY, t_list* pokemonLines);
void addTotalPokemonIfCoordinateExist(new_pokemon* newPokemon, t_list* pokemonLines);
void deletePokemonTotalIfCoordinateExist(catch_pokemon* catchPokemon, t_list* pokemonLines);
t_list* requestFreeBlocks(int extraBlocksNeeded);
char* formatToMetadataBlocks(t_list* blocks);
void gm_liberar_bitmaps();
void liberar_block_line(blockLine* newLineBlock);
void createNewPokemon(new_pokemon* newPokemon);
int catchAPokemon(catch_pokemon* catchPokemon);
t_list* getAPokemon(get_pokemon* getPokemon);
void operateNewPokemonFile(new_pokemon* newPokemon, char* completePath, int freeBlocks);
t_list* operateGetPokemonFile(get_pokemon* getPokemon, char* completePath);
int operateCatchPokemonFile(catch_pokemon* catchPokemon, char* completePath);


#endif
