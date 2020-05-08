
#ifndef TEAM_H_
#define TEAM_H_

typedef struct{
	char* especie;
	struct t_pokemon* sgte;
}t_pokemon;

 typedef struct {
	char* nombre;
	t_pokemon* pokemonesBuscados;
}t_entrenador;

typedef struct{
	char* nombre;
	t_entrenador* entrenadores;
	t_pokemon* objetivoGlobal;
}t_team;

void agregar_pokemon(t_entrenador* ,t_pokemon* );
void mostrar_pokemones(t_entrenador* );
t_pokemon* objetivo_global(t_team*);
//int necesita_pokemon(t_entrenador* ,t_pokemon*);

#endif /* TEAM_H_ */
