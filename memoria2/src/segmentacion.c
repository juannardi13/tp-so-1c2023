#include "segmentacion.h"

t_segmento *crear_segmento(int id, int size) {

	if (! hay_espacio(size)) { // NO hay espacio, no importa compactación
		log_info(logger, "No hay espacio suficiente en memoria para crear el segmento solicitado");
		return NULL;
	}

//	if (! hay_espacio_contiguo(size)) { // HAY ESPACIO, hay que compactar
//		// enviar_solicitud_compactacion();
//	}

	t_list *huecos_disponibles = buscar_huecos_libres(); // incluidos los que no alcanzan para size

	bool es_suficiente_para(t_segmento *hueco) { // TRAMOYA
		return (hueco->tamanio >= size);
	}

	t_list *huecos_suficientes = list_filter(huecos_disponibles, (void *) es_suficiente_para);

	if (list_is_empty(huecos_suficientes)) {
		log_info(logger, "No hay suficiente espacio contiguo para crear el segmento, requiere compactación");
		// enviar_solicitud_compactacion(); //TODO
		return NULL;
	}

	// AHORA SÍ, sabemos que HAY lugar y está CONTIGUO
	int base = obtener_base_segun_criterio(huecos_suficientes, size);

	dinamitar_listas(huecos_disponibles, huecos_suficientes);

	t_segmento *segmento = malloc(sizeof(t_segmento));

	segmento->base 	 = base;
	segmento->id   	 = id;
	segmento->tamanio = size;

	return segmento;
}

int hay_espacio(int size) {

	int disponible = 0;
	int offset = 0;

	while (offset < config_memoria->tam_memoria) {
		if (! bitarray_test_bit(bitmap, offset)) { // si está disponible devuelve 0
			disponible++;
		}
		offset++;
	}
	return size <= disponible ? 1 : 0;
}

int hay_espacio_contiguo(int size) {

	t_list *huecos_disponibles = buscar_huecos_libres(); // incluidos los que no alcanzan para size

	bool es_suficiente_para(t_segmento *hueco) {
		return (hueco->tamanio >= size);
	}

	t_list *huecos_suficientes = list_filter(huecos_disponibles, (void *) es_suficiente_para);

	if (list_is_empty(huecos_suficientes)) {
		return 0;
	}

	return 0;
}

t_list *buscar_huecos_libres(void) {

	t_list *aux = list_create();
	int base = 0;

	while (base < config_memoria->tam_memoria) {
		t_segmento *hueco = buscar_hueco_libre(&base); // HUECO == SEGMENTO A NIVEL CÓDIGO
		list_add(aux, hueco);
	}

	return aux;
}

t_segmento *buscar_hueco_libre(int *base) {

	t_segmento *aux = malloc(sizeof(t_segmento));

	while ((bitarray_test_bit(bitmap, *base) == 1) && (*base < config_memoria->tam_memoria)) {
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

	while ((bitarray_test_bit(bitmap, base) == 0) && (base < config_memoria->tam_memoria)) {
		base++;
		aux++;
	}

	return aux;
}

int obtener_base_segun_criterio(t_list *huecos, int size) {

	int base;
	char *algoritmo = config_memoria->algoritmo_asignacion;

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

	return hueco1->base < hueco2->base ? hueco1 : hueco2;
}

int base_segun_worst(t_list *huecos, int size) {

	t_segmento *aux = list_get_maximum(huecos, (void *) cmp_hueco_mayor);

	return aux->base;
}

t_segmento *cmp_hueco_mayor(t_segmento *hueco1, t_segmento *hueco2) {

	return hueco1->base > hueco2->base ? hueco1 : hueco2;
}

void dinamitar_listas(t_list *todos_los_huecos, t_list *algunos_huecos) {

	list_destroy_and_destroy_elements(todos_los_huecos, (void *) eliminar_nodo);
	list_destroy(algunos_huecos);
}

void eliminar_nodo(t_segmento *hueco) {

	free(hueco);
}

