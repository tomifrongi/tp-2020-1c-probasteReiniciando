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
#include "../logger/gamecard_logger.h"
#include "../config/gamecard_config.h"
//#include "../shared-common/common/utils.h"
//#include <commons/utils.h>
#include "./gamecard_setup.h"


typedef struct {
	uint32_t cantidad;
	uint32_t posX;
	uint32_t posY;
} blockLine;

int get_blocks(int tamanio); /*calcualar bloques*/
int blocks_occ(char* value); /*cuantos bloques ocupa*/ 
blockLine* create_block_line(int intPosX, int intPosY, int intCantidad);
// Formatters
char* formatListToStringLine(t_list* pokemonLines);
t_list* stringBlocksToList(char* blocks);
blockLine* formatStringToBlockLine(char* blockline);
char* formatToBlockLine(int posX, int posY, int cantidad);

bool stringFitsInBlocks(char* stringToWrite, t_list* listBlocks);
void printListOfPokemonReadedLines(t_list* pokemonLines);

void write_blocks(char* value, t_list* bloques);
t_list* readPokemonLines(t_list* blocks);
