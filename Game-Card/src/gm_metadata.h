#ifndef GM_METADATA_H
#define GM_METADATA_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include "gm_bitmap.h"
#include "commons/log.h"
#include <math.h>
#include "funcionesEnvio.c"

//-----------------------------------------ESTRUCTURAS---------------------------------------------//

typedef enum {
	METADATA,
	FILES,
	BLOCKS,
	POKEMON,
	TALL_GRASS
} e_paths_structure;

typedef struct {
	unsigned int block_size, blocks;
	char* magic_number;
} Metadata;

Metadata metadata;

t_config* config_metadata;
t_config* config_table_metadata;

t_bitarray* bitmap;

FILE* bitmap_file;

char* struct_paths[5];

char* puntoMontaje;
char* ipBroker;
int puertoBroker;

//----------------------------------------------------FUNCIONES----------------------------------------------------//

void crear_root_files();
void conf_metadata();
void conf_files();
void crear_blocks();
void crear_bitmap(char* bitmapBin);
void crear_metadata_file(char* metadataBin);
void leer_metadata(char* metadataPath);
void leer_bitmap(char* bitmapBin);


#endif GM_METADATA_H
