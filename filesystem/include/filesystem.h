#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/config.h>

#include "utils.h"

t_log* iniciar_logger(void);
t_config* iniciar_config(void);

#endif /* FILESYSTEM_H_ */
