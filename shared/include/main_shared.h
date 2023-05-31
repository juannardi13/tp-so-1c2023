#ifndef SOCKETS_H_
#define SOCKETS_H_

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


typedef enum
{
	MENSAJE,
	PAQUETE,
	PAQUETE_CONSOLA
}op_code;

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

typedef struct {
	nombre_instruccion nombre;
	char* parametro_1;
	char* parametro_2;
	char* parametro_3;
	int codigo_instruccion;
	int valor_a_procesar;
} t_instruccion;

typedef struct {
	int pid;
	t_list* instrucciones;
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

int iniciar_servidor(char* ip, char* puerto);
int esperar_cliente(int socket_servidor);
int crear_conexion(char* ip, char* puerto);
void liberar_conexion(int socket_cliente);
void* recibir_buffer(int*, int);
void crear_buffer(t_paquete*);
t_list* recibir_paquete(int);
t_paquete* recibe_paquete(int);
void recibir_mensaje(int, t_log*);
int recibir_operacion(int);
void* recibir_stream(int*, int);
void agregar_a_buffer(t_buffer *buffer, void *src, int size);
t_buffer *inicializar_buffer_con_parametros(uint32_t size, void *stream);
void* serializar_paquete_con_bytes(t_paquete* paquete, int bytes);
t_buffer* serializar_paquete(t_paquete* paquete);
void agregar_entero_a_paquete(t_paquete *paquete, int tamanio_proceso);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void enviar_pcb(t_pcb un_pcb, int conexion);
t_paquete* crear_paquete_consola(void);
int recibir_operacion_nuevo(int socket_cliente);
int enviar_datos(int socket_fd, void *source, uint32_t size);
int recibir_datos(int socket_fd, void *source, uint32_t size);

#endif
