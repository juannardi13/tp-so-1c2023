#ifndef UTILS_FILESYSTEM_H_
#define UTILS_FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

t_log *iniciar_logger(void);
t_config *iniciar_config(char*);

#endif /* UTILS_FILESYSTEM_H_ */
