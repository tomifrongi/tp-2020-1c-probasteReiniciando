#include "gm_metadata.h"

//Creacion de tall grass
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
		log_error(logger, "_mkpath");
	}
	else
	{
		// Creo carpetas
		mkdir(dir_metadata, 0777);
		log_info(logger, "Creada carpeta Metadata/");
		mkdir(archivos, 0777);
		log_info(logger, "Creada carpeta Files/");
		log_info("Creada carpeta Files/ %s", dir_bloques);
		mkdir(dir_bloques, 0777);
		log_info(logger, "Creada carpeta Bloques/");
	}

	struct_paths[METADATA] = dir_metadata;
	struct_paths[FILES] = archivos;
	struct_paths[BLOCKS] = dir_bloques;
	struct_paths[TALL_GRASS] = dir_tallGrass;
}

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

void crear_bitmap(char* bitmapBin)
{
	log_info(logger, "Creando el Bitmap.bin por primera vez");
	bitmap_file = fopen(bitmapBin, "wb+");
	char* bit_arr = malloc(/*1, */ceiling(metadata.blocks, 8));
	fwrite((void*) bit_arr, ceiling(metadata.blocks, 8), 1, bitmap_file);
	fflush(bitmap_file);
	free(bit_arr);
}

void crear_metadata_file(char* metadataBin)
{
	log_info(logger, "Creando Metadata.bin por primera vez");
	FILE* metadata = fopen(metadataBin, "w+b");
	config_metadata = config_create(metadataBin);
	config_set_value(config_metadata, "BLOCK_SIZE", "64");
	config_set_value(config_metadata, "BLOCKS", "4096"); // asi no tengo 5492 bloques
	config_set_value(config_metadata, "MAGIC_NUMBER", "TALL_GRASS");
	config_save(config_metadata);
	config_destroy(config_metadata);

	fclose(metadata);
}

void leer_metadata(char* metadataPath)
{
	log_info(logger, "Leyendo Metadata.bin");
	t_config* metadataFile = config_create(metadataPath);
	metadata.blocks = config_get_int_value(metadataFile,"BLOCKS");
	metadata.magic_number = string_duplicate(config_get_string_value(metadataFile,"MAGIC_NUMBER"));
	metadata.block_size = config_get_int_value(metadataFile,"BLOCK_SIZE");
	config_destroy(metadataFile);
}

void leer_bitmap(char* bitmapBin)
{
	log_info(logger, "Leyendo Bitmap.bin");
	bitmap_file = fopen(bitmapBin, "rb+");
	fseek(bitmap_file, 0, SEEK_END);
	int file_size = ftell(bitmap_file);
	fseek(bitmap_file, 0, SEEK_SET);
	char* bitarray_str = (char*) mmap(NULL, file_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fileno(bitmap_file), 0);
	if(bitarray_str == (char*) -1)
	{
		log_error(logger, "Fallo el mmap: %s");
	}
	fread((void*) bitarray_str, sizeof(char), file_size, bitmap_file);
	bitmap = bitarray_create_with_mode(bitarray_str, file_size, MSB_FIRST);
}

