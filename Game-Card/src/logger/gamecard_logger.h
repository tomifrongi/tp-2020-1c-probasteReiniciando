#ifndef LOGGER_GAMECARD_LOGGER_H_
#define LOGGER_GAMECARD_LOGGER_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/string.h>
#include <commons/log.h>
//#include "../../../shared-common/common/logger.h"

#define LOG_FILE "/home/utnso/log_gamecard.txt"
#define PROGRAM_NAME "GAME_CARD"

int  gamecard_logger_create();
void gamecard_logger_info(char* message, ...);
void gamecard_logger_warn(char* message, ...);
void gamecard_logger_error(char* message, ...);
void gamecard_logger_destroy();
t_log* gamecard_log_get();

t_log* gamecard_log;
#endif /* LOGGER_GAME_CARD_LOGGER_H_ */
