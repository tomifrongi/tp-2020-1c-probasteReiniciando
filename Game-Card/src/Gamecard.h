#ifndef GAMECARD_H_
#define GAMECARD_H_

#define LOCAL_IP "127.0.0.1"
#define LOCAL_PORT 5001

#include "commons/config.h"
#include "commons/log.h"
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include "ProtocoloDeMensajes.h"
#include <pthread.h>
#include "funcionesEnvio.h"
#include "time.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdbool.h>
#include "gm_filesystem.h"
#include <sys/queue.h>
#include <sys/socket.h>


#define MAX_CLIENTS 128

bool is_conn;

typedef struct{
	int cord_x;
	char guion;
	int cord_y;
	char igual;
	int cantidad;
}t_linea;

typedef struct{
	uint32_t block_size;
	uint32_t blocks;
	char* magic_number;
}t_principal;

typedef struct{
	char* bool_val;
	uint32_t fd;
}t_handle_connection;

typedef struct {
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}t_posiciones;
//-----------------------------------------CONEXIONES-------------------------------------------------//
int tiempoReintentoConexion;
int tiempoReintentoOperacion;
int tiempoRetardoOperacion;
char* puntoMontaje;
char* ipBroker;
int puertoBroker;
//----------------------------------------------------------------------------------------------------//

void* handler_suscripciones(uint32_t cola);
void initConfigLogger();
void gm_retry_connect(void* arg);
void game_card_init_as_server();
void *recv_gamecard(int fd, int send_to);
void game_card_exit();
//void subscribe_to(void *arg);
//static void *handle_conn(void *arg);
void enviar_ack(void* arg);
void process_new_and_send_appeared(void* arg);
void process_get_and_send_localized(void* arg);
void process_catch_and_send_caught(void* arg);

#endif
/* GAMECARD_H_ */



