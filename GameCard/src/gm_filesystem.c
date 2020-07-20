#include "gm_filesystem.h"


void gm_structs_fs() //Estructuras del File System
{
	crear_root_files();
	conf_metadata();
	init_semaphore();
}

void init_semaphore() //inicio semaforo
{
	pthread_mutex_init(&MUTEX_METADATA, NULL);
}

int directorio_recursivo(const char* path)
{
	char* completePath = string_new();
	char* newDirectoryMetadata = string_new();
	char* super_path = (char*) malloc(strlen(path) +1);
	char* nombre = (char*) malloc(strlen(path)+1);

	string_append(&completePath, struct_paths[TALL_GRASS]);
	string_append(&completePath, path);

	if(access(completePath, F_OK) != -1)
	{
        log_info("Existe el path %s", completePath);
		return -1;
    }
	else
    {
        log_info(completePath, "No existe el path %s");
		split_path(path, &super_path, &nombre);
		
		directorio_recursivo(super_path);

		string_append(&newDirectoryMetadata, completePath);
		string_append(&newDirectoryMetadata, "Metadata.bin");

		mkdir(completePath, 0777);
		FILE* metadata = fopen(newDirectoryMetadata, "w+b");
		config_metadata = config_create(newDirectoryMetadata);
		config_set_value(config_metadata, "DIRECTORY", "Y");
		config_save(config_metadata);
		config_destroy(config_metadata);
		fclose(metadata);
		return 0;
    };

	free(completePath);
	free(newDirectoryMetadata);
	free(super_path);
	free(nombre);
	return 0;
}

int crear_archivo(const char* fullPath)
{
	char* completePath = string_new();
	string_append(&completePath, struct_paths[FILES]);
	string_append(&completePath, fullPath);

	if(access(completePath, F_OK) != -1)
	{
        log_info(completePath, "Existe el directory para ese pokemon %s");
		return -1;
    }
	else
	{
		mkdir(completePath, 0777);
		actualizar_pokemon_metadata(fullPath, "N", "0", "[]", "Y");
	}

	free(completePath);
}
//actualizar_pokemon_metadata
void actualizar_pokemon_metadata(const char* fullPath, const char* directory, const char* size, const char* blocks, const char* open)
{
	char* completePath = string_new();
	char* newDirectoryMetadata = string_new();
	string_append(&completePath, struct_paths[FILES]);
	string_append(&completePath, fullPath);

	string_append(&newDirectoryMetadata, completePath);
	string_append(&newDirectoryMetadata, "/Metadata.bin");
	
	FILE* metadata = fopen(newDirectoryMetadata, "w+b");
	config_metadata = config_create(newDirectoryMetadata);
	config_set_value(config_metadata, "DIRECTORY", directory);
	config_set_value(config_metadata, "SIZE", size);
	config_set_value(config_metadata, "BLOCKS", blocks);
	config_set_value(config_metadata, "OPEN", open);
	config_save(config_metadata);
	
	config_destroy(config_metadata);
	fclose(metadata);
	free(completePath);
	free(newDirectoryMetadata);
}

void updateOpenFileState(const char* fullPath, const char* open)
{
	char* completePath = string_new();
	char* newDirectoryMetadata = string_new();
	char* blockSize = string_new();
	char* blocks = string_new();

	string_append(&completePath, struct_paths[FILES]);
	string_append(&completePath, fullPath);
	string_append(&newDirectoryMetadata, completePath);
	string_append(&newDirectoryMetadata, "/Metadata.bin");


	t_config* readMetadataFile = config_create(newDirectoryMetadata);
	blockSize = string_duplicate(config_get_string_value(readMetadataFile, "SIZE"));
	blocks = string_duplicate(config_get_string_value(readMetadataFile, "BLOCKS"));
	
	FILE* metadata = fopen(newDirectoryMetadata, "w+b");
	config_metadata = config_create(newDirectoryMetadata);
	config_set_value(config_metadata, "SIZE", blockSize);
	config_set_value(config_metadata, "DIRECTORY", "N");
	config_set_value(config_metadata, "BLOCKS", blocks);
	config_set_value(config_metadata, "OPEN", open);
	config_save(config_metadata);
	
	config_destroy(config_metadata);
	fclose(metadata);
	config_destroy(readMetadataFile);
	
	free(completePath);
	free(newDirectoryMetadata);
	free(blockSize);
	free(blocks);
}

