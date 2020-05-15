
#ifndef GAMEBOY_H_
#define GAMEBOY_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include "commons/string.h";
#define MAX_CLIENTS 128



int connect_to_server(char* host, int port, void*(*callback)());
int create_socket();

#endif /* GAMEBOY_H_ */
