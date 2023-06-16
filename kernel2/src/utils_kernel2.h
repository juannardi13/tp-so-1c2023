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

typedef struct {
	op_code nombre;
	int parametro_1_length;
	char* parametro_1;
	int parametro_2_length;
	char* parametro_2;
	int parametro_3_length;
	char* parametro_3;
}t_instruccion;

typedef enum {
	HRRN,
	FIFO
}tipo_algoritmo;

typedef struct {
	char* ip_memoria;
	char* puerto_memoria;
	char* ip_filesystem;
	char* puerto_filesystem;
	char* ip_cpu;
	char* puerto_cpu;
	char* puerto_escucha;
	tipo_algoritmo algoritmo_planificacion;
	int estimacion_inicial;
	float alfa_hrrn;
	int grado_multiprogramacion;
	//TODO ver como es el tema de los recursos más adelante
}archivo_config;

//Variables globales:
extern archivo_config config_kernel;
extern t_log* logger_kernel;
extern int socket_cpu;
extern int socket_memoria;
extern int socket_filesystem;
extern int pid_global;
extern t_consola* consola;
extern t_list* cola_new;
extern t_list* cola_ready;

//Estructura para pasarle a la función de los hilos:
typedef struct {
    t_log* log;
    int fd;
} t_manejar_conexion_args;

//Funciones que crean las estructuras(Logger, Config)
void cargar_valores_config(char*);
t_log* iniciar_logger(void);
t_config* iniciar_config(char*);

//Funciones sobre conexiones
bool generar_conexiones(t_config*, t_log*, int*, int*, int*);
t_instruccion recibir_instruccion(t_log*, int);

//Funciones para tratar con la PCB
void agregar_proceso_a_new(t_proceso*);
t_pcb * alocar_memoria_pcb(t_log*);
t_pcb* armar_pcb(char**, t_log*);
t_pcb* crear_estructura_pcb(char*);
t_pcb crear_pcb(t_instruccion*);
t_pcb* deserializar_pcb(t_buffer*);
void enviar_pcb(int, t_pcb*);
t_pcb* recibir_pcb(int);

//Funciones de clientes
int atender_clientes_kernel(int);
void manejar_conexion(int);
int server_escuchar(t_log*, char*, int);

//Serialización y deserialización de datos
void agregar_instruccion_a_lista(char**, char*);
t_consola *deserializar_consola(char*);
t_list *deserializar_instrucciones(t_list*, int);
char* deserializar_string(t_buffer*);
void recibir_instruccion_serializada(int);
char* recibir_instrucciones_como_string(int);

//Manejo de procesos
void agregar_proceso_a_ready(void);
void ejecutar_proceso(void);
void mostrar_cola_new(t_list*);
t_proceso* obtener_proceso_cola_ready(void);

//Planificadores
void iniciar_planificador_largo_plazo(void);
void iniciar_planificador_mediano_plazo(void);

#endif /* UTILS_H_ */
