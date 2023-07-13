#ifndef SEGMENTACION_H_
#define SEGMENTACION_H_

#include "utils_memoria.h"

t_segmento *crear_segmento(int, int);
int hay_espacio(int);
int hay_espacio_contiguo(int);
t_list *buscar_huecos_libres(void);
t_segmento *buscar_hueco_libre(int *);
int bytes_libres_desde(int);
int obtener_base_segun_criterio(t_list *, int);
int base_segun_best(t_list *, int);
t_segmento *cmp_hueco_menor(t_segmento *, t_segmento *);
int base_segun_worst(t_list *, int);
t_segmento *cmp_hueco_mayor(t_segmento *, t_segmento *);
void dinamitar_listas(t_list *, t_list *);
void eliminar_nodo(t_segmento *);


#endif /* SEGMENTACION_H_ */
