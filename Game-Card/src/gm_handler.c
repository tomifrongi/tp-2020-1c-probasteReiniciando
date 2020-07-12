#include "gm_handler.h"


//Evalua si el ultimo caracter de string es char
int es_char(const char* str, char chr){
	if ( ( str[strlen(str)-1]  == chr) ) return 1;
	return 0;
}

/*
 * 	DESC
 * 		Divide el path con formato de [RUTA] en: [RUTA_SUPERIOR] y [NOMBRE].
 * 		Ejemplo:
 * 			path: /home/utnso/algo.txt == /home/utnso - algo.txt
 * 			path: /home/utnso/ == /home - utnso
 *
 * 	PARAM
 * 		path - Ruta a dividir
 * 		super_path - Puntero sobre el cual se guardara la ruta superior.
 * 		name - Puntero al nombre del archivo
 *
 * 	RET
 * 		0... SIEMPRE!;
 */

int split_path(const char* path, char** super_path, char** name){
	int aux;
	strcpy(*super_path, path);
	strcpy(*name, path);
	// Obtiene y acomoda el nombre del archivo.
	if (lastchar(path, '/')) {
		(*name)[strlen(*name)-1] = '\0';
	}
	*name = strrchr(*name, '/');
	*name = *name + 1; // Acomoda el nombre, ya que el primer digito siempre es '/'

	// Acomoda el super_path
	if (lastchar(*super_path, '/')) {
		(*super_path)[strlen(*super_path)-1] = '\0';
	}
	aux = strlen(*super_path) - strlen(*name);
	(*super_path)[aux] = '\0';

	return 0;
}

int _mkpath(char* file_path, mode_t mode)
{
	assert(file_path && *file_path);
	char* p;
	for(p = strchr(file_path + 1, '/'); p; p = strchr(p + 1, '/'))
	{
		*p = '\0';
		if(mkdir(file_path, mode) == -1)
		{
			if (errno != EEXIST)
			{
				*p = '/';
				return -1;
			}
		}
		*p = '/';
	}
	return 0;
}

//Lo que hace es obtener el path que corresponde al bloque en el que estamos trabajando
char* obtener_path_nro_bloque(int numeroDeBloque){
	char* path_del_bloque = malloc(strlen(puntoMontaje)+strlen("/Bloques")+20);
	sprintf(path_del_bloque,"%sBloques/%d.bin",puntoMontaje, numeroDeBloque);
	return path_del_bloque;
}

//Esta funcion es para leer el metadata del pokemon
t_pokemon_metadata leer_metadata_pokemon(char* pokemonPath) {
	char* existingPokemonMetadata = string_new();
	char* existingPokemonBlocks = string_new();

	t_pokemon_metadata metadata;

	metadata.blocks = string_new();
	metadata.isOpen = string_new();
	
	string_append(&existingPokemonMetadata, pokemonPath);
	string_append(&existingPokemonMetadata, "/Metadata.bin");

	t_config* metadataFile = config_create(existingPokemonMetadata);
	metadata.blockSize = config_get_int_value(metadataFile, "SIZE");
	metadata.blocks = string_duplicate(config_get_string_value(metadataFile, "BLOCKS"));
	metadata.isOpen = string_duplicate(config_get_string_value(metadataFile, "OPEN"));
	config_destroy(metadataFile);

	free(existingPokemonMetadata);
	free(existingPokemonBlocks);
	return metadata;
}
