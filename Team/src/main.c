#include "main.h"

int main(){
	cargar_configuracion();
	inicializar_logger();
	t_team* team = malloc(sizeof(t_team));

	team->cantidad_cambios_de_contexto = 0;
	team->hubo_deadlocks = false;
	team->conectado_al_broker = false;
	team->entrenadores = inicializar_entrenadores(team);
	team->entrenadores_desocupados = list_create();
	list_add_all(team->entrenadores_desocupados,team->entrenadores);
	team->entrenadores_ready = list_create();
	team->hilos_entrenadores = list_create();
	team->mapa_pokemones = list_create();


	team->objetivo_pokemones_restantes = obtener_objetivo_pokemones_restantes(team->entrenadores);

	if(strcmp(ALGORITMO_PLANIFICACION,"FIFO")==0)
		team->planificador = FIFO;
	else if(strcmp(ALGORITMO_PLANIFICACION,"RR")==0)
		team->planificador = RR;
	else if(strcmp(ALGORITMO_PLANIFICACION,"SJF-CD")==0)
		team->planificador = SJF_CD;
	else if(strcmp(ALGORITMO_PLANIFICACION,"SJF-SD")==0)
		team->planificador = SJF_SD;

	//mostrar_entrenadores(team->entrenadores);


	planificar_team(team);

	return 0;
}
