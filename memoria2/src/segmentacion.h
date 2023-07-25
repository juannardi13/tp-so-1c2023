#ifndef SEGMENTACION_H_
#define SEGMENTACION_H_

#include "utils_memoria.h"

int hay_espacio(int);
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
void guardar_en_bitmap(t_segmento *);
t_tabla_segmentos *tabla_por_pid(int);
void agregar_a_lista(int, t_segmento *);
t_tabla_segmentos *nueva_tabla_segmentos(int);
t_segmento *obtener_segmento(int, int);
void eliminar_segmento(int, int);
void liberar_segmento(t_segmento *);
void quitar_de_lista(int, t_segmento *);
t_list *get_segmentos_en_uso(void);
t_list *get_contenido_segmentos(t_list *);
void *obtener_contenido(t_segmento *);
int primer_byte_disponible(void);
bool esta_ocupado(int);
void mostrar_esquema_memoria(void);
void destruir_tabla(t_tabla_segmentos *);

#endif /* SEGMENTACION_H_ */
