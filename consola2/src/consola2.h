#ifndef CLIENT_H_
#define CLIENT_H_



#include "utils_consola2.h"


t_log* iniciar_logger(void);

void leer_consola(t_log*);
void paquete(int);
void finalizar_programa(int, t_log*, t_config*);
void serializar_y_enviar_instruccion(int, t_list *instrucciones, t_paquete *paquete);

#endif /* CLIENT_H_ */

