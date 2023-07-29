#ifndef SRC_SHARED_2_H_
#define SRC_SHARED_2_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <commons/log.h>
#include<commons/collections/list.h>
#include<commons/collections/dictionary.h>
#include <unistd.h>
#include <commons/config.h>

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXT //Queda como EXT porque si no tiene conflicto con el tipo de la instruccion.
}estado_proceso;

typedef enum {
	READ,
	WRITE,
	EXECUTE,
	READ_WRITE
}proteccion;

typedef enum {
	DESALOJADO,
	NO_DESALOJADO
}desalojo_cpu; //No se preocupen por esto que es solamente para una cosa de Kernel ;))

typedef enum
{
	CONTEXTO_DE_EJECUCION,
	MENSAJE,
	PAQUETE,
	PAQUETE_CONSOLA,
	CONSOLA,
	PCB,
	SET,
	MOV_IN,
	MOV_OUT,
	IO,
	F_OPEN,
	F_CLOSE,
	F_SEEK,
	F_READ,
	F_WRITE,
	F_TRUNCATE,
	F_CREATE,
	WAIT,
	SIGNAL,
	CREATE_SEGMENT,
	DELETE_SEGMENT,
	YIELD,
	EXIT,
	ESCRIBIR_EN_MEMORIA,
	ESCRIBIR_EN_MEMORIA_FS,
	LEER_DE_MEMORIA,
	LEER_DE_MEMORIA_FS,
	LEIDO,
	NO_LEIDO,
	SEG_FAULT,
	FINALIZAR_CONSOLA,
	SIN_INSTRUCCION,
	CREAR_ESTRUCTURAS,
	ESTRUCTURAS_CREADAS,
	LIBERAR_ESTRUCTURAS,
	ESTRUCTURAS_LIBERADAS,
	NUEVA_TABLA,
	OUT_OF_MEMORY,
	SEGMENTO_CREADO,
	NECESITO_COMPACTAR,
	COMPACTAR,
	COMPACTACION_TERMINADA,
	INICIAR_PROCESO,
	EXISTE,
	NO_EXISTE,
	CREAR_ARCHIVO,
	OK_VALOR_ESCRITO,
	FINALIZADO
}op_code;

typedef struct
{
	int stream_size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct {
	char ax[5];
	char bx[5];
	char cx[5];
	char dx[5];
	char eax[9];
	char ebx[9];
	char ecx[9];
	char edx[9];
	char rax[17];
	char rbx[17];
	char rcx[17];
	char rdx[17]; //tienen en cuenta el \0
}t_registros;

typedef struct{
	int id;
	int base;
	int tamanio;
}t_segmento;

typedef struct {
    int pid;
    t_list* segmentos;
} t_tabla_segmentos;

typedef struct {
	int pid;
	char* pid_string;
	int tamanio_instrucciones;
	char* instrucciones;
	estado_proceso estado;
	int pc;
	int tamanio_registros;
	t_registros registros;
	int tamanio_segmentos;
	t_tabla_segmentos tabla_segmentos;
	t_list* recursos_asignados;
	t_dictionary* tabla_archivos;
	//float est_prox_rafaga; //iniciaizar con archivo configuracion
	//int archivos_abiertos[][2];
} t_pcb;

typedef struct {
	int pid;
	int tamanio_instrucciones;
	char* instrucciones;// recibimos el choclo entero de las intrucciones, sin parsear nada "SET AX HOLA/nIO 12/nEXIT/n"
	int pc;
	t_registros registros_pcb;
	int tamanio_registros;
	int tamanio_segmentos;
	t_list* tabla_segmentos;
}t_contexto_de_ejecucion;

typedef struct {
    uint32_t tamanio_proceso;
    char* instrucciones;
} t_consola;


typedef struct {
	int socket;
	t_pcb* pcb;
	int llegada_ready;
	int tiempo_espera;
	int inicio_bloqueo;
	int tiempo_bloqueo;
	int principio_ultima_rafaga;
	int final_ultima_rafaga;
	int ultima_rafaga;
	int rafaga_estimada;
	double response_ratio;
	op_code ultima_instruccion;
	desalojo_cpu desalojado;
}t_proceso;

typedef struct
{
	int block_count;
	int block_size;
} t_super_bloque;

typedef struct
{
	char* nombre_archivo;
	int tamanio_archivo;
	uint32_t puntero_directo;
	uint32_t puntero_indirecto;
}t_fcb;

void agregar_a_buffer(t_buffer *, void *, int);
void agregar_a_paquete(t_paquete *, void *, int);
void agregar_entero_a_paquete(t_paquete *, int);
void agregar_int_a_paquete(t_paquete *, int);
void agregar_segmento_a_paquete(t_paquete *, t_segmento *);
void agregar_string_a_paquete(t_paquete *, void *, int);
void agregar_tabla_a_paquete(t_paquete *, t_tabla_segmentos *);
int bits_to_bytes(int);
void crear_buffer(t_paquete *);
int crear_conexion(t_log *, const char *, char *, char *);
int crear_conexion_alt(char*,char*);
t_paquete *crear_paquete(int);
t_paquete *crear_paquete_consola(void);
t_segmento *crear_segmento(int, int, int);
int deserializar_int(void *, int *);
void* deserializar_string(int, void *, int *);
void eliminar_paquete(t_paquete *);
int enviar_datos(int, void *, uint32_t);
void enviar_mensaje(char *, int);
void enviar_paquete(t_paquete *, int);
int esperar_cliente(t_log *, const char *, int);
int esperar_cliente_alt(int);
void hola(void);
t_buffer *inicializar_buffer_con_parametros(uint32_t, void *);
int iniciar_servidor(char *);
void liberar_conexion(int);
void msleep(int);
int recibir_datos(int, void *, uint32_t);
t_paquete* recibe_paquete(int);
void* recibir_buffer(int *, int);
char* recibir_mensaje(t_log *, int);
int recibir_operacion(int);
t_list *recibir_paquete(int);
void *recibir_stream(int *, int);
void send_op(int, int);
void* serializar_paquete(t_paquete* paquete, int bytes);
void* serializar_paquete_con_bytes(t_paquete *, int);

#endif /* SRC_SHARED_2_H_ */
