#include "inicio.h"

void iniciar_memoria(void) {

	memoria = malloc(config_memoria.tam_memoria);
	iniciar_lista_tabla_segmentos();
	iniciar_bitmap_bytes_libres(config_memoria.tam_memoria);
	segmento_0 = crear_segmento(0, 0, config_memoria.tam_segmento_0);
	guardar_en_bitmap(segmento_0);
}

void iniciar_lista_tabla_segmentos(void) {

	tablas_segmentos = list_create();
}

void iniciar_bitmap_bytes_libres(int tam_memoria) {

	void *data = malloc(tam_memoria); // cant bits = cant bytes memoria
	memset(data, 0, tam_memoria);

	int size = bits_to_bytes(tam_memoria); // ej. 4096b = 512B
	bitmap = bitarray_create_with_mode(data, size, MSB_FIRST);
}
