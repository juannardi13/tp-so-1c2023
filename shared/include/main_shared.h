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
	PAQUETE
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
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct {
	nombre_instruccion nombre;
	//parametros ???
} t_instruccion;

typedef struct {
	int PID;
	char** instrucciones;
	int PC;
	//t_registros registros_cpu;
	//int segmentos[][3];
	//float est_prox_rafaga; //iniciaizar con archivo configuracion
	//float timestamp;
	//int archivos_abiertos[][2];

} t_pcb;

int iniciar_servidor(t_log* logger, const char* name, char* ip, char* puerto);
int esperar_cliente(t_log* logger, const char* name, int socket_servidor);
int crear_conexion(t_log* logger, const char* server_name, char* ip, char* puerto);
void liberar_conexion(int* socket_cliente);
void* recibir_buffer(int*, int);
void crear_buffer(t_paquete*);
t_list* recibir_paquete(int);
char* recibir_mensaje(t_log*, int);
int recibir_operacion(t_log*, int);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete* crear_paquete(void);
void* serializar_paquete(t_paquete*, int);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void enviar_pcb(t_pcb un_pcb, int conexion);

#endif
