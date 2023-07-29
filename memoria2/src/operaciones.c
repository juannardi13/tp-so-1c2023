#include "operaciones.h"

void atender_File_System(int *fileSystem_fd) {

	int socket_cliente = *fileSystem_fd;

	while (1) {
		int cod_op = recibir_operacion(socket_cliente);

		switch(cod_op) {
		case -1:
			log_warning(logger, "Se desconectó el File System!");
			return;
		}
	}
}

void recv_nuevo_proceso(int pid, int socket_cliente) {

	t_tabla_segmentos *nueva_tabla = nueva_tabla_segmentos(pid);

	list_add(tablas_segmentos, nueva_tabla);

	log_info(logger, "Creación de Proceso PID: %d", pid);

	send_tabla(socket_cliente, nueva_tabla); //TODO revisar esta función
}

void send_tabla(int socket_cliente, t_tabla_segmentos *tabla) {

	t_paquete *paquete = crear_paquete(NUEVA_TABLA);

	agregar_tabla_a_paquete(paquete, tabla);

	enviar_paquete(paquete, socket_cliente);
	eliminar_paquete(paquete);

//	int cant_segs = list_size(tabla->segmentos);
//
//	t_paquete *paquete = malloc(sizeof(t_paquete));
//	t_buffer* buffer = malloc(sizeof(t_buffer));
//
//	buffer->stream_size = sizeof(int) + sizeof(int) + (3 * sizeof(int) * cant_segs);
//
//	void *stream = malloc(buffer->stream_size);
//
//	memcpy(stream, &(tabla->pid), sizeof(int));
//	stream += sizeof(int);
//	memcpy(stream, &cant_segs, sizeof(int));
//	stream += sizeof(int);
//
//	t_list_iterator* iterador = list_iterator_create(tabla->segmentos);
//
//	while(list_iterator_has_next(iterador)) {
//		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);
//
//		memcpy(stream, &(aux->id), sizeof(int));
//		stream += sizeof(int);
//		memcpy(stream,&(aux->base), sizeof(int));
//		stream += sizeof(int);
//		memcpy(stream, &(aux->tamanio), sizeof(int));
//		stream += sizeof(int);
//	}
//
//	list_iterator_destroy(iterador);
//
//	buffer->stream = stream;
//
//	paquete->codigo_operacion = NUEVA_TABLA;
//
//	paquete->buffer = buffer;
//
//	int bytes = 2 * sizeof(int) + paquete->buffer->stream_size;
//	void *a_enviar = malloc(bytes);
//	int offset = 0;
//
//	agregar_a_stream(a_enviar, &offset, &(paquete->codigo_operacion), sizeof(int));
//	agregar_a_stream(a_enviar, &offset, &(paquete->buffer->stream_size), sizeof(int));
//	agregar_a_stream(a_enviar, &offset, paquete->buffer->stream, paquete->buffer->stream_size);
//
//	send(socket_cliente, a_enviar, bytes, 0);
//
//	free(a_enviar);
////	eliminar_paquete(paquete);
}

