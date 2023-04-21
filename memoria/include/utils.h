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
#include<string.h>
#include<assert.h>

#define PUERTO "7294"

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

extern t_log* logger;

void* recibir_buffer(int*, int);
t_log* iniciar_logger(void);
int iniciar_servidor(t_log* logger, const char* name, char* ip, char* puerto);
int esperar_cliente(t_log* logger, const char* name, int socket_servidor);
t_list* recibir_paquete(int);
void recibir_mensaje(t_log*, int);
int recibir_operacion(t_log*, int);
t_config* iniciar_config(void);
void iterator(char* value);

#endif /* UTILS_H_ */
