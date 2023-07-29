#ifndef OPERACIONES_H_
#define OPERACIONES_H_

#include "utils_memoria.h"
#include "segmentacion.h"

void atender_File_System(int *);
void atender_kernel(int *);
void atender_CPU(int*);
void recv_nuevo_proceso(int, int);
void send_tabla(int, t_tabla_segmentos *);
void recv_crear_segmento(int, void *);
void send_base_segmento(int, int);
void recv_eliminar_segmento(int, void *);
void compactar(int);
void send_tablas(int);
void recv_liberar_estructuras(int, void *);
void recv_leer_de_memoria(int, int, int);
void recv_leer_de_memoria_cpu(int, int, int);
void recv_escribir_en_memoria(int, int, char*, int);
void buscar_valor_enviar(int, int, int);
void buscar_valor_enviar_cpu(int, int, int);
void escribir_valor_en_direccion_fisica(int, int, int, char*);
int agregar_a_stream(void*, int*, void*, int);
void enviar_string_por(op_code, char*, int);

#endif /* OPERACIONES_H_ */
