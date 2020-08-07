#include "gm_filesystem.h"
#include <math.h>
void gm_structs_fs() //Estructuras del File System
{
	crear_root_files();
	conf_metadata();
	init_semaphore();
}

void init_semaphore()
{
	pthread_mutex_init(&MUTEX_LISTA_ARCHIVO_ABIERTO, NULL);
	files_open = dictionary_create();
}

/*
 * Directorios, se chequean los path
 * */
int directorio_recursivo(char* path)
{
	char* completePath = string_new();

	char* newDirectoryMetadata = string_new();

	char* super_path = (char*) malloc(strlen(path) +1);

	char* nombre = (char*) malloc(strlen(path)+1);

	string_append(&completePath, struct_paths[TALL_GRASS]);
	string_append(&completePath, path);

	if(access(completePath, F_OK) != -1)
	{
        log_info(logger, "Existe el path %s", completePath);
		return -1;
    }
	else
    {
        log_info(logger, "No existe el path %s", completePath);
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

/*
 * Crear archivo para el pokemon
 * */
int crear_archivo(char* fullPath)
{
	char* completePath = string_new();

	string_append(&completePath, struct_paths[FILES]);
	string_append(&completePath, fullPath);

	if(access(completePath, F_OK) != -1)
	{
        log_info(logger, "Existe el directorio para el pokemon dado %s", completePath);
		return -1;
    }
	else
	{
		mkdir(completePath, 0777);
		actualizar_pokemon_metadata(fullPath, "N", "0", "[]", "N", "NEW_POKEMON");
	}

	free(completePath);
	return 0;
}

/*
 * Actualizar en files/pokemon el metadata.bin que le corresponde a dicho pokemon
 * */
void actualizar_pokemon_metadata(char* fullPath, char* directory, char* size, char* blocks, char* open, char* op)
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

/*
 * Se actualiza a la carpeta files el metadata.bin correspondiente
 * */
void updateOpenFileState(char* fullPath, char* open, char* op)
{
	char* completePath = string_new();

	char* newDirectoryMetadata = string_new();

	char* blockSize;

	char* blocks;

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

/*
 * Chequear coordenadas del pokemon dado
 * */
int coordinateExists(unsigned int posX, unsigned int posY, t_list* pokemonLines)
{
	int coordinateExist = 0;
	for (int i=0; i<list_size(pokemonLines); i++)
	{
		blockLine* newLineBlock = (blockLine*) list_get(pokemonLines, i);

		if (newLineBlock->posX == posX && newLineBlock->posY == posY)
		{
			coordinateExist = 1;
		}
	}
	return coordinateExist;
}

/*
 * Agregar pokemon en caso de que las coordenadas sean las correctas
 * */
void addTotalPokemonIfCoordinateExist(new_pokemon* newPokemon, t_list* pokemonLines)
{
	for (int i=0; i<list_size(pokemonLines); i++)
	{
		blockLine* pokemonLineBlock = (blockLine*) list_get(pokemonLines, i);
		if (pokemonLineBlock->posX == newPokemon->posicionEjeX && pokemonLineBlock->posY == newPokemon->posicionEjeY)
		{
			pokemonLineBlock->cantidad = pokemonLineBlock->cantidad + newPokemon->cantidad;
		}
	}
}

/*
 * Eliminar pokemon en caso de que las coordenadas sean correctas
 * */
void deletePokemonTotalIfCoordinateExist(catch_pokemon* catchPokemon, t_list* pokemonLines)
{
	for (int i=0; i<list_size(pokemonLines); i++)
	{
		blockLine* pokemonLineBlock = (blockLine*) list_get(pokemonLines, i);
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

/*
 * Pedir bloques libres
 * */
t_list* requestFreeBlocks(int extraBlocksNeeded)
{
	t_list* retList = list_create();
	for (int i=0; i<extraBlocksNeeded; i++)
	{
		int freeBlockPosition = obtener_conf_free_block(bitmap, metadata.blocks);
		list_add(retList, (void*) freeBlockPosition);
	}
	return retList;
}
/*
 * Formatea una lista de enteros a un string con formato [1,2,3,4,5,6,...] requerido por el metadata
 * */
char* formatToMetadataBlocks(t_list* blocks)
{
	char* retBlocks = string_new();
	string_append(&retBlocks, "[");

	if (list_size(blocks) > 1)
	{
		for(int i=0; i<list_size(blocks); i++)
		{
			char* aux1 = string_itoa((int)list_get(blocks, i));
			string_append(&retBlocks, aux1);//CORREGIDO
			if (i != (list_size(blocks) - 1)) string_append(&retBlocks, ",");
		}
	} 
	
	if (list_size(blocks) == 1)
	{
		char* aux2 = string_itoa((int)list_get(blocks, 0));
		string_append(&retBlocks, aux2); //CORREGIDO
		free(aux2);

	}

	string_append(&retBlocks, "]");
	return retBlocks;
}

void gcfsFreeBitmaps() {
	free(bitmap->bitarray);
	bitarray_destroy(bitmap);
}
/*
 * Liberamos el bloque
 * */
void liberar_block_line(t_blockLine* newLineBlock)
{
	free(newLineBlock);
}

/*
 * Creamos un new_pokemon en caso de que no exista en el FS
 * */
void createNewPokemon(new_pokemon* newPokemon)
{
	char* completePath = string_new();

	string_append(&completePath, struct_paths[FILES]);
	string_append(&completePath, newPokemon->nombrePokemon);

	int freeBlocks = liberar_blocks(metadata.blocks, bitmap);

	// Existe Pokemon
	if (access(completePath, F_OK) != -1)
	{
		log_info(logger, "Pokemon existe dentro del FileSystem!");
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

			list_add(freeBlocks, (void*) freeBlockPosition);

			char* metadataBlocks = formatToMetadataBlocks(freeBlocks);
			char* stringLength = string_itoa(pokemonPerPositionLength);
			char* pathBloque = obtener_path_nro_bloque(freeBlockPosition);
			
			FILE* blockFile = fopen(pathBloque,"wr");
			fwrite(pokemonPerPosition, 1 , pokemonPerPositionLength, blockFile);
			actualizar_pokemon_metadata(newPokemon->nombrePokemon, "N", stringLength, metadataBlocks, "N", "NEW_POKEMON");
			log_info(logger, "Operacion NEW_POKEMON %s, Coordenada: (%d, %d, %d) terminada correctamente", newPokemon->nombrePokemon, newPokemon->posicionEjeX, newPokemon->posicionEjeY, newPokemon->cantidad);
			
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
		  t_blockLine* newNode = crear_block_line(newPokemon->posicionEjeX, newPokemon->posicionEjeY, newPokemon->cantidad);
		  list_add(pokemonLines, (void*) newNode);

		  char* stringToWrite = block_lines_string(pokemonLines);
		  int blocksRequired = blocks_ocupados(stringToWrite);

		  if (freeBlocks > blocksRequired)
		  {
			char* stringLength = string_itoa(strlen(stringToWrite));
			t_list* listBlocks = requestFreeBlocks(blocksRequired);
			escribir_blocks(stringToWrite, listBlocks);
			char* metadataBlocks = formatToMetadataBlocks(listBlocks);
			actualizar_pokemon_metadata(newPokemon->nombrePokemon, "N", stringLength, metadataBlocks, "N", "NEW_POKEMON");
			log_info(logger, "Operacion NEW_POKEMON %s, Coordenada: (%d, %d, %d) terminada correctamente", newPokemon->nombrePokemon, newPokemon->posicionEjeX, newPokemon->posicionEjeY, newPokemon->cantidad);

			list_destroy(listBlocks);
			free(metadataBlocks);
		  }
		  else
		  {
			  log_error(logger, "No hay bloques disponibles. No se puede hacer la operacion");
		  }
		  
		  list_destroy_and_destroy_elements(pokemonLines, (void*) liberar_block_line);
		  free(stringToWrite);
	  	}

		free(super_path);
		free(pokemonDirectory);
		free(pokemonPerPosition);
	}
	
	free(completePath);
}

/*
 * Se chequea que el pokemon exista para luego leer las estructuras
 * */
int catchAPokemon(catch_pokemon* catchPokemon)
{
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
		log_error(logger, "No existe ese Pokemon en el FileSystem.");
	}

	free(completePath);

	return res;
}

/*
 * Si existe el pokemon dado, se leen las estructuras
 * */
t_list* getAPokemon(get_pokemon* getPokemon)
{
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
		log_error(logger, "No existe ese Pokemon en el FileSystem.");
		t_list* listaAux = list_create();
		res = listaAux; //TODO ERROR, se pierde info, CORREGIDO
	}

	free(completePath);
	return res;
}

/*
 * Se opera el new_pokemon, es decir, abrir el archivo y ver el contenido del mismo
 * */
void operateNewPokemonFile(new_pokemon* newPokemon, char* completePath, int freeBlocks)
{
	t_pokemon_metadata pokemonMetadata = leer_metadata_pokemon(completePath);

	pthread_mutex_lock(&MUTEX_LISTA_ARCHIVO_ABIERTO);
	pokemon_open_tad* pokemonOpenTad = dictionary_get(files_open, completePath);

	if(pokemonOpenTad == NULL)
	{
		pokemonOpenTad = new_pokemon_open_tad();
		dictionary_put(files_open, completePath, pokemonOpenTad);
	}

	pthread_mutex_unlock(&MUTEX_LISTA_ARCHIVO_ABIERTO);

	while(true)
	{
		//CORREGIDO contenido del if
		if(strcmp(pokemonMetadata.isOpen, "N") == 0) { //TODO ERROR INVALID READ OF SIZE 1
			log_info(logger, "El archivo no está abierto por ningun proceso, se procede a abrir el mismo...");

			pthread_mutex_lock(&pokemonOpenTad->mArchivo);
			updateOpenFileState(newPokemon->nombrePokemon, "Y", "NEW_POKEMON");
			pthread_mutex_unlock(&pokemonOpenTad->mArchivo);

			t_list* listBlocks = string_blocks_list(pokemonMetadata.blocks);
			t_list* pokemonLines = leer_pokemons(listBlocks);
			if (coordinateExists(newPokemon->posicionEjeX, newPokemon->posicionEjeY, pokemonLines) == 1) {
				addTotalPokemonIfCoordinateExist(newPokemon, pokemonLines);
			}
			else {
				t_blockLine* newNode = crear_block_line(newPokemon->posicionEjeX, newPokemon->posicionEjeY, newPokemon->cantidad);
				list_add(pokemonLines, (void*) newNode);
			}

			char* stringToWrite = block_lines_string(pokemonLines);
			int blocksRequired = blocks_ocupados(stringToWrite);
			char* stringLength = string_itoa(strlen(stringToWrite));
			
			if (freeBlocks > blocksRequired) {
				//necesito pedir bloques
				if (blocksRequired > list_size(listBlocks)) {
					int extraBlocksNeeded = blocksRequired - list_size(listBlocks);
					t_list* extraBlocks = requestFreeBlocks(extraBlocksNeeded);

					//agrego los nuevos bloques en la lista original
					list_add_all(listBlocks, (void*) extraBlocks);
					list_destroy(extraBlocks);
				}
				escribir_blocks(stringToWrite, listBlocks);
				char* metadataBlocks = formatToMetadataBlocks(listBlocks);

				pthread_mutex_lock(&pokemonOpenTad->mArchivo);
				actualizar_pokemon_metadata(newPokemon->nombrePokemon, "N", stringLength, metadataBlocks, "N", "NEW_POKEMON");
				pthread_mutex_unlock(&pokemonOpenTad->mArchivo);

				log_info(logger, "Operacion NEW_POKEMON %s, Coordenada (%d, %d, %d) terminada correctamente", newPokemon->nombrePokemon, newPokemon->posicionEjeX, newPokemon->posicionEjeY, newPokemon->cantidad);
				free(metadataBlocks);
			}
			else {
				log_error(logger, "No hay bloques disponibles, por lo tanto no se puede hacer la operacion..");
			}

			char* metadataBlocks = formatToMetadataBlocks(listBlocks);

			pthread_mutex_lock(&pokemonOpenTad->mArchivo);
			actualizar_pokemon_metadata(newPokemon->nombrePokemon, "N", stringLength, metadataBlocks, "N", "NEW_POKEMON");
			pthread_mutex_unlock(&pokemonOpenTad->mArchivo);

			list_destroy_and_destroy_elements(pokemonLines, (void*) liberar_block_line);
			free(stringToWrite);
			free(stringLength);
			//free(metadataBlocks);
			break;
		}
		else {
			log_info(logger, "Archivo abierto, se procede a reintentar luego de %d segundos. ", tiempoReintentoOperacion);
			log_info(logger, "Operacion NEW_POKEMON %s, Coordenada: (%d, %d, %d)", newPokemon->nombrePokemon, newPokemon->posicionEjeX, newPokemon->posicionEjeY, newPokemon->cantidad);
			sleep(tiempoReintentoOperacion);
			pokemonMetadata = leer_metadata_pokemon(completePath);
		}
	}

	free(pokemonMetadata.blocks); //TODO ERROR, corregido
	free(pokemonMetadata.isOpen);//TODO ERROR, corregido
	//free(pokemonMetadata);//CORREGIDO,
}

/*
 * Se opera el get pokemon, es decir se abre el archivo en caso de ser necesario
 * */
t_list* operateGetPokemonFile(get_pokemon* getPokemon, char* completePath)
{
	t_pokemon_metadata pokemonMetadata = leer_metadata_pokemon(completePath);
	t_list* res;

	pthread_mutex_lock(&MUTEX_LISTA_ARCHIVO_ABIERTO);
	pokemon_open_tad* pokemonOpenTad = dictionary_get(files_open, completePath);

	if(pokemonOpenTad == NULL){
		pokemonOpenTad = new_pokemon_open_tad();
		dictionary_put(files_open, completePath, pokemonOpenTad);
	}
	pthread_mutex_unlock(&MUTEX_LISTA_ARCHIVO_ABIERTO);

	while(true) {
		if (strcmp(pokemonMetadata.isOpen, "N")==0) {
			log_info(logger, "El archivo no esta abierto por ningun proceso, se procede a abrir el mismo..");

			pthread_mutex_lock(&pokemonOpenTad->mArchivo);
			updateOpenFileState(getPokemon->nombrePokemon, "Y", "GET_POKEMON");
			pthread_mutex_unlock(&pokemonOpenTad->mArchivo);

			t_list* listBlocks = string_blocks_list(pokemonMetadata.blocks);
			res = leer_pokemons(listBlocks);

			pthread_mutex_lock(&pokemonOpenTad->mArchivo);
			updateOpenFileState(getPokemon->nombrePokemon, "N", "GET_POKEMON");
			pthread_mutex_unlock(&pokemonOpenTad->mArchivo);

			log_info(logger, "Operacion GET_POKEMON %s terminada correctamente!", getPokemon->nombrePokemon);
			break;
		}
		else {
			log_info(logger, "Archivo abierto, se procede a reintentar luego de %d segundos..", tiempoReintentoOperacion);
			log_info(logger, "Operacion GET_POKEMON %s", getPokemon->nombrePokemon);
			sleep(tiempoReintentoOperacion);
			pokemonMetadata = leer_metadata_pokemon(completePath);
		}
	}

	free(pokemonMetadata.blocks); //TODO ERROR, CORREGIDO
	free(pokemonMetadata.isOpen);//TODO ERROR, CORREGIDO
	return res;
}

/*
 * Operar catch pokemon, se abre archivo y demás
 * */
int operateCatchPokemonFile(catch_pokemon* catchPokemon, char* completePath)
{
	t_pokemon_metadata pokemonMetadata = leer_metadata_pokemon(completePath);
	int res = 0;

	pthread_mutex_lock(&MUTEX_LISTA_ARCHIVO_ABIERTO);
	pokemon_open_tad* pokemonOpenTad = dictionary_get(files_open, completePath);

	if(pokemonOpenTad == NULL) {
		pokemonOpenTad = new_pokemon_open_tad();
		dictionary_put(files_open, completePath, pokemonOpenTad);
	}
	pthread_mutex_unlock(&MUTEX_LISTA_ARCHIVO_ABIERTO);

	while(true) {
		if (strcmp(pokemonMetadata.isOpen, "N")==0) {
			log_info(logger, "El archivo no está abierto por ningun proceso, por lo tanto se procede a abrir el mismo..");

			pthread_mutex_lock(&pokemonOpenTad->mArchivo);
			updateOpenFileState(catchPokemon->nombrePokemon, "Y", "CATCH_POKEMON");
			pthread_mutex_unlock(&pokemonOpenTad->mArchivo);

			t_list* listBlocks = string_blocks_list(pokemonMetadata.blocks);
			t_list* pokemonLines = leer_pokemons(listBlocks);

			if(coordinateExists(catchPokemon->posicionEjeX, catchPokemon->posicionEjeY, pokemonLines) == 1) {
				deletePokemonTotalIfCoordinateExist(catchPokemon, pokemonLines);

				char* stringToWrite = block_lines_string(pokemonLines);
				int blocksRequired = blocks_ocupados(stringToWrite);
				char* stringLength = string_itoa(strlen(stringToWrite));

				if (strlen(stringToWrite) != 0) {
					if (blocksRequired == list_size(listBlocks)) {
						escribir_blocks(stringToWrite, listBlocks);
						char* metadataBlocks = formatToMetadataBlocks(listBlocks);

						pthread_mutex_lock(&pokemonOpenTad->mArchivo);
						actualizar_pokemon_metadata(catchPokemon->nombrePokemon, "N", stringLength, metadataBlocks, "N", "CATCH_POKEMON");
						pthread_mutex_unlock(&pokemonOpenTad->mArchivo);

						free(metadataBlocks); //Todo ojo al piojo
					}
					
					if(blocksRequired < list_size(listBlocks)) {
						int lastBlockUsing = (int) list_get(listBlocks, list_size(listBlocks) - 1);
						list_remove(listBlocks, list_size(listBlocks) - 1);
						escribir_blocks(stringToWrite, listBlocks);
						char* metadataBlocks = formatToMetadataBlocks(listBlocks);

						pthread_mutex_lock(&pokemonOpenTad->mArchivo);
						actualizar_pokemon_metadata(catchPokemon->nombrePokemon, "N", stringLength, metadataBlocks, "N", "CATCH_POKEMON");
						pthread_mutex_unlock(&pokemonOpenTad->mArchivo);

						//Limpio las estructuras que ya no se usan..
						conf_liberar_bloq_in_pos(bitmap, lastBlockUsing);
						fclose(fopen(obtener_path_nro_bloque(lastBlockUsing), "w"));
						free(metadataBlocks);
					}
				}
				//si el pokemon tiene una sola linea, un solo bloque asignado y la unica coordenada es ==1
				//asumo que el bloque se queda ocupado pero con size = 0
				if (strlen(stringToWrite) == 0 && list_size(pokemonLines) == 0) {
					int blockUsed = (int) list_get(listBlocks, 0);
					char* metadataBlocks = formatToMetadataBlocks(listBlocks);
					char* zeroLength = string_itoa(0);
					
					pthread_mutex_lock(&pokemonOpenTad->mArchivo);
					actualizar_pokemon_metadata(catchPokemon->nombrePokemon, "N", zeroLength, "[]", "N", "CATCH_POKEMON");
					pthread_mutex_unlock(&pokemonOpenTad->mArchivo);
					
					//limpio structs q ya no se usan
					fclose(fopen(obtener_path_nro_bloque(blockUsed), "w"));
					log_info(logger, "Se procede a setear el bloque %d como Libre", blockUsed);
					conf_liberar_bloq_in_pos(bitmap, blockUsed);
					free(zeroLength);
					free(metadataBlocks);
				}
				res = 1;
				log_info(logger, "Operacion CATCH_POKEMON %s en la posicion (%d, %d) terminada correctamente", catchPokemon->nombrePokemon, catchPokemon->posicionEjeX, catchPokemon->posicionEjeY);

				free(stringToWrite);
				free(stringLength);
			}
			else {
				log_error(logger, "No existen las coordenadas para ese pokemon, no se puede completar la operacion..");

				pthread_mutex_lock(&pokemonOpenTad->mArchivo);
				updateOpenFileState(catchPokemon->nombrePokemon, "N", "CATCH_POKEMON");
				pthread_mutex_unlock(&pokemonOpenTad->mArchivo);
			}

			list_destroy_and_destroy_elements(pokemonLines, (void*) liberar_block_line);
			break;
		}
		else {
			log_info(logger, "Archivo abierto, se procede a reintentar luego de %d segundos", tiempoReintentoOperacion);
			log_info(logger, "Operacion CATCH_POKEMON %s, Coordenada: (%d, %d)", catchPokemon->nombrePokemon, catchPokemon->posicionEjeX, catchPokemon->posicionEjeY);
			sleep(tiempoReintentoOperacion);
			pokemonMetadata = leer_metadata_pokemon(completePath);
		}
	}

	free(pokemonMetadata.blocks); //TODO ERROR, CORREGIDO
	free(pokemonMetadata.isOpen); //TODO ERROR, CORREGIDO
	return res;
}

/*
 * Semaforo mutex para la sincronizacion de operacion de los distintos tipos de mensajes que llegan al gamecard
 * */
pokemon_open_tad* new_pokemon_open_tad() {
	pokemon_open_tad* pokemonOpenTad = malloc(sizeof(pokemon_open_tad));
	pthread_mutex_init(&pokemonOpenTad->mArchivo, NULL);
	return pokemonOpenTad;
}

//----------------------------GM_HANDLER------------------------------//
/*
 * Evalua si el ultimo caracter de string es char
 * */
int es_char(char* str, char chr)
{
	if ( ( str[strlen(str)-1]  == chr) ) return 1;
	return 0;
}

/*
 * Acomoda el path
 * */
int split_path(char* path, char** super_path, char** name)
{
	int aux;
	strcpy(*super_path, path);
	strcpy(*name, path);
	// Obtiene y acomoda el nombre del archivo.
	if (es_char(path, '/'))
	{
		(*name)[strlen(*name)-1] = '\0';
	}
	*name = strrchr(*name, '/');
	*name = *name + 1; // Acomoda el nombre, ya que el primer digito siempre es '/'

	// Acomoda el super_path
	if (es_char(*super_path, '/'))
	{
		(*super_path)[strlen(*super_path)-1] = '\0';
	}
	aux = strlen(*super_path) - strlen(*name);
	(*super_path)[aux] = '\0';

	return 0;
}

/*
 * Se chequea que el path tenga '/' correctamente
 *  */
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

/*
 * Obtiene el path que corresponde al bloque en el que se está trabajando
 * */
char* obtener_path_nro_bloque(int numeroDeBloque)
{
	char* path_del_bloque = malloc(strlen(puntoMontaje)+strlen("/Bloques")+20); //TODO ERROR
	sprintf(path_del_bloque,"%sBloques/%d.bin",puntoMontaje, numeroDeBloque);
	return path_del_bloque;
}

/*
 * Se lee el metadata del pokemon y la info que contiene
 * */
t_pokemon_metadata leer_metadata_pokemon(char* pokemonPath)
{
	char* existingPokemonMetadata = string_new();
	char* existingPokemonBlocks = string_new();

	t_pokemon_metadata metadata;

	string_append(&existingPokemonMetadata, pokemonPath);
	string_append(&existingPokemonMetadata, "/Metadata.bin");

	t_config* metadataFile = config_create(existingPokemonMetadata);
	metadata.blockSize = config_get_int_value(metadataFile, "SIZE");
	metadata.blocks = string_duplicate(config_get_string_value(metadataFile, "BLOCKS"));
	metadata.isOpen = string_duplicate(config_get_string_value(metadataFile, "OPEN"));
	config_destroy(metadataFile);

	free(existingPokemonMetadata);
	free(existingPokemonBlocks);

	//free(metadata.blocks);
	//free(metadata.isOpen);

	return metadata;
}

//---------------------BLOQUE_HANDLER----------------------//
/*
 * Devuelve cuantos bloques ocupados hay
 * */
int blocks_ocupados(char* value)
{
    int tamanio = string_length(value);
    return obtener_blocks(tamanio);
}

/*
 * Se calcula la cantidad de bloques que tiene
 * */
int obtener_blocks(int tamanio)
{
    // Redondea hacia arriba
    return 1 + ((tamanio - 1) / metadata.block_size);
}

/*
 * Escribir en bloques
 * */
void escribir_blocks(char* value, t_list* bloques)
{
    int limite = string_length(value);
    char* valorAGuardar = string_duplicate(value);

    for(int i = 0; i < list_size(bloques) && limite > 0; i++)
    {
        char* pathBloque = obtener_path_nro_bloque((int) list_get(bloques, i));

        FILE * bloque = fopen(pathBloque, "w+");

        int limiteSuperior;

        if(limite >= metadata.block_size)
        {
            limiteSuperior = metadata.block_size;
        }
        else
        {
            limiteSuperior = limite;
        }

        char* take = string_substring(valorAGuardar, 0, limiteSuperior);
        fwrite(take, 1, limiteSuperior, bloque);
        limite -= string_length(take);

        if(limite > 0)
        {
            valorAGuardar = string_substring_from(valorAGuardar, limiteSuperior);
        }

        fclose(bloque);
        free(pathBloque);
        free(take);
    }

    free(valorAGuardar);
}

/*
 * Se leen los contenidos de los bloques y retorna una lista con los contenidos leidos
 * */
t_list* leer_pokemons(t_list* blocks)
{
	t_list* retList = list_create(); //TODO BORRAR
	size_t len = 0;
	char* line = NULL;
	ssize_t read;
	FILE* blockFile;
	int isBreakFile = 0;
	char* previousLastLine = string_new(); //TODO BORRAR

	for (int i = 0; i < list_size(blocks); i++)
	{
		char* blockPath = string_new();
        string_append(&blockPath, struct_paths[BLOCKS]);
		char* aux3 = string_itoa((int)list_get(blocks, i));
        string_append(&blockPath, aux3); //TODO BORRAR, CORREGIDO
        free(aux3);
		string_append(&blockPath, ".bin");

		blockFile = fopen(blockPath, "r");

		if (blockFile == NULL)
		{
			log_error(logger, "No ha sido posible leer el archivo");
		}

		while((read = getline(&line, &len, blockFile)) != -1)
		{
			blockLine* blockLine;
			if(string_contains(line, "\n") == 0)
			{
				isBreakFile = 1;
				string_append(&previousLastLine, line);
			}
			else if(isBreakFile == 1)
			{
				string_append(&previousLastLine, line);
				blockLine = estructura_block_line(previousLastLine);
				isBreakFile = 0;
				free(previousLastLine);
				previousLastLine = string_new();// -------------------------------------------------------------
				list_add(retList, (void*)blockLine);
			}
			else
			{
				blockLine = estructura_block_line(line);
				list_add(retList, (void*)blockLine);
			}
		}

		free(blockPath);
		fclose(blockFile);
	}

	if (line) free(line);

	return retList;
}

/*
 * Dada [1,2,3,...] retorna una lista con los enteros que simbolizan un numero de bloque
 * */
t_list* string_blocks_list(char* blocks)
{
	t_list* retList = list_create(); //TODO BORRAR
	// Solo esta usando un bloque
	if (strlen(blocks) == 3)
	{
		char* blockStrWithoutBraces = string_substring(blocks, 1, 1); //TODO BORRAR, CORREGIDO
		list_add(retList, (void*)atoi(blockStrWithoutBraces));

		free(blockStrWithoutBraces);
	} // Mas de un bloque siendo usado
	else
	{
		char* blocksStrWithoutBraces = string_substring(blocks, 1, strlen(blocks) - 2); //TODO BORRAR, corregido
		char** blocksWithoutCommaSeparator = string_split(blocksStrWithoutBraces, ","); //TODO BORRAR, corregido

		int i = 0;
		while(blocksWithoutCommaSeparator[i] != NULL)
		{
			list_add(retList, (void*)atoi(blocksWithoutCommaSeparator[i]));
			i++;
		}

		//para liberar char**
		int cont = 0;
		while(blocksWithoutCommaSeparator[cont] != NULL){
			free(blocksWithoutCommaSeparator[cont]);
			cont++;
		}
		free(blocksWithoutCommaSeparator);//LO AGREGUE YO

		free(blocksStrWithoutBraces);//LO AGREGUE YO
	}

	return retList;
}

/*
 * Dada una linea "1-1=100\n" se devuelve la estructura correspondienta para poder manipularla
 * */
blockLine* estructura_block_line(char* blockline)
{
	blockLine* newLineBlock = malloc(sizeof(blockLine)); //TODO BORRAR, corregido
	char** splittedLine = string_split(blockline, "="); //TODO BORRAR, CORREGIDO
	char** coordinates = string_split(splittedLine[0], "-"); //TODO BORRAR, CORREGIDO
	newLineBlock->posX = atoi(coordinates[0]);
	newLineBlock->posY = atoi(coordinates[1]);
	newLineBlock->cantidad = atoi(splittedLine[1]);

	int cont1 = 0;
	while(splittedLine[cont1] != NULL){
		free(splittedLine[cont1]);
		cont1++;
	}
	free(splittedLine);

	int cont2 = 0;
	while(coordinates[cont2] != NULL){
		free(coordinates[cont2]);
		cont2++;
	}
	free(coordinates);

	return newLineBlock;
}

/*
 * Formatea una lista de blockLines al string final que se va a escribir "1-3=10\n1-3=50\n"
 * */
char* block_lines_string(t_list* pokemonLines)
{
	char* retChar = string_new();
	for(int j=0; j<list_size(pokemonLines); j++)
	{
		blockLine* newLineBlock = (blockLine*)list_get(pokemonLines, j);
		string_append(&retChar, formatear_block_lines(newLineBlock->posX, newLineBlock->posY, newLineBlock->cantidad));
	}
	return retChar;
}

/*
 * Formatea unas coordenadas y la cantidad a "1-1=100" string
 * */
char* formatear_block_lines(int intPosX, int intPosY, int intCantidad)
{
	char* pokemonPerPosition = string_new();
	char* posX = string_itoa(intPosX);
	char* posY = string_itoa(intPosY);
	char* cantidad = string_itoa(intCantidad);

	string_append(&pokemonPerPosition, posX);
	string_append(&pokemonPerPosition, "-");
	string_append(&pokemonPerPosition, posY);
	string_append(&pokemonPerPosition, "=");
	string_append(&pokemonPerPosition, cantidad);
	string_append(&pokemonPerPosition, "\n"); //TODO CORREGIR, corregido

	free(posX);
	free(posY);
	free(cantidad);



	//free(pokemonPerPosition);
	return pokemonPerPosition;
}

/*
 * Se crea un nuevo bloque con las coordenadas y cantidad
 * */
t_blockLine* crear_block_line(int intPosX, int intPosY, int intCantidad)
{
	t_blockLine* newLineBlock = malloc(sizeof(blockLine));
	newLineBlock->posX = intPosX;
	newLineBlock->posY = intPosY;
	newLineBlock->cantidad = intCantidad;
	return newLineBlock;
}

/*
 * Muestra al pokemon con sus coordenadas y cantidad
 * */
void mostrar_lista_pokemons(t_list* pokemonLines)
{
	log_info(logger, "Size lista %d:", list_size(pokemonLines));
	for (int i=0; i<list_size(pokemonLines); i++)
	{
		blockLine* newLineBlock = (blockLine*)list_get(pokemonLines, i);
		log_info(logger, "Elemento i %d:", i);
		log_info(logger, "Pokemon line %s", formatear_block_lines(newLineBlock->posX, newLineBlock->posY, newLineBlock->cantidad));
	}
}

/*
 * Chequea si el string a escribir entra en los bloques
 * */
bool entra_en_blocks(char* stringToWrite, t_list* listBlocks)
{
	int stringToWriteSize = strlen(stringToWrite);
	int spaceToAllocateString = list_size(listBlocks) * metadata.block_size;
	return stringToWriteSize <= spaceToAllocateString;
}

//-----------------------------GM_BITMAP------------------------------//
/*
 * Muestra el bitarray en la posicion
 * */
void mostrar_bitarray(t_bitarray* bitmap)
{
	for(int k =0;k<(bitarray_get_max_bit(bitmap));k++)
	printf("Test bit posicion, es  %d en posicion %d \n", bitarray_test_bit(bitmap,k),k);
}

/*
 * Setea el bloque ocupado en la posicion correspondiente
 * */
void conf_bloq_in_pos(t_bitarray* bitmap, off_t pos)
{
	bitarray_set_bit(bitmap, pos);
}

/*
 * Setea el bloque libre en posicion correspondiente
 * */
void conf_liberar_bloq_in_pos(t_bitarray* bitmap, off_t pos)
{
	bitarray_clean_bit(bitmap, pos);
}

/*
 * Testea en bloque libre en la posicion correspondiente
 * */
bool testear_bloq_lib_in_pos(t_bitarray* bitmap, int pos)
{
	return bitarray_test_bit(bitmap, (off_t)(pos));
}

/*
 * Obtiene y setea el proximo bloque libre
 * */
int obtener_conf_free_block(t_bitarray* bitmap, unsigned int blocks)
{
	int j;
	for(j =0; testear_bloq_lib_in_pos(bitmap, j); j++); // Hasta un bloque libre
	conf_bloq_in_pos(bitmap, j);
	log_info(logger, "Se procede a setear el bloque %d como Ocupado..", j);
	return j;
}

/*
 * Retorna la cantidad de bloques libres
 * */
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

//----------------------GM_METADATA SETUP--------------------------//

/*
 * Creacion de tall grass con las carpetas
 * */
void crear_root_files()
{
	char* dir_tallGrass = string_new();
	string_append(&dir_tallGrass, puntoMontaje);

	char* dir_metadata = string_new();
	string_append(&dir_metadata, puntoMontaje);
	string_append(&dir_metadata, "Metadata/");

	char* archivos = string_new();
	string_append(&archivos, puntoMontaje);
	string_append(&archivos, "Files/");

	char* dir_bloques = string_new();
	string_append(&dir_bloques, puntoMontaje);
	string_append(&dir_bloques, "Bloques/");

	if(_mkpath(puntoMontaje, 0755) == -1)
	{
		log_error(logger, "Error al crear la Carpeta...");
	}
	else
	{
		// Creo carpetas
		mkdir(dir_metadata, 0777);
		log_info(logger, "Creada carpeta Metadata/");

		mkdir(archivos, 0777);
		log_info(logger, "Creada carpeta Files/");
		log_info(logger, "Creada carpeta Files/ %s", dir_bloques);//es la de pokemons


		mkdir(dir_bloques, 0777);
		log_info(logger, "Creada carpeta Bloques/");
	}

	struct_paths[METADATA] = dir_metadata;
	struct_paths[FILES] = archivos;
	struct_paths[BLOCKS] = dir_bloques;
	struct_paths[TALL_GRASS] = dir_tallGrass;
}

/*
 * Creacion de metadata con archivos correspondientes, se crean bloques
 * */
void conf_metadata()
{
	char* metadataBin = string_new();
	char* bitmapBin = string_new();

	string_append(&metadataBin, struct_paths[METADATA]);
	string_append(&metadataBin, "Metadata.bin");

	if(access(metadataBin, F_OK) != -1)
	{
		leer_metadata(metadataBin);
    }
	else
	{
		crear_metadata_file(metadataBin);
        leer_metadata(metadataBin);
    }

	string_append(&bitmapBin, struct_paths[METADATA]);
	string_append(&bitmapBin, "Bitmap.bin");

	if(access(bitmapBin, F_OK) != -1)
	{
		leer_bitmap(bitmapBin);
    }
	else
	{
        // Creo bloques + bitmap
		crear_bitmap(bitmapBin);
		leer_bitmap(bitmapBin);
		crear_blocks();
    }

	free(metadataBin);
	free(bitmapBin);
}

/*
 * Se crea path de files/pokemon, y en caso de no tener pokemon, su metadata
 * */
void conf_files()
{
	char* pokemonBasePath = string_new();
	char* pokemonBaseBin = string_new();

	string_append(&pokemonBasePath, struct_paths[FILES]);
	string_append(&pokemonBasePath, "Pokemon/");

	struct_paths[POKEMON] = pokemonBasePath;

	mkdir(pokemonBasePath, 0777);

	string_append(&pokemonBaseBin, pokemonBasePath);
	string_append(&pokemonBaseBin, "Metadata.bin");

	FILE* pokemonMetadata = fopen(pokemonBaseBin, "w+b");
	t_config* pokemonConfigMetadata = config_create(pokemonBaseBin);

	config_set_value(pokemonConfigMetadata, "DIRECTORY", "Y");
	config_save(pokemonConfigMetadata);
	log_info(logger, "Creado directorio base /Pokemon y su Metadata.bin");

	fclose(pokemonMetadata);

	free(pokemonBasePath);
	free(pokemonBaseBin);
}

/*
 * Se crean los bloques
 * */
void crear_blocks()
{
	log_info(logger, "Creando bloques en el path /Bloques");
	FILE* newBloque;
	for(int i=0; i <= metadata.blocks-1; i++){
        char* pathBloque = obtener_path_nro_bloque(i);
        newBloque = fopen(pathBloque,"w+b");
        fclose(newBloque);
		free(pathBloque);
    }
}

/*
 * Se crea bitmap
 * */
void crear_bitmap(char* bitmap_bin){
	log_info(logger, "Creando el Bitmap.bin por primera vez...");
	bitmap_file = fopen(bitmap_bin, "wb+");
	char* bitarray = calloc(1, ceiling(metadata.blocks, 8));
	fwrite((void*)bitarray, ceiling(metadata.blocks, 8), 1, bitmap_file);
	fflush(bitmap_file);//devuelve EOF si ocurre escritura
	free(bitarray);
}

/*
 * Configuracion del sistema con los valores ya cargados
 * */
void crear_metadata_file(char* metadataBin)
{
	log_info(logger, "Creando Metadata.bin por primera vez...");
	FILE* metadata = fopen(metadataBin, "w+b");
	config_metadata = config_create(metadataBin);
	config_set_value(config_metadata, "BLOCK_SIZE", "64");
	config_set_value(config_metadata, "BLOCKS", "1024");
	config_set_value(config_metadata, "MAGIC_NUMBER", "TALL_GRASS");
	config_save(config_metadata);

	config_destroy(config_metadata);
	fclose(metadata);
}

/*
 * Se lee metadata con su contenido
 * */
void leer_metadata(char* metadataPath)
{
	log_info(logger, "Leyendo Metadata.bin ...");
	t_config* metadataFile = config_create(metadataPath);
	metadata.blocks = config_get_int_value(metadataFile,"BLOCKS");
	metadata.magic_number = string_duplicate(config_get_string_value(metadataFile,"MAGIC_NUMBER"));
	metadata.block_size = config_get_int_value(metadataFile,"BLOCK_SIZE");

	config_destroy(metadataFile);
}

/*
 * se valida el bitmap
 * */
void leer_bitmap(char* bitmapBin)
{
	log_info(logger, "Leyendo Bitmap.bin ...");
	bitmap_file = fopen(bitmapBin, "rb+");
	fseek(bitmap_file, 0, SEEK_END);
	int file_size = ftell(bitmap_file);
	fseek(bitmap_file, 0, SEEK_SET);
	char* bitarray_str = (char*) mmap(NULL, file_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fileno(bitmap_file), 0);
	if(bitarray_str == (char*) -1)
	{
		log_error(logger, "Fallo el mmap: %s", strerror(errno));
	}
	fread((void*) bitarray_str, sizeof(char), file_size, bitmap_file);
	bitmap = bitarray_create_with_mode(bitarray_str, file_size, MSB_FIRST);
}

