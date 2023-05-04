#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

int crear_conexion(char *ip, char* puerto);

#endif /* UTILS_H_ */