void recv_crear_segmento(int socket_cliente, void *stream) {

	int offset = 0;

	int pid = deserializar_int(stream, &offset);
	int id = deserializar_int(stream, &offset);
	int tamanio = deserializar_int(stream, &offset);

	if (! hay_espacio(tamanio)) { // NO hay espacio, no importa compactación
		log_info(logger, "No hay espacio suficiente en memoria para crear el segmento solicitado");
		t_paquete *paquete = crear_paquete(OUT_OF_MEMORY);
		int random = 1;
		agregar_int_a_paquete(paquete, random);
		enviar_paquete(paquete, socket_cliente);
		eliminar_paquete(paquete);
		return;
	}

	t_list *huecos_disponibles = buscar_huecos_libres(); // incluidos los que no alcanzan para size

	bool es_suficiente_para(t_segmento *hueco) { // TRAMOYA
		return (hueco->tamanio >= tamanio);
	}

	t_list *huecos_suficientes = list_filter(huecos_disponibles, (void *) es_suficiente_para);

	if (list_is_empty(huecos_suficientes)) {
		log_info(logger, "No hay suficiente espacio contiguo para crear el segmento, requiere compactación");
//		send_op(socket_cliente, NECESITO_COMPACTAR);
		t_paquete *paquete = crear_paquete(NECESITO_COMPACTAR);
		int rand = 1;
		agregar_int_a_paquete(paquete, rand);
		enviar_paquete(paquete, socket_cliente);
		eliminar_paquete(paquete);
		dinamitar_listas(huecos_disponibles, huecos_suficientes);
		return;
	}

	int base = obtener_base_segun_criterio(huecos_suficientes, tamanio);

	dinamitar_listas(huecos_disponibles, huecos_suficientes);

	t_segmento *seg = crear_segmento(id, base, tamanio);
	guardar_en_bitmap(seg);
	agregar_a_lista(pid, seg);

	log_info(logger, "PID: %d - Crear Segmento: %d - Base: %d - Tamaño: %d", pid, id, base, tamanio);

	send_base_segmento(socket_cliente, base);
}

void send_base_segmento(int socket_cliente, int base) {

	t_paquete *paquete = crear_paquete(SEGMENTO_CREADO);

	agregar_int_a_paquete(paquete, base);

	enviar_paquete(paquete, socket_cliente);
	eliminar_paquete(paquete);
}

void recv_eliminar_segmento(int socket_cliente, void *stream) {

	int offset = 0;

	int pid = deserializar_int(stream, &offset);
	int id = deserializar_int(stream, &offset);

	t_segmento *seg = obtener_segmento(pid, id);
	int base = seg->base;
	int tamanio = seg->tamanio;

	quitar_de_lista(pid, seg);

	liberar_segmento(seg);

	log_info(logger, "PID: %d - Eliminar Segmento: %d - Base: %d - Tamaño: %d", pid, id, base, tamanio);

	t_tabla_segmentos *tabla = tabla_por_pid(pid);
	send_tabla(socket_cliente, tabla);
}

void compactar(int socket_cliente) {

	t_list *segmentos_en_uso = get_segmentos_en_uso(); // lista de t_segmento *

	t_list *contenido_segmentos = get_contenido_segmentos(segmentos_en_uso); // lista de void * (la anterior mapeada)

	for (int i = 0; i < config_memoria.tam_memoria; i++) {
		bitarray_clean_bit(bitmap, config_memoria.tam_segmento_0 + i); // los bits del seg 0 se quedan así
	}

	int cant_segs_en_uso = list_size(segmentos_en_uso);

	for (int j = 0; j < cant_segs_en_uso; j++) {

		t_segmento *seg_viejo = list_get(segmentos_en_uso, j);
		void *contenido = list_get(contenido_segmentos, j);

		int nueva_base = primer_byte_disponible();

		t_segmento *seg_nuevo = crear_segmento(seg_viejo->id, nueva_base, seg_viejo->tamanio);

		guardar_en_bitmap(seg_nuevo);

		memcpy(memoria + seg_nuevo->base, contenido, seg_nuevo->tamanio); // lo "escribimos" en el void *memoria

		// -------------------------------------------------------------------

		bool segmento_tiene_base(t_segmento *segmento) {
			return (segmento->base == seg_viejo->base) && (segmento->id == seg_viejo->id);
		}

		bool tiene_segmento_con_base(t_tabla_segmentos *tabla) {
			return list_any_satisfy(tabla->segmentos, (void *) segmento_tiene_base);
		}

		t_tabla_segmentos *tabla = list_find(tablas_segmentos, (void *) tiene_segmento_con_base);
		int pid = tabla->pid;
		quitar_de_lista(pid, seg_viejo);
		agregar_a_lista(pid, seg_nuevo);

		free(seg_viejo);
		free(contenido);
	}

	list_destroy(segmentos_en_uso);
	list_destroy(contenido_segmentos);

	msleep(config_memoria.retardo_compactacion);
	mostrar_esquema_memoria();

	send_tablas(socket_cliente);
}

