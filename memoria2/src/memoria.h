#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "utils_memoria.h"
#include "operaciones.h"
#include "inicio.h"

t_config *config;
t_config_memoria config_memoria;
t_log *logger;
int *conexiones[3];
pthread_t hilos[3];
void *memoria;
t_list *tablas_segmentos;
t_bitarray *bitmap;
t_segmento *segmento_0;

void inicializar_config(char *);
int esperar_conexiones(int);
void terminar_memoria(void);
void destruir_bitmap(void);
void manejar_conexion(int*);


#endif /* MEMORIA_H_ */
