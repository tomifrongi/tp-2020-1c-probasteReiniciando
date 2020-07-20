#ifndef GAMECARD_H_
#define GAMECARD_H_

#define LOCAL_IP "127.0.0.1"
#define LOCAL_PORT 5001

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
#include <pthread.h>
#include <sys/socket.h>

#include "commons/config.h"
#include "commons/log.h"
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <commons/collections/queue.h>

#include "ProtocoloDeMensajes.h"
#include "funcionesEnvio.h"
#include "gm_filesystem.h"
#include "mensajes.h"

#define MAX_CLIENTS 128

//------------------------------------ESTRUCTURAS--------------------------------------------//

bool is_conn;

//HANDLE CONNECTION//
typedef struct{
	char* valor;
	uint32_t fd;
}t_handle_connection;


//POSICIONES//
typedef struct {
	uint32_t posicionEjeX;
	uint32_t posicionEjeY;
}t_posiciones;

//CONEXIONES//
int tiempoReintentoConexion;
int tiempoReintentoOperacion;
int tiempoRetardoOperacion;
char* puntoMontaje;
char* ipBroker;
int puertoBroker;

//SUSCRIPCION//
typedef struct {
	uint32_t ip;
	uint32_t puerto;
	t_queue* cola;
}t_subscribe;

//-----------------------------------FUNCIONES DECLARADAS-------------------------------------------//

void reintentar_conexion(void* arg);
void iniciar_config_logger();
void* handler_suscripciones(uint32_t cola);
void gm_init();
void *handle_conexion_server(void *arg);
void *recibir_msgs_gamecard(int fd, int send_to);
void enviar_ack(void* arg);
void procesar_new_enviar_appeared(void* arg);
void procesar_get_enviar_localized(void* arg);
void procesar_catch_enviar_caught(void* arg);
void gm_exit();


#endif


