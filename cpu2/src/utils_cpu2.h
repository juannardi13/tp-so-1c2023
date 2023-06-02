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
#include <shared-2.h>


t_log* iniciar_logger(void);
t_config* iniciar_config(void);
// int fetch_instruccion(char *, t_log *);

#endif /* UTILS_H_ */