int coordinateExists(unsigned int posX, unsigned int posY, t_list* pokemonLines)
{
	int coordinateExist = 0;

	for (int i=0; i<list_size(pokemonLines); i++)
	{
		blockLine* newLineBlock = list_get(pokemonLines, i);

		if (newLineBlock->posX == posX && newLineBlock->posY == posY)
		{
			coordinateExist = 1;
		}
	}
	
	return coordinateExist;
}

void addTotalPokemonIfCoordinateExist(new_pokemon* newPokemon, t_list* pokemonLines)
{
	for (int i=0; i<list_size(pokemonLines); i++)
	{
		blockLine* pokemonLineBlock = list_get(pokemonLines, i);
		if (pokemonLineBlock->posX == newPokemon->posicionEjeX && pokemonLineBlock->posY == newPokemon->posicionEjeY)
		{
			pokemonLineBlock->cantidad = pokemonLineBlock->cantidad + newPokemon->cantidad;
		}
	}
}

void deletePokemonTotalIfCoordinateExist(catch_pokemon* catchPokemon, t_list* pokemonLines)
{
	for (int i=0; i<list_size(pokemonLines); i++)
	{
		blockLine* pokemonLineBlock = list_get(pokemonLines, i);
		if (pokemonLineBlock->posX == catchPokemon->posicionEjeX && pokemonLineBlock->posY == catchPokemon->posicionEjeY)
		{
			if (pokemonLineBlock->cantidad != 1)
			{
				pokemonLineBlock->cantidad = pokemonLineBlock->cantidad - 1;	
			}
			else
			{
				list_remove(pokemonLines, i);
			}
		}
	}
}

t_list* requestFreeBlocks(int extraBlocksNeeded)
{
	t_list* retList = list_create();
	for (int i=0; i<extraBlocksNeeded; i++)
	{
		int freeBlockPosition = obtener_conf_free_block(bitmap, metadata.blocks);
		list_add(retList, freeBlockPosition);
	}
	return retList;
}

// Formatea una lista de enteros a un string con formato [1, 2, 3] requerido por el Metadata
char* formatToMetadataBlocks(t_list* blocks)
{
	char* retBlocks = string_new();
	string_append(&retBlocks, "[");

	if (list_size(blocks) > 1)
	{
		for(int i=0; i<list_size(blocks); i++)
		{
			string_append(&retBlocks, string_itoa(list_get(blocks, i)));
			if (i != (list_size(blocks) - 1)) string_append(&retBlocks, ",");
		}
	} 
	
	if (list_size(blocks) == 1)
	{
		string_append(&retBlocks, string_itoa(list_get(blocks, 0)));
	}

	string_append(&retBlocks, "]");
	return retBlocks;
}

void gm_liberar_bitmaps()
{
	free(bitmap->bitarray);
	bitarray_destroy(bitmap);
}


void liberar_block_line(blockLine* newLineBlock)
{
	free(newLineBlock);
}

