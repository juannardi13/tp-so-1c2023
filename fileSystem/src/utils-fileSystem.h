#ifndef UTILS_FILESYSTEM_H_
#define UTILS_FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/bitarray.h>

t_log *iniciar_logger(void);
t_config *iniciar_config(char*);
t_bitarray *inicializarArchivoBM(int);
bool estaOcupado(t_bitarray*,int);
void usarBloque(t_bitarray*,int);
void liberarRecursosBitmap(t_bitarray*,int);

#endif /* UTILS_FILESYSTEM_H_ */
