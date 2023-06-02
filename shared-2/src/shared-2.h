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
	PAQUETE
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
	int pid;
	t_list* instrucciones;
	estado_proceso estado;
	int pc;
	int tamanio;
	//t_registros registros_cpu;
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

void hola(void);
int iniciar_servidor(t_log*, const char *, char *ip, char *);
int esperar_cliente(t_log *, const char *, int);
int recibir_operacion(t_log *, int);
void* recibir_buffer(int *, int);
char* recibir_mensaje(t_log *, int);

#endif /* SRC_SHARED_2_H_ */
