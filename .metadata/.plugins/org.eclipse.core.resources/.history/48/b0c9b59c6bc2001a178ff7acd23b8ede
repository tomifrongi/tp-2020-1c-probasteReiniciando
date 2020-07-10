#include "gamecard_logger.h"

int gamecard_logger_create()
{
	gamecard_log = logger_create(LOG_FILE, PROGRAM_NAME);
	if (gamecard_log == NULL || gamecard_log < 0)
	{
		perror("No ha sido posible instanciar el game_card_logger");
		return -1;
	}

	logger_print_header(gamecard_log, PROGRAM_NAME);

	return 0;
}

void gamecard_logger_info(char* message, ...)
{
	va_list arguments;
	va_start(arguments, message);
	char* formated_message = string_from_vformat(message, arguments);
	log_info(gamecard_log, formated_message);
	free(formated_message);
	va_end(arguments);
}

void gamecard_logger_warn(char* message, ...)
{
	va_list arguments;
	va_start(arguments, message);
	char* formated_message = string_from_vformat(message, arguments);
	log_warning(gamecard_log, formated_message);
	free(formated_message);
	va_end(arguments);
}

void gamecard_logger_error(char* message, ...)
{
	va_list arguments;
	va_start(arguments, message);
	char* formated_message = string_from_vformat(message, arguments);
	log_error(gamecard_log, formated_message);
	free(formated_message);
	va_end(arguments);
}

void gamecard_logger_destroy()
{
	logger_print_footer(gamecard_log, PROGRAM_NAME);
	logger_destroy(gamecard_log);
}

t_log* gamecard_log_get()
{
	return gamecard_log;
}
