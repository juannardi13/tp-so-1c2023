#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

t_log* iniciar_logger(void);
t_config* iniciar_config(char*);
FILE* abrir_archivo_instrucciones(char *, t_log*);
void levantar_instrucciones(FILE*, t_log*, int);
void cargar_valores_config(t_log*, char*, char*, char*);

#endif /* UTILS_H_ */