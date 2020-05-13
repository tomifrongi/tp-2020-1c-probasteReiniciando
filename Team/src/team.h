

#ifndef TEAM_H_
#define TEAM_H_

typedef enum{
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT
}t_state;

typedef struct{
	char* especie;
	struct t_pokemon* sgte;
}t_pokemon;

 typedef struct {
	char* nombre;
	int id;
	t_pokemon* pokemonesBuscados;
	int estado;
}t_entrenador;




typedef struct{
	char* nombre;
	t_entrenador* entrenadores;
	t_pokemon* objetivoGlobal;
}t_team;

typedef struct{
	t_pokemon pokemon;
	int cantidad;
	struct t_objetivo* sgte;
}t_objetivo;

void agregar_pokemon(t_entrenador* ,t_pokemon* );
void mostrar_pokemones(t_entrenador* );
t_objetivo* objetivo_global(t_team*);
int cantidad_por_especie(t_team*,t_pokemon*);
void crear_hilo(t_entrenador*);


#endif /* TEAM_H_ */
