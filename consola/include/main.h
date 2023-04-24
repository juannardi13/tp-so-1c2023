#ifndef CLIENT_H_
#define CLIENT_H_



#include "utils.h"


t_log* iniciar_logger(void);

void leer_consola(t_log*);
void paquete(int);
void terminar_programa(int, t_log*, t_config*);

#endif /* CLIENT_H_ */
