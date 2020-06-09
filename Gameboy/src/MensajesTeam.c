
#include "MensajesTeam.h"

void* serializarAppearedContentTeam (appeared_pokemon_team parametros){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(uint32_t)*5+parametros.sizeNombre);

	memcpy (content + bytes_escritos, &parametros.id_mensaje, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros.idCorrelativo, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros.sizeNombre, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, parametros.nombrePokemon, parametros.sizeNombre);
	bytes_escritos += parametros.sizeNombre;
	memcpy (content + bytes_escritos, &parametros.posicionEjeX, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);
	memcpy (content + bytes_escritos, &parametros.posicionEjeY, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);


	return content;

}
