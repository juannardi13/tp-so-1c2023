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

typedef struct
{
	op_code nombre;
	int parametro_1_length;
	char* parametro_1;
	int parametro_2_length;
	char* parametro_2;
	int parametro_3_length;
	char* parametro_3;
}t_instruccion;

//Variables globales:
int socket_cpu;
int pid_global = 1000;
t_consola* consola;
t_list* cola_new;
t_list* cola_ready;

//Estructura para pasarle a la función de los hilos:
typedef struct {
    t_log* log;
    int fd;
} t_manejar_conexion_args;

//Funciones que crean las estructuras(Logger, Config)
t_log* iniciar_logger(void);
t_config* iniciar_config(char*);

//Funciones sobre conexiones
bool generar_conexiones(t_config*, t_log*, int*, int*, int*);
t_instruccion recibir_instruccion(t_log* logger, int conexion_consola);

//Funciones para tratar con la PCB
void agregar_pcb_a_new(t_proceso* proceso, t_log* logger);
t_pcb * alocar_memoria_pcb(t_log* logger);
t_pcb* armar_pcb(char** lista_instrucciones, t_log* logger);
t_pcb* crear_estructura_pcb(t_consola*);
t_pcb crear_pcb(t_instruccion* instrucciones);
t_pcb* deserializar_pcb(t_buffer*);
void enviar_pcb(int, t_pcb*);
t_pcb* recibir_pcb(int);

//Funciones de clientes
int atender_clientes_kernel(int, t_log*);
void manejar_conexion(void*);
int server_escuchar(t_log* logger, char* server_name, int server_socket);

//Serialización y deserialización de datos
void agregar_instruccion_a_lista(char ** lista, char* instruccion);
t_consola *deserializar_consola(char*, t_log*);
t_list *deserializar_instrucciones(t_list*, int);
char* deserializar_string(t_buffer* buffer);
void recibir_instruccion_serializada(int socket_cliente);
char* recibir_instrucciones_como_string(int socket_cliente, t_log* logger);

//Manejo de procesos
void agregar_proceso_a_ready(t_log* logger);
void ejecutar_proceso(int socket_cpu, t_log* logger);
void mostrar_cola_new(t_list* lista, t_log* logger);
t_proceso* obtener_proceso_cola_ready(void);

//Planificadores
void iniciar_planificador_largo_plazo(void);
void iniciar_planificador_mediano_plazo(void);

#endif /* UTILS_H_ */
