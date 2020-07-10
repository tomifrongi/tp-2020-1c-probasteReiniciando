#include <string.h>
#include <stdio.h>
#include <commons/bitarray.h>
#include <commons/txt.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>
#include <commons/bitarray.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "../logger/gamecard_logger.h"

void show_bitarray(t_bitarray* bitmap);
void set_bloq_occ_in_pos(t_bitarray* bitmap, off_t pos);
void set_bloq_free_in_pos(t_bitarray* bitmap, off_t pos);
bool test_bloq_free_in_pos(t_bitarray* bitmap, int pos);
int get_and_set_free_block(t_bitarray* bitmap, unsigned int blocks);
int get_free_blocks(int metadataBlocks, t_bitarray* bitmap);