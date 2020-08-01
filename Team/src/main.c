#include "main.h"

int main(){
	cargar_configuracion();
	inicializar_logger();
	t_team* team = malloc(sizeof(t_team));
	//TODO PONER TOOODOS LOS LOGS
	team->cantidad_ciclos_cpu_ejecutados = 0;
	team->cantidad_deadlocks = 0;
	team->cantidad_deadlocks_detectados = 0;
	team->cantidad_deadlocks_solucionados = 0;
	team->conectado_al_broker = false;
	team->entrenadores = inicializar_entrenadores(team);
	team->entrenadores_desocupados = list_create();
	list_add_all(team->entrenadores_desocupados,team->entrenadores);
	team->entrenadores_ready = list_create();
	team->hilos_entrenadores = list_create();
	team->mapa_pokemones = list_create();
	//mostrar_entrenadores(team->entrenadores);

	team->objetivo_pokemones_restantes = obtener_objetivo_pokemones_restantes(team->entrenadores);
	if(strcmp(ALGORITMO_PLANIFICACION,"FIFO")==0)
		team->planificador = FIFO;
	else if(strcmp(ALGORITMO_PLANIFICACION,"RR")==0)
		team->planificador = RR;
	else if(strcmp(ALGORITMO_PLANIFICACION,"SJF-CD")==0)
		team->planificador = SJF_CD;
	else if(strcmp(ALGORITMO_PLANIFICACION,"SJF-SD")==0)
		team->planificador = SJF_SD;



	planificar_team(team);

	return 0;
}
