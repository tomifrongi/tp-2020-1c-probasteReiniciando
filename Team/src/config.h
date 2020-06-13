
#include<commons/config.h>
#include<commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include<commons/string.h>
#include "entrenadores.h"
#ifndef CONFIG_H_
#define CONFIG_H_

void separa_valores(char*string_valores, t_list * ptr_valores) ;
t_list*array_to_list(char**array);
int largo_array(char**array);
t_config * leer_config(t_team* team);
int cantidad_entrenadores(t_team*team);
#endif /* CONFIG_H_ */
