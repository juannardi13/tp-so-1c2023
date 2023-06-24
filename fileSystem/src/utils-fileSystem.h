#ifndef UTILS_FILESYSTEM_H_
#define UTILS_FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <shared-2.h>

t_log *iniciar_logger(void);
t_config *iniciar_config(char*);
t_bitarray* inicializar_archivo_bm(FILE*f,int,t_config*);
bool esta_ocupado(t_bitarray*,int);
void usar_bloque(t_bitarray*,int);
void liberar_recursos_bitmap(t_bitarray*,int,int,FILE*,FILE*,char*);
void levantar_fcb_nuevo_archivo(const char*,int,t_bitarray*);
void levantar_archivo_bloques(FILE* f,int,int,t_config*,char *);
int atender_clientes_file_system(int ,t_log*,t_config*,t_bitarray*);
char* obtener_ruta_archivo(const char*);
void truncar_archivo(char*,char*,t_config*,t_bitarray*,char*);
int abrir_archivo(char*);
int crear_archivo(char*);

#endif /* UTILS_FILESYSTEM_H_ */