void createNewPokemon(new_pokemon* newPokemon)
{
	log_info("New Pokemon: %s", newPokemon->nombrePokemon);
	char* completePath = string_new();
	string_append(&completePath, struct_paths[FILES]);
	string_append(&completePath, newPokemon->nombrePokemon);
	int freeBlocks = liberar_blocks(metadata.blocks, bitmap);

	// Existe Pokemon
	if (access(completePath, F_OK) != -1)
	{
		log_info(logger, "Pokemon existe dentro del FS!.");
		operateNewPokemonFile(newPokemon, completePath, freeBlocks);
	}
	else
	{
		log_info(logger, "No existe ese Pokemon. Se crean y escriben las estructuras.");
		char* super_path = (char*) malloc(strlen(newPokemon->nombrePokemon) +1);
		char* pokemonDirectory = (char*) malloc(strlen(newPokemon->nombrePokemon)+1);
	
		if (string_contains(newPokemon->nombrePokemon, "/"))
		{
	    	split_path(newPokemon->nombrePokemon, &super_path, &pokemonDirectory);
			char* filePath = string_new();
			string_append(&filePath, "Files/");
			string_append(&filePath, super_path);
			directorio_recursivo(filePath);
			free(filePath);
		}

		crear_archivo(newPokemon->nombrePokemon);

		char* pokemonPerPosition = formatear_block_lines(newPokemon->posicionEjeX, newPokemon->posicionEjeY, newPokemon->cantidad);
		int pokemonPerPositionLength = strlen(pokemonPerPosition);

		// Necesito 1 solo bloque
		if(metadata.block_size >= pokemonPerPositionLength)
		{
		  
		  int blocksRequired = blocks_ocupados(pokemonPerPosition);

		  if (freeBlocks > blocksRequired)
		  {
			int freeBlockPosition = obtener_conf_free_block(bitmap, metadata.blocks);
			t_list* freeBlocks = list_create();
			list_add(freeBlocks, freeBlockPosition);
			char* metadataBlocks = formatToMetadataBlocks(freeBlocks);
			char* stringLength = string_itoa(pokemonPerPositionLength);
			
			char* pathBloque = obtener_path_nro_bloque(freeBlockPosition);
			FILE* blockFile = fopen(pathBloque,"wr");
			fwrite(pokemonPerPosition, 1 , pokemonPerPositionLength, blockFile);
			actualizar_pokemon_metadata(newPokemon->nombrePokemon, "N", stringLength, metadataBlocks, "N");
			log_info(logger, "Operacion NEW_POKEMON terminada correctamente");
			
			fclose(blockFile);
			free(metadataBlocks);
			free(stringLength);
			free(pathBloque);
			list_destroy(freeBlocks);

		  }
		  else
		  {
			log_error(logger, "No hay bloques disponibles. No se puede hacer la operacion");
		  }
		}
		else if(metadata.block_size < pokemonPerPositionLength)
		{
		  
		  t_list* pokemonLines = list_create();
		  blockLine* newNode = crear_block_line(newPokemon->posicionEjeX, newPokemon->posicionEjeY, newPokemon->cantidad);
		  list_add(pokemonLines, newNode);

		  char* stringToWrite = block_lines_string(pokemonLines);
		  int blocksRequired = blocks_ocupados(stringToWrite);

		  if (freeBlocks > blocksRequired)
		  {
			char* stringLength = string_itoa(strlen(stringToWrite));
			t_list* listBlocks = requestFreeBlocks(blocksRequired);
			escribir_blocks(stringToWrite, listBlocks);
			char* metadataBlocks = formatToMetadataBlocks(listBlocks);
			actualizar_pokemon_metadata(newPokemon->nombrePokemon, "N", stringLength, metadataBlocks, "N");
			log_info(logger, "Operacion NEW_POKEMON terminada correctamente");

			list_destroy(listBlocks);
			free(metadataBlocks);
		  }
		  else
		  {
			  log_error(logger, "No hay bloques disponibles. No se puede hacer la operacion");
		  }
		  
		  list_destroy_and_destroy_elements(pokemonLines, liberar_block_line);
		  free(stringToWrite);
	  	}

		free(super_path);
		free(pokemonDirectory);
		free(pokemonPerPosition);
	}
	
	free(completePath);
}

int catchAPokemon(catch_pokemon* catchPokemon)
{
	log_info(logger, "Catch Pokemon: %s", catchPokemon->nombrePokemon);
	char* completePath = string_new();
	int res;
	string_append(&completePath, struct_paths[FILES]);
	string_append(&completePath, catchPokemon->nombrePokemon);

	if (access(completePath, F_OK) != -1)
	{
		log_info(logger, "Existe el pokemon, se leen las estructuras");
		res = operateCatchPokemonFile(catchPokemon, completePath);
	}
	else
	{
		log_error(logger, "No existe ese Pokemon en el filesystem.");
	}

	free(completePath);

	return res;
}

t_list* getAPokemon(get_pokemon* getPokemon)
{
	log_info(logger, "Get Pokemon: %s", getPokemon->nombrePokemon);
	char* completePath = string_new();
	string_append(&completePath, struct_paths[FILES]);
	string_append(&completePath, getPokemon->nombrePokemon);
	t_list* res;

	if (access(completePath, F_OK) != -1)
	{
		log_info(logger, "Existe el pokemon, se leen las estructuras");
		res = operateGetPokemonFile(getPokemon, completePath);
	}
	else
	{
		log_error(logger, "No existe ese Pokemon en el filesystem.");
	}

	free(completePath);
	return res;
}

