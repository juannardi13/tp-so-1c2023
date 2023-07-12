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
#include<shared-2.h>

typedef struct {
	op_code nombre;
	int parametro_1_length;
	char* parametro_1;
	int parametro_2_length;
	char* parametro_2;
	int parametro_3_length;
	char* parametro_3;
} t_instruccion;

//Iniciación estructuras (Logger, Config)
FILE* abrir_archivo_instrucciones(char *, t_log*);
void cargar_valores_config(t_log*, char*, char*, char*);
t_config* iniciar_config(char*);
t_log* iniciar_logger(void);

//Armado de Instrucciones con la estructura t_instruccion (Quedó obsoleto, optamos por usar char* para las instrucciones)
t_instruccion* armar_instruccion(op_code, char*, char*, char*);
t_list* parsear_instrucciones(char*, t_log*);
void parsear_instrucciones_y_enviar(char*, int, t_log*);
void serializar_instrucciones(t_list*, t_paquete*);

//Apertura y lectura de Pseudocodigo desde un archivo .txt
char* leer_archivo_pseudocodigo(char*, t_log*);
void levantar_instrucciones(FILE*, t_log*, int);

//Serialización de las instrucciones como String y envío 
int agregar_a_stream(void*, int*, void*, int);
void enviar_string(int, char*, op_code);
void* serializar_string(char*, t_paquete*, op_code);

#endif /* UTILS_H_ */
