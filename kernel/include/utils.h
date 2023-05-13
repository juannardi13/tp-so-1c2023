#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>
#include"../../shared/include/main.h"

#define PUERTO "9120"

t_log* iniciar_logger(void);
t_config* iniciar_config(char*);
void iterator(char* value);
bool generar_conexiones(t_config*, t_log*, int*, int*, int*);
void agregar_instruccion_a_lista(char ** lista, char* instruccion);
t_instruccion recibir_instruccion(t_log* logger, int conexion_consola);
t_pcb crear_pcb(t_instruccion* instrucciones);

#endif /* UTILS_H_ */
