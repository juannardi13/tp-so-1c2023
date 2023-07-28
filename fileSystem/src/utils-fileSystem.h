#ifndef UTILS_FILESYSTEM_H_
#define UTILS_FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <shared-2.h>
#include "funcionesTruncar.h"
#include "funcionesArchivos.h"

typedef struct{
	char* ip;
	char* puerto_memoria;
	char* puerto_escucha;
	char* path_superbloque;
	char* path_bitmap;
	char* path_bloques;
	char* path_fcb;
	int retardo_acceso_bloque;
} t_config_valores;

typedef struct{
	int block_size;
	int block_count;
} t_super_bloque_valores;

t_log *iniciar_logger(void);
t_config *iniciar_config(char*);
t_bitarray* inicializar_archivo_bm();
t_bitarray* inicializar_archivo_bm_prueba();
bool esta_ocupado(int);
void usar_bloque(t_bitarray*,int);
void liberar_recursos_bitmap();
void levantar_fcb_nuevo_archivo(const char*);
void levantar_archivo_bloques();
void levantar_archivo_bloques_prueba();
void atender_clientes_file_system(int);
char* obtener_ruta_archivo(const char*);
void truncar_archivo(char*,int);
int abrir_archivo(char*);
int crear_archivo(char*);
void inicializar_estructuras();
void probando_cositas();
void probando_cositas_2();
void inicializar_servidor();
void reducir_tamanio(int,int,t_config*,char* nombre_archivo);
void ampliar_tamanio(int,int,t_config*,char* nombre_archivo);
void acceso_escritura_bitmap(int,int);
void asignar_bloques_iniciales(char*,t_config*);
int primer_bloque_disponible(void);
int obtener_posicion_archivo_bloques(int);
void* leer_archivo(char*,int,int);
void escribir_bytes_mismo_bloque(int ,int ,void* ,int ,t_config*,char*);
void escribir_archivo(char*,int,void*,int);
void escribir_bytes_diferentes_bloques(int,int,void*,int,t_config*,char*);
void acceso_a_bloque(void* , void* ,int ,char* ,int ,int );
int agregar_a_stream(void *, int* , void *, int );



extern int socket_memoria;
extern t_config* config_super_bloque;
extern t_config* config;
extern t_log* logger;
extern t_config_valores config_valores;
extern t_super_bloque_valores config_super_bloque_valores;
extern t_bitarray* estructura_bitmap;
extern char* mapping_archivo_bloques;
extern FILE* archivo_bloques;
extern FILE* archivo_bm;

#endif /* UTILS_FILESYSTEM_H_ */

