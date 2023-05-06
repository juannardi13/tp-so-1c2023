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

#define PUERTO "9120"

t_log* iniciar_logger(void);
t_config* iniciar_config(char*);
void iterator(char* value);
bool generar_conexiones(t_config*, t_log*, int*, int*, int*);

#endif /* UTILS_H_ */
