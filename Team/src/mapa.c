#include"mapa.h"



double map_distancia(int pos1_x,int pos1_y,int pos2_x,int pos2_y){//distancia entre coordenadas ,no va a ser la distancia total que se recorrera,por las limitaciones de recorrido en no diagonal
	return  sqrt(pow(pos1_x-pos2_x,2)+pow(pos1_y-pos2_y,2));
}
double distancia_entrenador_pokemon(t_entrenador*entrenador,t_pokemon*pokemon){
	return distancia(entrenador->posicion_x,entrenador->posicion_y,entrenador->posicion_x,pokemon->posicion_y);

}
/*
bool mapa_hay_pokemon_suelto(void *mapa[][]) {
	return list_size(pokemones_sueltos) > 0;
}*/
bool mapa_hay_pokemon_suelto(t_team*team) {
	return list_size(team->pokemones_sueltos) > 0;
}

t_entrenador*entrenador_mas_cercano(t_list*entrenadores,t_pokemon*pokemon){

	list_filter(entrenadores,)
	 t_entrenador*entrenador_cercano=list_get(entrenadores,0);
	 for (int i=1;i<list_size(entrenadores);i++){
		 if (distancia_entrenador_pokemon(list_get(entrenadores,i),pokemon)<distancia_entrenador_pokemon(entrenador_cercano,pokemon)){
			 entrenador_cercano=list_get(entrenadores,i);
		 }
	 }

return entrenador_cercano;
}
