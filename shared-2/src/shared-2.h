#ifndef SRC_SHARED_2_H_
#define SRC_SHARED_2_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <commons/log.h>
#include<commons/collections/list.h>
#include <unistd.h>

typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXT //Queda como EXT porque si no tiene conflicto con el tipo de la instruccion.
}estado_proceso;

typedef enum
{
	MENSAJE,
	PAQUETE,
	PAQUETE_CONSOLA
} op_code;

typedef enum {
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
	WAIT,
	SIGNAL,
	CREATE_SEGMENT,
	DELETE_SEGMENT,
	YIELD,
	EXIT
} nombre_instruccion;

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

// creo t_recurso, ver que iria en sus campos
typedef struct {
	// campos del recurso
}t_recurso;

typedef struct {
	nombre_instruccion nombre;

	char* parametro_1; //tipo char
	char* parametro_2;
	char* parametro_3;

} t_instruccion;

typedef struct {
	char ax[3];
	char bx[3];
	char cx[3];
	char dx[3];
	char eax[7];
	char ebx[7];
	char ecx[7];
	char edx[7];
	char rax[15];
	char rbx[15];
	char rcx[15];
	char rdx[15];
}t_registros;


typedef struct {
	int pid;
	t_list* instrucciones;
	estado_proceso estado;
	int pc;
	int tamanio;
	t_registros registros;
	//int segmentos[][3];
	//float est_prox_rafaga; //iniciaizar con archivo configuracion
	//float timestamp;
	//int archivos_abiertos[][2];

} t_pcb;

typedef struct
{
    uint32_t tamanio_proceso;
    t_list* instrucciones;
} t_consola;



typedef struct {
	int socket;
	t_pcb* pcb;
}t_proceso;

typedef struct
{
	int block_count;
	int block_size;
} t_super_bloque;

void hola(void);
int iniciar_servidor(char *);
int esperar_cliente(t_log *, const char *, int);
int recibir_operacion(int);
void* recibir_buffer(int *, int);
char* recibir_mensaje(t_log *, int);
t_paquete *crear_paquete_consola(void);
void crear_buffer(t_paquete *);
void agregar_a_paquete(t_paquete *, void *, int);
void agregar_a_buffer(t_buffer *, void *, int);
void enviar_mensaje(char *, int);
void* serializar_paquete_con_bytes(t_paquete *, int);
void eliminar_paquete(t_paquete *);
void *recibir_stream(int *, int);
t_buffer *inicializar_buffer_con_parametros(uint32_t, void *);
t_buffer* serializar_paquete(t_paquete *);
t_paquete *crear_paquete(void);
void agregar_entero_a_paquete(t_paquete *, int);
void enviar_paquete(t_paquete *, int);
t_list *recibir_paquete(int);
t_paquete* recibe_paquete(int);
int enviar_datos(int, void *, uint32_t);
int recibir_datos(int, void *, uint32_t);
int crear_conexion(t_log *, const char *, char *, char *);
void liberar_conexion(int);

#endif /* SRC_SHARED_2_H_ */