void send_tablas(int socket_cliente) {

	t_paquete *paquete = crear_paquete(COMPACTACION_TERMINADA);
	int cant_procesos = list_size(tablas_segmentos);

	agregar_int_a_paquete(paquete, cant_procesos);

	for (int i = 0; i < cant_procesos; i++) {
		t_tabla_segmentos *tabla = list_get(tablas_segmentos, i);
		agregar_tabla_a_paquete(paquete, tabla);
	}

	enviar_paquete(paquete, socket_cliente);
	eliminar_paquete(paquete);
}

void recv_liberar_estructuras(int socket_cliente, void *stream) {

	int offset = 0;

	int pid = deserializar_int(stream, &offset);

	bool mismo_pid(t_tabla_segmentos *tabla) {
		return tabla->pid == pid;
	}

	list_remove_and_destroy_by_condition(tablas_segmentos, (void *) mismo_pid, (void *) destruir_tabla);

	log_info(logger, "Eliminación de Proceso PID: %d", pid);

	send_op(socket_cliente, ESTRUCTURAS_LIBERADAS);
}



// FUNCIONES AUXILIARES - Gero, si queres las pasamos a otro archivo
void recv_leer_de_memoria(int tamanio, int direccion_fisica, int socket_cliente){
	log_info(logger, "Se busca el valor de la direccion fisica: %d", direccion_fisica);
	buscar_valor_enviar(direccion_fisica, tamanio, socket_cliente);
}

void recv_leer_de_memoria_cpu(int tamanio, int direccion_fisica, int socket_cliente){
	log_info(logger, "Se busca el valor de la direccion fisica: %d", direccion_fisica);
	buscar_valor_enviar_cpu(direccion_fisica, tamanio, socket_cliente);
}



void recv_escribir_en_memoria(int direccion_fisica, int tamanio, char* valor, int socket_cliente){
	log_info(logger, "Se escribe el valor <%s> de tamaño <%d> en la direccion fisica: %d", valor, tamanio, direccion_fisica);
	escribir_valor_en_direccion_fisica(socket_cliente, direccion_fisica, tamanio, valor);
}



void buscar_valor_enviar(int direccion_fisica_buscada, int tamanio, int socket_cliente){

	char* valor_buscado = malloc(tamanio);
	memset(valor_buscado,0,tamanio);

	if((memoria + direccion_fisica_buscada) != NULL){

		msleep(config_memoria.retardo_memoria); // Cada vez que accede al espacio memoria debe retrasarse segun config

		memcpy(valor_buscado, memoria + direccion_fisica_buscada, tamanio);

		t_paquete* paquete = malloc(sizeof(t_paquete));
		t_buffer* buffer = malloc(sizeof(t_buffer));

		int tam_val = strlen(valor_buscado) + 1;
		buffer->stream_size = sizeof(int) + tam_val;

		void* stream = malloc(buffer->stream_size);
		int off = 0;

		memcpy(stream + off, &tam_val, sizeof(int));
		off += sizeof(int);
		memcpy(stream + off, valor_buscado, tam_val);
		off += tam_val;

		buffer->stream = stream;

		paquete->buffer = buffer;
		paquete->codigo_operacion = LEIDO;

		void* a_enviar = malloc(sizeof(int) + sizeof(int) + paquete->buffer->stream_size);
		off = 0;

		agregar_a_stream(a_enviar,&off,&(paquete->codigo_operacion),sizeof(int));
		agregar_a_stream(a_enviar,&off,&(paquete->buffer->stream_size),sizeof(int));
		agregar_a_stream(a_enviar,&off,paquete->buffer->stream,paquete->buffer->stream_size);

		send(socket_cliente,a_enviar,sizeof(int) + sizeof(int) + paquete->buffer->stream_size,0);

		free(a_enviar);
		eliminar_paquete(paquete);



		log_info(logger, "Se envia el valor: %s", valor_buscado);
	}
	else{
		t_paquete* paquete = crear_paquete(NO_LEIDO);
		enviar_paquete(paquete, socket_cliente);
	}
}

