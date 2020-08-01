#ifndef GM__FILE_SYSTEM_H_
#define GM__FILE_SYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <semaphore.h>
#include <math.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "commons/log.h"
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include <commons/txt.h>
#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>

#include "ProtocoloDeMensajes.h"
#include "Gamecard.h"

/*
 * Estructuras
 * */

pthread_mutex_t MUTEX_METADATA;

t_dictionary* files_open;

pthread_mutex_t MUTEX_LISTA_ARCHIVO_ABIERTO;

typedef struct pokemon_open_tad {
    pthread_mutex_t mArchivo;
} pokemon_open_tad;

typedef struct {
	uint32_t cantidad;
	uint32_t posX;
	uint32_t posY;
} t_blockLine;

typedef struct {
	uint32_t blockSize;
	char* blocks;
	char* isOpen;
} t_pokemon_metadata;

typedef struct {
	uint32_t cantidad;
	uint32_t posX;
	uint32_t posY;
} blockLine;

typedef enum {
	METADATA,
	FILES,
	BLOCKS,
	POKEMON,
	TALL_GRASS
} e_paths_structure;

typedef struct {
	int block_size;
	int blocks;
	char* magic_number;
} Metadata;

//#define RUTA_BITMAP_GENERAL "/Metadata/Bitmap.bin"
Metadata metadata;

t_config* config_metadata;
t_config* config_table_metadata;
t_bitarray* bitmap;
FILE* bitmap_file;
char* struct_paths[5];

#define RUTA_METADATA_GENERAL "/Metadata/Metadata.bin"
#define RUTA_BITMAP_GENERAL "/Metadata/Bitmap.bin"
int g_blocks_maximos;
int g_blocks_usados;

/*
 * Funciones
 * */

void gm_structs_fs();
void init_semaphore();
int directorio_recursivo(const char* path);
int crear_archivo(char* fullPath);
void actualizar_pokemon_metadata(char* fullPath, char* directory, char* size, char* blocks, char* open, char* op);
void updateOpenFileState(char* fullPath, char* open, char* op);

int coordinateExists(unsigned int posX, unsigned int posY, t_list* pokemonLines);
void addTotalPokemonIfCoordinateExist(new_pokemon* newPokemon, t_list* pokemonLines);
void deletePokemonTotalIfCoordinateExist(catch_pokemon* catchPokemon, t_list* pokemonLines);
t_list* requestFreeBlocks(int extraBlocksNeeded);
char* formatToMetadataBlocks(t_list* blocks);

void gcfsFreeBitmaps();
void liberar_block_line(t_blockLine* newLineBlock);

void createNewPokemon(new_pokemon* newPokemon);
int catchAPokemon(catch_pokemon* catchPokemon);
t_list* getAPokemon(get_pokemon* getPokemon);

void operateNewPokemonFile(new_pokemon* newPokemon, char* completePath, int freeBlocks);
t_list* operateGetPokemonFile(get_pokemon* getPokemon, char* completePath);
int operateCatchPokemonFile(catch_pokemon* catchPokemon, char* completePath);

pokemon_open_tad* new_pokemon_open_tad();

int es_char(char* str, char chr);
int split_path(char* path, char** super_path, char** name);
int _mkpath(char* file_path, mode_t mode);
char* obtener_path_nro_bloque(int numeroDeBloque);
t_pokemon_metadata leer_metadata_pokemon(char* pokemonPath);

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

void mostrar_bitarray(t_bitarray* bitmap);
void conf_bloq_in_pos(t_bitarray* bitmap, off_t pos);
void conf_liberar_bloq_in_pos(t_bitarray* bitmap, off_t pos);
bool testear_bloq_lib_in_pos(t_bitarray* bitmap, int pos);
int obtener_conf_free_block(t_bitarray* bitmap, unsigned int blocks);
int liberar_blocks(int metadataBlocks, t_bitarray* bitmap);

void crear_root_files();
void conf_metadata();
void conf_files();
void crear_blocks();
t_bitarray* crear_bitmap(char* bitmapBin);
void crear_metadata_file(char* metadataBin);
void leer_metadata(char* metadataPath);
void leer_bitmap(char* bitmapBin);


#endif

