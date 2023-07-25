#include "segmentacion.h"

int hay_espacio(int size) {

	int disponible = 0;
	int offset = 0;

	while (offset < config_memoria.tam_memoria) {
		if (! bitarray_test_bit(bitmap, offset)) { // si está disponible devuelve 0
			disponible++;
		}
		offset++;
	}
	return size <= disponible ? 1 : 0;
}

t_list *buscar_huecos_libres(void) {

	t_list *aux = list_create();
	int base = 0;

	while (base < config_memoria.tam_memoria) {
		t_segmento *hueco = buscar_hueco_libre(&base); // HUECO == SEGMENTO A NIVEL CÓDIGO
		list_add(aux, hueco);
	}

	return aux;
}

t_segmento *buscar_hueco_libre(int *base) {

	t_segmento *aux = malloc(sizeof(t_segmento));

	while ((bitarray_test_bit(bitmap, *base) == 1) && (*base < config_memoria.tam_memoria)) {
		(*base)++;
	}

	int limite = bytes_libres_desde(*base);

	// aux->id = ??
	aux->base = *base;
	aux->tamanio = limite;

	*base += limite;

	return aux;
}

int bytes_libres_desde(int base) {

	int aux = 0;

	while ((bitarray_test_bit(bitmap, base) == 0) && (base < config_memoria.tam_memoria)) {
		base++;
		aux++;
	}

	return aux;
}

int obtener_base_segun_criterio(t_list *huecos, int size) {

	int base;
	char *algoritmo = config_memoria.algoritmo_asignacion;

	if (string_equals_ignore_case(algoritmo, "FIRST")) {
		t_segmento *aux = list_get(huecos, 0);
		base = aux->base;
	}
	else if (string_equals_ignore_case(algoritmo, "BEST")) {
		base = base_segun_best(huecos, size);
	}
	else if (string_equals_ignore_case(algoritmo, "WORST")) {
		base = base_segun_worst(huecos, size);
	}

	return base;
}

int base_segun_best(t_list *huecos, int size) {

	t_segmento *aux = list_get_minimum(huecos, (void *) cmp_hueco_menor);

	return aux->base;
}

t_segmento *cmp_hueco_menor(t_segmento *hueco1, t_segmento *hueco2) {

	return hueco1->tamanio < hueco2->tamanio ? hueco1 : hueco2;
}

int base_segun_worst(t_list *huecos, int size) {

	t_segmento *aux = list_get_maximum(huecos, (void *) cmp_hueco_mayor);

	return aux->base;
}

t_segmento *cmp_hueco_mayor(t_segmento *hueco1, t_segmento *hueco2) {

	return hueco1->tamanio > hueco2->tamanio ? hueco1 : hueco2;
}

void dinamitar_listas(t_list *todos_los_huecos, t_list *algunos_huecos) {

	list_destroy_and_destroy_elements(todos_los_huecos, (void *) eliminar_nodo);
	list_destroy(algunos_huecos);
}

void eliminar_nodo(t_segmento *hueco) {

	free(hueco);
}

void guardar_en_bitmap(t_segmento *seg) {

	for (int i = 0; i < seg->tamanio; i++) {
		bitarray_set_bit(bitmap, seg->base + i);
	}
}

void agregar_a_lista(int pid, t_segmento *seg) {

	t_tabla_segmentos *tabla = tabla_por_pid(pid);

	list_add(tabla->segmentos, seg);
}

t_tabla_segmentos *tabla_por_pid(int pid) {

	bool mismo_pid(t_tabla_segmentos *tabla) {
		return tabla->pid == pid;
	}
	t_tabla_segmentos *aux = list_find(tablas_segmentos, (void *) mismo_pid);

	return aux;
}

t_tabla_segmentos *nueva_tabla_segmentos(int pid) {

	t_tabla_segmentos *tabla = malloc(sizeof(t_tabla_segmentos));
	tabla->pid = pid;
	tabla->segmentos = list_create();
	list_add(tabla->segmentos, segmento_0);

	return tabla;
}

t_segmento *obtener_segmento(int pid, int id) {

	bool mismo_id(t_segmento *seg) {
		return seg->id == id;
	}

	t_tabla_segmentos *aux = tabla_por_pid(pid);
	t_segmento *seg = list_find(aux->segmentos, (void *) mismo_id);

	return seg;
}

void liberar_segmento(t_segmento *seg) {

	if (seg->id != 0) {
		for (int i = 0; i < seg->tamanio; i++) {
			bitarray_clean_bit(bitmap, seg->base + i);
		}
		free(seg);
	}
}

void quitar_de_lista(int pid, t_segmento *seg) {

	t_tabla_segmentos *tabla = tabla_por_pid(pid);

	if (! list_remove_element(tabla->segmentos, seg)) {
		log_error(logger, "Error al borrar segmento");
		abort();
	}
}

t_list *get_segmentos_en_uso(void) {

	t_list *aux = list_create();
	int tam_lista = list_size(tablas_segmentos);

	for (int i = 0; i < tam_lista; i++) {

		t_tabla_segmentos *tabla = list_get(tablas_segmentos, i);
		int cant_segs = list_size(tabla->segmentos);

		for (int j = 0; j < cant_segs; j++) {

			t_segmento *seg = list_get(tabla->segmentos, j);

			if (seg->id != 0) {
				list_add(aux, seg);
			}
		}
	}
	return aux;
}

t_list *get_contenido_segmentos(t_list *segmentos) {

	t_list *aux = list_map(segmentos, (void *) obtener_contenido);

	return aux;
}

void *obtener_contenido(t_segmento *seg) {

	void *contenido = malloc(seg->tamanio);
	memcpy(contenido, memoria + seg->base, seg->tamanio);

	return contenido;
}

int primer_byte_disponible(void) {

	int i = 0;

    while (i < config_memoria.tam_memoria && esta_ocupado(i)) {
        i++;
    }
    return i;
}

bool esta_ocupado(int i) {
    return bitarray_test_bit(bitmap, i);
}

void mostrar_esquema_memoria(void) {

	int cant_procesos = list_size(tablas_segmentos);

	for (int i = 0; i < cant_procesos; i++) {
		t_tabla_segmentos *tabla = list_get(tablas_segmentos, i);
		int pid = tabla->pid;
		int cant_segs = list_size(tabla->segmentos);

		for (int j = 0; j < cant_segs; j++) {
			t_segmento *seg = list_get(tabla->segmentos, j);
			log_info(logger, "PID: %d - Segmento: %d - Base: %d - Tamaño: %d", pid, seg->id, seg->base, seg->tamanio);
		}
	}
}

void destruir_tabla(t_tabla_segmentos *tabla) {

	list_destroy_and_destroy_elements(tabla->segmentos, (void *) liberar_segmento);
	free(tabla);
}
