#ifndef SRC_SHARED_2_H_
#define SRC_SHARED_2_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <commons/log.h>
#include <unistd.h>

typedef enum
{
	MENSAJE,
	PAQUETE
} op_code;

void hola(void);
int iniciar_servidor(t_log*, const char *, char *ip, char *);
int esperar_cliente(t_log *, const char *, int);
int recibir_operacion(t_log *, int);
void* recibir_buffer(int *, int);
char* recibir_mensaje(t_log *, int);

#endif /* SRC_SHARED_2_H_ */
