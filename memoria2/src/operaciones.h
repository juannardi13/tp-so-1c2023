#ifndef OPERACIONES_H_
#define OPERACIONES_H_

#include "utils_memoria.h"
#include "segmentacion.h"

void atender_File_System(int *);
void atender_kernel(int *);
void recv_nuevo_proceso(int);
void send_tabla(int, t_tabla_segmentos *);
void recv_crear_segmento(int);
void send_base_segmento(int, int);
void recv_eliminar_segmento(int);
void recv_leer_de_memoria(int);
void recv_escribir_en_memoria(int);
void buscar_valor_enviar_CPU(int, int, int);
void escribir_valor_en_direccion_fisica(int, int, char*);
#endif /* OPERACIONES_H_ */
