#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<commons/string.h>
#include<semaphore.h>
#include<string.h>
#include<assert.h>
#include<pthread.h>
#include<shared-2.h>

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
	char** recursos;
	char** instancias_recursos;
}archivo_config;

typedef struct {
	char* nombre;
	int instancias_usadas;
	int instancias_totales;
	t_queue* cola_espera;
	pthread_mutex_t* mutex_cola_espera;
	pthread_mutex_t* mutex_instancia_usada;
}t_recurso;

typedef struct {
	char* nombre;
	int puntero;
	int tamanio;
	int instancias_usadas;
	int instancias_totales;
	t_queue* cola_bloqueados;
	pthread_mutex_t* mutex_cola_bloqueados;
	pthread_mutex_t* mutex_instancia_usada;
}t_archivo;

typedef struct {
	char* nombre;
	int puntero;
	int tamanio;
}t_archivo_proceso;

typedef struct {
	t_proceso* proceso;
	char* nombre_archivo;
	int nuevo_tamanio_truncate;
	int puntero;
	int cantidad_bytes;
	int direccion_fisica;
	op_code codigo_operacion;
}t_peticion;

//Variables globales:
extern archivo_config config_kernel;
extern t_log* logger_kernel;
extern t_log* kernel_principal;
extern int pid_global;
extern int socket_cpu;
extern int socket_memoria;
extern int socket_filesystem;
extern int compactacion_solicitada;
extern t_consola* consola;
extern t_queue* cola_peticiones_file_system;
extern t_list* cola_block_io;
extern t_list* cola_exec;
extern t_list* cola_exit;
extern t_list* cola_new;
extern t_list* cola_ready;
extern t_registros registros_iniciados;
extern t_dictionary* recursos;
extern t_dictionary* tabla_global_archivos;
extern t_dictionary* segmentos;
extern t_dictionary* procesos_en_el_sistema;

extern pthread_mutex_t mutex_block_io;
extern pthread_mutex_t mutex_exec;
extern pthread_mutex_t mutex_exit;
extern pthread_mutex_t mutex_new;
extern pthread_mutex_t mutex_operacion_file_system;
extern pthread_mutex_t mutex_operacion_memoria;
extern pthread_mutex_t mutex_pid;
extern pthread_mutex_t mutex_ready;
extern pthread_mutex_t mutex_tabla_global_archivos;
extern pthread_mutex_t mutex_compactacion;
extern pthread_mutex_t mutex_peticiones_fs;
extern pthread_mutex_t mutex_procesos_en_el_sistema;
extern pthread_mutex_t mutex_compactacion_solicitada;
extern sem_t sem_archivos;
extern sem_t sem_admitir;
extern sem_t sem_block_io;
extern sem_t sem_exec;
extern sem_t sem_exit;
extern sem_t sem_grado_multiprogramacion;
extern sem_t sem_ready;
extern sem_t sem_recursos;
extern sem_t sem_peticiones_file_system;
extern sem_t sem_finalizar_peticiones_fs;

extern pthread_t thread_archivos;
extern pthread_t thread_ready;
extern pthread_t thread_recursos;
extern pthread_t thread_exit;
extern pthread_t thread_exec;
extern pthread_t thread_ready;
extern pthread_t thread_blocked;
extern pthread_t thread_admitir_ready;
extern pthread_t thread_peticiones_fs;

//Estructura para pasarle a la función de los hilos:
typedef struct {
    t_log* log;
    int fd;
} t_manejar_conexion_args;

//Funciones que crean las estructuras(Logger, Config)
void cargar_valores_config(char*);
t_config* iniciar_config(char*);
t_log* iniciar_logger(void);
void iniciar_registros(void);

//Funciones sobre conexiones
void avisar_a_modulo(int, op_code);
bool generar_conexiones(t_config*, t_log*, int*, int*, int*);
t_instruccion recibir_instruccion(t_log*, int);

//Funciones para tratar con la PCB
void agregar_proceso_a_new(t_proceso*);
t_pcb * alocar_memoria_pcb(t_log*);
t_pcb* armar_pcb(char**, t_log*);
t_pcb* crear_estructura_pcb(char*);
t_pcb crear_pcb(t_instruccion*);
t_pcb* deserializar_pcb(t_buffer*);
void eliminar_pcb(t_pcb*);
void enviar_pcb(int, t_pcb*);
t_pcb* recibir_pcb(int);

//Funciones de clientes
int atender_clientes_kernel(int);
void manejar_conexion(int*);
int server_escuchar(t_log*, char*, int);

//Serialización y deserialización de datos
int agregar_a_stream(void*, int*, void*, int);
void agregar_instruccion_a_lista(char**, char*);
t_consola *deserializar_consola(char*);
t_list *deserializar_instrucciones(t_list*, int);
char* deserializar_el_string(t_buffer*);
void recibir_instruccion_serializada(int);
char* recibir_instrucciones_como_string(int);

//Manejo de procesos
void agregar_proceso_a_ready(void);
void estado_block_io(void);
void estado_ejecutar(void);
void estado_exit(void);
void estado_ready(void);
void mostrar_cola(t_list*);
t_proceso* obtener_proceso_cola_ready(void);

//Planificadores
void iniciar_planificador_largo_plazo(void);
void iniciar_planificador_corto_plazo(void);

//Finalización y destrucción Kernel
void destruir_listas(void);
void destruir_semaforos(void);
void finalizar_kernel(void);
void liberar_recurso(char*, void*);
void liberar_recursos(void);
void sighandler(int);

//Funciones para el manejo de los recursos
int asignar_recurso(t_recurso*, t_proceso*);
void funcion_recurso(char*, void*);
void* hilo_recursos(void*);
void iniciar_recursos(void);
void liberar_instancia_recurso(t_recurso*);
void liberar_recursos_asignados(t_list*);
int string_to_int(char*);

//Funciones de planificación HRRN y FIFO
void aplicar_hrrn(void);
void calcular_estimacion(t_proceso*);
void calcular_response_ratio(t_proceso*);
bool comparador_response_ratio(t_proceso*, t_proceso*);
void mostrar_response_ratio(t_proceso*);
int get_time(void);

//Funciones de la conexión con Memoria
void asignar_segmentos_de_memoria(t_pcb*);
void enviar_parametros_a_memoria(int, int, int, op_code);
void enviar_pid_memoria(int, op_code);
void ordenar_compactacion(void);
void recibir_respuesta_create(t_proceso*, int, int);
void recibir_tablas_segmentos(t_pcb*);
void recibir_tablas_delete(t_pcb*);

//Funciones para el manejo de archivos
void inicializar_tabla_global_archivos(void);
void agregar_archivo_a_tabla_global_archivos(char*);
void quitar_entrada_archivo_del_proceso(char*, t_proceso*);
void quitar_entrada_archivo_de_tabla_global_archivos(char*);
void* hilo_archivos(void*);
void funcion_archivo(char*, void*);
void liberar_entrada_archivo(t_archivo*);
void actualizar_puntero_archivo_proceso(t_proceso*, char*, int);
op_code consultar_existencia_archivo_a_fs(char*);
void crear_archivo_en_fs(char*);

//Funciones conexión con File System
void iniciar_peticiones_file_system(void);
void* manejar_peticiones_fs(void);
t_proceso* enviar_peticion_file_system(void);
void respuesta_peticion_file_system(void);

#endif /* UTILS_H_ */
