
#ifndef ENTRENADORES_H_
#define ENTRENADORES_H_


typedef enum{
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT
}t_state;


 typedef struct {
	char* nombre;
	int id;
	t_pokemon* pokemonesBuscados;
	t_state estado = NEW;
}t_entrenador;


int cambiar_estado(t_entrenador*,t_state);

void bloquear_entrenador(t_entrenador*);

void capturar_pokemon(t_entrenador*,t_pokemon*);

int cargar_objetivos(t_entrenador*);

int cantidad_objetivos(t_entrenador*);

int puede_atrapar(t_entrenador*,t_pokemon*);

int necesita_pokemon(t_entrenador*,t_pokemon*);

#endif /* ENTRENADORES_H_ */

