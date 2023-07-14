#ifndef UTILS_FILESYSTEM_H_
#define UTILS_FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <shared-2.h>

typedef struct{
	char* ip;
	int puerto_memoria;
	int puerto_escucha;
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
t_bitarray* inicializar_archivo_bm(FILE*f);
bool esta_ocupado(int);
void usar_bloque(t_bitarray*,int);
void liberar_recursos_bitmap(FILE*,FILE*,char*);
void levantar_fcb_nuevo_archivo(const char*);
void levantar_archivo_bloques(FILE* f,char *);
int atender_clientes_file_system(int);
char* obtener_ruta_archivo(const char*);
void truncar_archivo(char*,char*,char*);
int abrir_archivo(char*);
int crear_archivo(char*);
void inicializar_estructuras();
void probando_cositas();
void inicializar_servidor();

extern t_log* logger;
extern t_config_valores *config_valores;
extern t_super_bloque_valores *config_super_bloque_valores;
extern t_bitarray* estructura_bitmap;
extern char* mapping_archivo_bloques;
extern FILE* archivo_bloques;

#endif /* UTILS_FILESYSTEM_H_ */
