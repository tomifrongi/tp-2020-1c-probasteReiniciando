
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

void agregar_pokemon(t_entrenador* entrenador,t_pokemon* pokemon);
void mostrar_pokemones(t_entrenador* entrenador);
//int necesita_pokemon(t_entrenador* entrenador,t_pokemon* pokemon);

#endif /* TEAM_H_ */
