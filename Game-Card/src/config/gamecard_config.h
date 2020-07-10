#ifndef CONFIG_gamecard_CONFIG_H_
#define CONFIG_gamecard_CONFIG_H_

#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include "../funcionesEnvio.h"

#define CONFIG_FILE_PATH "GameCard.config"

typedef struct
{
	int tiempo_de_reintento_conexion;
	int tiempo_de_reintento_operacion;
	int tiempo_retardo_operacion;
	char* punto_montaje_tallgrass;
	char* ip_broker;
	int puerto_broker;
	char* ip_game_card;
	int puerto_game_card;
} t_gamecard_config;

t_log* log;

t_gamecard_config* gamecard_config;

void init_log();
void init_config();
void gamecard_config_free();
void read_config(t_config* config_file);
void print_config();

#endif /* CONFIG_gamecard_CONFIG_H_ */
