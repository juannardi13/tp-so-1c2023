/*
 * funcionesArchivos.h
 *
 *  Created on: Jul 25, 2023
 *      Author: utnso
 */

#ifndef FUNCIONESARCHIVOS_H_
#define FUNCIONESARCHIVOS_H_

#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <commons/string.h>
#include <sys/mman.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

int nro_bloque_escribir_cuando_escribo_en_unico_bloque(int,t_config*,char*);
void escribir_bytes_mismo_bloque(int ,int ,void* ,int ,t_config*,char*);
void escribir_archivo(char*,int,void*,int);
void escribir_bytes_diferentes_bloques(int,int,void*,int,t_config*,char*);
void escribir_bytes_en_bloque_de_principio_a_fin(int nro_bloque,void* contenido_escribir,int start_escritura,t_config* config_fcb_archivo,char*);
void* leer_bytes_mismo_bloque(int,int,int,t_config*,char*);
void leer_bytes_en_bloque(int ,int ,void* ,int,t_config*,int*,char*);
void* leer_bytes_en_bloques_distintos(int ,int ,int ,t_config*,char*);

#endif /* FUNCIONESARCHIVOS_H_ */
