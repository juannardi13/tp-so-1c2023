#ifndef UTILS_FILESYSTEM_H_
#define UTILS_FILESYSTEM_H_

#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <commons/string.h>
#include <sys/mman.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

void reducir_tamanio_cuando_tam_actual_mayor_tam_bloque_y_nuevo_tam_menor_tam_bloque(int,int);
void reducir_tamanio_cuando_tam_actual_mayor_tam_bloque_y_nuevo_tam_mayor_tam_bloque(int,int,int);
void ampliar_con_tam_actual_cero_y_tam_nuevo_menor_igual_tam_bloque(t_config*);
void ampliar_con_tam_actual_cero_y_tam_nuevo_mayor_tam_bloque(int ,t_config*);
void ampliar_con_tam_actual_menor_tam_bloque_tam_nuevo_mayor_tam_bloque(int,t_config*);
void ampliar_con_tam_actual_mayor_tam_bloque(int, int,t_config*);

#endif /* UTILS_FILESYSTEM_H_ */
