#include "gm_bitmap.h"

void mostrar_bitarray(t_bitarray* bitmap)
{
	for(int k =0;k<(bitarray_get_max_bit(bitmap));k++)
	log_info("test bit posicion, es  %d en posicion %d \n", bitarray_test_bit(bitmap,k)/*,k*/);
}

void conf_bloq_in_pos(t_bitarray* bitmap, off_t pos)
{
	bitarray_set_bit(bitmap, pos);
}

void conf_liberar_bloq_in_pos(t_bitarray* bitmap, off_t pos)
{
	bitarray_clean_bit(bitmap, pos);
}

bool testear_bloq_lib_in_pos(t_bitarray* bitmap, int pos)
{
	return bitarray_test_bit(bitmap, (off_t)(pos));
}

// Obtiene y setea el proximo bloque libre
int obtener_conf_free_block(t_bitarray* bitmap, unsigned int blocks)
{
	int j;
	for(j =0; testear_bloq_lib_in_pos(bitmap, j); j++); // Hasta un bloque libre
	conf_bloq_in_pos(bitmap, j);
	return j;
}

// Retorna la cantidad de bloques libres
int liberar_blocks(int metadataBlocks, t_bitarray* bitmap)
{
    int bloques_libres = 0;
    int bloque_libre;
    int bit = 0;
    int tamMaximo = metadataBlocks;
    while(bit < tamMaximo)
    {
        bloque_libre = bitarray_test_bit(bitmap,bit);
        if(bloque_libre == 0)bloques_libres ++;
        bit++;
    }

    return bloques_libres;
}
