#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <shared-2.h>

t_log* iniciar_logger(void);
t_config* iniciar_config(void);
void decode_instruccion(char*, t_contexto_de_ejecucion*, t_config*, int, int, bool);
char* fetch_instruccion(t_contexto_de_ejecucion*);
void asignar_valor_a_registro(char*, char*, t_registros*);
void activar_segmentation_fault(t_contexto_de_ejecucion*);
bool desplazamiento_supera_tamanio(int, char*);
char* leer_de_memoria(int, t_config*, int);
int obtener_direccion_fisica(int, int, t_config*, t_contexto_de_ejecucion*);
void escribir_en_memoria(int, char*, int);
char* mmu_valor_buscado(t_contexto_de_ejecucion*, int, int, t_config*);
void ejecutar_SET(char**, t_contexto_de_ejecucion*);
//void ejecutar_MOV_IN(char**, t_contexto_de_ejecucion*, int, t_config*);
//void ejecutar_MOV_OUT(char**, t_contexto_de_ejecucion*, int, t_config*);
//void ejecutar_IO(char**, t_contexto_de_ejecucion*, int, bool);

#endif /* UTILS_H_ */
