#ifndef UTILS_MEMORIA_H_
#define UTILS_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <shared-2.h>
#include <pthread.h>
#include <commons/bitarray.h>
#include <stdbool.h>
#include <commons/string.h>

typedef struct {
	char *puerto_escucha;
	int tam_memoria;
	int tam_segmento_0;
	int cant_segmentos;
	int retardo_memoria;
	int retardo_compactacion;
	char *algoritmo_asignacion;
} t_config_memoria;

extern t_config_memoria config_memoria;
extern t_log *logger;
extern void *memoria;
extern t_list *tablas_segmentos;
extern t_bitarray *bitmap;
extern t_segmento *segmento_0;
extern t_tabla_segmentos *tabla_segmentos;

#endif /* UTILS_MEMORIA_H_ */