void buscar_valor_enviar_cpu(int direccion_fisica_buscada, int tamanio, int socket_cliente){

	char* valor_buscado = malloc(tamanio-1);
	memset(valor_buscado,0,tamanio-1);

	if((memoria + direccion_fisica_buscada) != NULL){

		msleep(config_memoria.retardo_memoria); // Cada vez que accede al espacio memoria debe retrasarse segun config

		memcpy(valor_buscado, memoria + direccion_fisica_buscada, tamanio-1);

		t_paquete* paquete = malloc(sizeof(t_paquete));
		t_buffer* buffer = malloc(sizeof(t_buffer));

		int tam_val = strlen(valor_buscado) + 1;
		buffer->stream_size = sizeof(int) + tam_val;

		void* stream = malloc(buffer->stream_size);
		int off = 0;

		memcpy(stream + off, &tam_val, sizeof(int));
		off += sizeof(int);
		memcpy(stream + off, valor_buscado, tam_val);
		off += tam_val;

		buffer->stream = stream;

		paquete->buffer = buffer;
		paquete->codigo_operacion = LEIDO;

		void* a_enviar = malloc(sizeof(int) + sizeof(int) + paquete->buffer->stream_size);
		off = 0;

		agregar_a_stream(a_enviar,&off,&(paquete->codigo_operacion),sizeof(int));
		agregar_a_stream(a_enviar,&off,&(paquete->buffer->stream_size),sizeof(int));
		agregar_a_stream(a_enviar,&off,paquete->buffer->stream,paquete->buffer->stream_size);

		send(socket_cliente,a_enviar,sizeof(int) + sizeof(int) + paquete->buffer->stream_size,0);

		free(a_enviar);
		eliminar_paquete(paquete);



		log_info(logger, "Se envia el valor: %s", valor_buscado);
	}
	else{
		t_paquete* paquete = crear_paquete(NO_LEIDO);
		enviar_paquete(paquete, socket_cliente);
	}
}

void escribir_valor_en_direccion_fisica(int socket_cliente, int direccion_fisica, int tamanio_valor, char* valor){

	log_info(logger, "Dirección física: %d", direccion_fisica);
	log_info(logger, "Valor Registro: %s", valor);

	msleep(config_memoria.retardo_memoria);
	memcpy(memoria + direccion_fisica, valor, tamanio_valor);
	log_info(logger, "%s", valor);

	/*
	char *aux = malloc(tamanio_valor); // ver si se saca
	msleep(config_memoria.retardo_memoria);
	memcpy(aux, memoria + direccion_fisica, tamanio_valor);
	log_info(logger, "%s", aux);
	free(aux);
	*/

//	free(valor);
	send_op(socket_cliente, OK_VALOR_ESCRITO); // agregar en CPU esto de recibir el cod_op
}


void msleep(int tiempo_milisegundos) {
	usleep(tiempo_milisegundos * 1000);
}

int agregar_a_stream(void *stream, int* offset, void *src, int size) {
	memcpy(stream + *offset, src, size);
	*offset += size;
}

void enviar_string_por(op_code codigo_operacion, char* valor, int socket) {
	int tamanio_string = strlen(valor) + 1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream_size = sizeof(int)
			+ tamanio_string;

	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, &tamanio_string, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, valor, tamanio_string);
	offset += tamanio_string;

	buffer->stream = stream;

	paquete->codigo_operacion = LEIDO;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(socket, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}
