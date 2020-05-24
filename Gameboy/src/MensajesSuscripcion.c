
#include "MensajesSuscripcion.h"


void* serializarSuscripcionContent(id_cola id){
	int bytes_escritos = 0;
	void* content = malloc(sizeof(suscripcion));

	memcpy (content + bytes_escritos, &id, sizeof(uint32_t));
	bytes_escritos += sizeof (uint32_t);

	return content;
}

char* obtenerNombreCola(id_cola id){

	switch(id){
	case NEW:
		return "NEW";
		break;
	case APPEARED:
		return "APPEARED";
		break;
	case GET:
		return "GET";
		break;
	case LOCALIZED:
		return "LOCALIZED";
		break;
	case CATCH:
		return "CATCH";
		break;
	case CAUGHT:
		return "CAUGHT";
		break;
	}
return "error";
}
