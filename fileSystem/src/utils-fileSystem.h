#ifndef UTILS_FILESYSTEM_H_
#define UTILS_FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>

t_log *iniciar_logger(void);
t_config *iniciar_config(char*);
t_bitarray *inicializar_archivo_bm(int);
bool esta_ocupado(t_bitarray*,int);
void usar_bloque(t_bitarray*,int);
void liberar_recursos_bitmap(t_bitarray*,int);
void levantar_fcb_nuevo_archivo(const char*,int,t_bitarray*);
void levantar_archivo_bloques(int,int);

#endif /* UTILS_FILESYSTEM_H_ */
