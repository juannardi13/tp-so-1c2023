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
#include<commons/collections/queue.h>
#include<commons/string.h>
#include<string.h>
#include<assert.h>
#include <pthread.h>
#include <shared-2.h>

#define PUERTO "9120"

int socket_cpu;

t_log* iniciar_logger(void);
t_config* iniciar_config(char*);
void iterator(char* value);
bool generar_conexiones(t_config*, t_log*, int*, int*, int*);
void agregar_instruccion_a_lista(char ** lista, char* instruccion);
t_instruccion recibir_instruccion(t_log* logger, int conexion_consola);
t_pcb crear_pcb(t_instruccion* instrucciones);
int server_escuchar(t_log* logger, char* server_name, int server_socket);
t_pcb* armar_pcb(char** lista_instrucciones, t_log* logger);
t_pcb * alocar_memoria_pcb(t_log* logger);
void manejar_conexion(void*);
int atender_clientes_kernel(int, t_log*);
t_list *deserializar_instrucciones(t_list*, int);
t_consola *deserializar_consola(char*, t_log*);
t_pcb* crear_estructura_pcb(t_consola*);
void iniciar_planificador_largo_plazo(void);
void iniciar_planificador_mediano_plazo(void);
void agregar_pcb_a_new(t_proceso* proceso, t_log* logger);
void mostrar_cola_new(t_list* lista, t_log* logger);
void recibir_instruccion_serializada(int socket_cliente);
char* recibir_instrucciones_como_string(int socket_cliente, t_log* logger);

#endif /* UTILS_H_ */
