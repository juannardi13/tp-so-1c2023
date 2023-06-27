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

extern t_registros* registros;

void asignar_valor_a_registro(char*, char*, t_registros*);
void activar_segmentation_fault(t_contexto_de_ejecucion*, int);
bool desplazamiento_supera_tamanio(int, char*);
char* deserializar_paquete_de_memoria(t_buffer*);
void deserializar_segmentos(t_segmento*, void*);
void decode_instruccion(char*, t_contexto_de_ejecucion*, t_config*, int, int, bool);
void ejecutar_SET(char**, t_contexto_de_ejecucion*);
void ejecutar_MOV_IN(char**, t_contexto_de_ejecucion*, int, t_config*);
void ejecutar_MOV_OUT(char**, t_contexto_de_ejecucion*, int, t_config*);
void ejecutar_IO(char**, t_contexto_de_ejecucion*, int, bool);
void ejecutar_F_OPEN(char**, t_contexto_de_ejecucion*, int);
void ejecutar_F_CLOSE(char**, t_contexto_de_ejecucion*, int);
void ejecutar_F_SEEK(char**, t_contexto_de_ejecucion*, int);
void ejecutar_F_READ(char**, t_contexto_de_ejecucion*, int, int, t_config*);
void ejecutar_F_WRITE(char**, t_contexto_de_ejecucion*, int, int, t_config*);
void ejecutar_F_TRUNCATE(char**, t_contexto_de_ejecucion*, int);
void ejecutar_WAIT(char**, t_contexto_de_ejecucion*, int);
void ejecutar_CREATE_SEGMENT(char**, t_contexto_de_ejecucion*, int);
void ejecutar_DELETE_SEGMENT(char**, t_contexto_de_ejecucion*, int);
void ejecutar_YIELD(char**, t_contexto_de_ejecucion*, int);
void ejecutar_EXIT(char** instruccion, t_contexto_de_ejecucion*, int);
void escribir_en_memoria(int, char*, int);
char* fetch_instruccion(t_contexto_de_ejecucion*);
t_log* iniciar_logger(void);
void inicializar_registros(t_registros*);
t_config* iniciar_config(void);
char* leer_de_memoria(int, t_config*, int);
char* mmu_valor_buscado(t_contexto_de_ejecucion*, int, int, t_config*);
int obtener_direccion_fisica(int, int, t_config*, t_contexto_de_ejecucion*);
void recibir_contexto(int, t_contexto_de_ejecucion*);
void serializar_contexto(t_contexto_de_ejecucion*, t_buffer*, void*, int);
void serializar_segmentos(t_segmento*, void*, int);
#endif /* UTILS_H_ */