void operateNewPokemonFile(new_pokemon* newPokemon, char* completePath, int freeBlocks)
{
	t_pokemon_metadata* pokemonMetadata = leer_pokemons(completePath);

	if(string_equals_ignore_case(pokemonMetadata->isOpen, "N"))
	{
		printf("El archivo no esta abierto por ningun proceso, se procede a abrir el mismo.");
		
		pthread_mutex_lock(&MUTEX_METADATA);
		updateOpenFileState(newPokemon->nombrePokemon, "Y");
		pthread_mutex_unlock(&MUTEX_METADATA);
		
		t_list* listBlocks = string_blocks_list(pokemonMetadata->blocks);
		t_list* pokemonLines = leer_pokemons(listBlocks);
		if (coordinateExists(newPokemon->posicionEjeX, newPokemon->posicionEjeY, pokemonLines) == 1)
		{
			addTotalPokemonIfCoordinateExist(newPokemon, pokemonLines);
		}
		else
		{
			blockLine* newNode = crear_block_line(newPokemon->posicionEjeX, newPokemon->posicionEjeY, newPokemon->cantidad);
			list_add(pokemonLines, newNode);
		}
		
		char* stringToWrite = string_blocks_list(pokemonLines);
		int blocksRequired = blocks_ocupados(stringToWrite);
		char* stringLength = string_itoa(strlen(stringToWrite));

		if (freeBlocks > blocksRequired) {
			// Necesito pedir bloques
			if (blocksRequired > list_size(listBlocks)) {
				int extraBlocksNeeded = blocksRequired - list_size(listBlocks);
				t_list* extraBlocks = requestFreeBlocks(extraBlocksNeeded);
				// Agrego los nuevos bloques en la lista original
				list_add_all(listBlocks, extraBlocks);
				list_destroy(extraBlocks);
			} 
			escribir_blocks(stringToWrite, listBlocks);
			char* metadataBlocks = formatToMetadataBlocks(listBlocks);
			
			pthread_mutex_lock(&MUTEX_METADATA);
			actualizar_pokemon_metadata(newPokemon->nombrePokemon, "N", stringLength, metadataBlocks, "N");
			pthread_mutex_unlock(&MUTEX_METADATA);

			log_info(logger, "Operacion NEW_POKEMON terminada correctamente");
			free(metadataBlocks);
		}
		else
		{
			log_error(logger, "No hay bloques disponibles. No se puede hacer la operacion");
		}

		list_destroy_and_destroy_elements(pokemonLines, liberar_block_line);
		free(stringToWrite);
		free(stringLength);
	}
	else
	{
		log_info(logger, "Archivo abierto, se procede a reintentar luego de %d segundos", tiempoReintentoOperacion);
		sleep(tiempoReintentoOperacion);
		operateNewPokemonFile(newPokemon, completePath, freeBlocks);
	}

	free(pokemonMetadata->blocks);
	free(pokemonMetadata->isOpen);
}


t_list* operateGetPokemonFile(get_pokemon* getPokemon, char* completePath) {
	t_pokemon_metadata* pokemonMetadata = leer_pokemons(completePath);
	t_list* res;

	if (string_equals_ignore_case(pokemonMetadata->isOpen, "N"))
	{
		printf("El archivo no esta abierto por ningun proceso, se procede a abrir el mismo.");
		
		pthread_mutex_lock(&MUTEX_METADATA);
		updateOpenFileState(getPokemon->nombrePokemon, "Y");
		pthread_mutex_unlock(&MUTEX_METADATA);

		t_list* listBlocks = string_blocks_list(pokemonMetadata->blocks);
		res = leer_pokemons(listBlocks);
		
		pthread_mutex_lock(&MUTEX_METADATA);
		updateOpenFileState(getPokemon->nombrePokemon, "N");
		pthread_mutex_unlock(&MUTEX_METADATA);

		log_info(logger, "Operacion GET_POKEMON terminada correctamente");
	}
	else
	{
		log_info(logger, "Archivo abierto, se procede a reintentar luego de %d segundos", tiempoReintentoOperacion);
		sleep(tiempoReintentoOperacion);
		operateGetPokemonFile(getPokemon, completePath);
	}

	free(pokemonMetadata->blocks);
	free(pokemonMetadata->isOpen);
	return res;
}

