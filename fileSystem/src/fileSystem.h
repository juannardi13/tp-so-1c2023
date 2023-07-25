
#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <shared-2.h>
#include <commons/bitarray.h>

#include "utils-fileSystem.h"

t_config* config_super_bloque;
t_config* config;
t_log* logger;
t_config_valores config_valores;
t_super_bloque_valores config_super_bloque_valores;
t_bitarray* estructura_bitmap;
char* mapping_archivo_bloques;
FILE* archivo_bloques;
FILE* archivo_bm;

#endif /* FILESYSTEM_H_ */