int operateCatchPokemonFile(catch_pokemon* catchPokemon, char* completePath)
{
	t_pokemon_metadata* pokemonMetadata = leer_pokemons(completePath);
	int res = 0;

	if (string_equals_ignore_case(pokemonMetadata->isOpen, "N"))
	{
		log_info(logger, "El archivo no esta abierto por ningun proceso, se procede a abrir el mismo.");
		
		pthread_mutex_lock(&MUTEX_METADATA);
		updateOpenFileState(catchPokemon->nombrePokemon, "Y");
		pthread_mutex_unlock(&MUTEX_METADATA);

		t_list* listBlocks = string_blocks_list(pokemonMetadata->blocks);
		t_list* pokemonLines = leer_pokemons(listBlocks);

		if (coordinateExists(catchPokemon->posicionEjeX, catchPokemon->posicionEjeY, pokemonLines) == 1)
		{
			deletePokemonTotalIfCoordinateExist(catchPokemon, pokemonLines);
			char* stringToWrite = block_lines_string(pokemonLines);
			int blocksRequired = blocks_ocupados(stringToWrite);
			char* stringLength = string_itoa(strlen(stringToWrite));

			if (strlen(stringToWrite) != 0)
			{
				if (blocksRequired == list_size(listBlocks))
				{
					escribir_blocks(stringToWrite, listBlocks);
					char* metadataBlocks = formatToMetadataBlocks(listBlocks);
					
					pthread_mutex_lock(&MUTEX_METADATA);
					actualizar_pokemon_metadata(catchPokemon->nombrePokemon, "N", stringLength, metadataBlocks, "N");
					pthread_mutex_unlock(&MUTEX_METADATA);
					free(metadataBlocks);
				}

				if (blocksRequired < list_size(listBlocks))
				{
					int lastBlockUsing = list_get(listBlocks, list_size(listBlocks) - 1 );
					list_remove(listBlocks, list_size(listBlocks) - 1);
					escribir_blocks(stringToWrite, listBlocks);
					char* metadataBlocks = formatToMetadataBlocks(listBlocks);
					
					
					pthread_mutex_lock(&MUTEX_METADATA);
					actualizar_pokemon_metadata(catchPokemon->nombrePokemon, "N", stringLength, metadataBlocks, "N");
					pthread_mutex_unlock(&MUTEX_METADATA);
					
					// Limpio estructuras que ya no uso
					conf_liberar_bloq_in_pos(bitmap, lastBlockUsing);
					fclose(fopen(obtener_path_nro_bloque(lastBlockUsing), "w"));
					free(metadataBlocks);
				}
			} 

			// Edge case donde el pokemon tiene una sola linea, un solo bloque asignado y la unica coordenada es == 1
			// Asumo que el bloque se queda ocupado pero con size = 0
			if (strlen(stringToWrite) == 0 && list_size(pokemonLines) == 0)
			{
				int blockUsed = list_get(listBlocks, 0);
				char* metadataBlocks = formatToMetadataBlocks(listBlocks);
				char* zeroLength = string_itoa(0);

				pthread_mutex_lock(&MUTEX_METADATA);
				actualizar_pokemon_metadata(catchPokemon->nombrePokemon, "N", zeroLength, metadataBlocks, "N");
				pthread_mutex_unlock(&MUTEX_METADATA);
					
				// Limpio estructuras que ya no uso
				fclose(fopen(obtener_path_nro_bloque(blockUsed), "w"));
				free(zeroLength);
				free(metadataBlocks);
			}
			res = 1;
			log_info(logger, "Operacion CATCH_POKEMON terminada correctamente");

			free(stringToWrite);
			free(stringLength);
		}
		else
		{
			log_error(logger, "No existen las coordenadas para ese pokemon, no se puede completar la operacion.");
		}

		list_destroy_and_destroy_elements(pokemonLines, liberar_block_line);
	}
	else
	{
		log_info("Archivo abierto, se procede a reintentar luego de %d segundos", tiempoReintentoOperacion);
		sleep(tiempoReintentoOperacion);
		operateCatchPokemonFile(catchPokemon, completePath);
	}

	free(pokemonMetadata->blocks);
	free(pokemonMetadata->isOpen);

	return res;
}
//Ver que falta