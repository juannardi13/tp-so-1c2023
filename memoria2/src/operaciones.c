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

//void atender_kernel(int *kernel_fd) {
//
//	int socket_cliente = *kernel_fd;
//
//	while (1) {
//		int cod_op = recibir_operacion(socket_cliente);
//
//		switch(cod_op) {
//
//		/*case INICIAR_PROCESO:
//			recv_nuevo_proceso(socket_cliente);
//			break;*/
//
//		case CREATE_SEGMENT:
//			recv_crear_segmento(socket_cliente);
//			break;
//
//		case DELETE_SEGMENT:
//			recv_eliminar_segmento(socket_cliente);
//			break;
//
//		case COMPACTAR:
//			log_info(logger, "Inicia compactación");
//			compactar(socket_cliente);
//			break;
//
//		case -1:
//			log_warning(logger, "Se desconectó el File System!");
//			return;
//		}
//	}
//}

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
}

void recv_crear_segmento(int socket_cliente, void *stream) {

	int offset = 0;

	int pid = deserializar_int(stream, &offset);
	int id = deserializar_int(stream, &offset);
	int tamanio = deserializar_int(stream, &offset);

	if (! hay_espacio(tamanio)) { // NO hay espacio, no importa compactación
		log_info(logger, "No hay espacio suficiente en memoria para crear el segmento solicitado");
		send_op(socket_cliente, OUT_OF_MEMORY);
		return;
	}

	t_list *huecos_disponibles = buscar_huecos_libres(); // incluidos los que no alcanzan para size

	bool es_suficiente_para(t_segmento *hueco) { // TRAMOYA
		return (hueco->tamanio >= tamanio);
	}

	t_list *huecos_suficientes = list_filter(huecos_disponibles, (void *) es_suficiente_para);

	if (list_is_empty(huecos_suficientes)) {
		log_info(logger, "No hay suficiente espacio contiguo para crear el segmento, requiere compactación");
		send_op(socket_cliente, NECESITO_COMPACTAR);
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
}

void recv_eliminar_segmento(int socket_cliente, void *stream) {

	int offset_ = 0;

	int pid = deserializar_int(stream, &offset_);
	int id = deserializar_int(stream, &offset_);

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
}

// AGREGO INICIO DE ATENDER A CPU
void atender_CPU(int *cpu_fd){

	int socket_cliente = *cpu_fd;

	while(1){
		//int cod_op = recibir_operacion(socket_cliente);


		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));
		recv(socket_cliente, &(paquete->codigo_operacion), sizeof(op_code), MSG_WAITALL);
		recv(socket_cliente, &(paquete->buffer->stream_size), sizeof(int), 0);
		paquete->buffer->stream = malloc(paquete->buffer->stream_size);
		recv(socket_cliente, paquete->buffer->stream, paquete->buffer->stream_size, 0);

		void* stream = paquete->buffer->stream;

		switch(paquete->codigo_operacion){
		case LEER_DE_MEMORIA :
			//recv_leer_de_memoria(socket_cliente);
			int direccion_fisica_a_leer;
			int tamanio_valor_a_leer;

			memcpy(&direccion_fisica_a_leer, stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&tamanio_valor_a_leer, stream, sizeof(int));
			stream += sizeof(int);

			log_info(logger, "CPU quiere leer en la dirección <%d> un tamaño de <%d>", direccion_fisica_a_leer, tamanio_valor_a_leer);

			//TODO acá van las operaciones para obtener el valor de la dirección física.

			//Esto de acá es simplemente para probarlo
			char* valor = malloc(tamanio_valor_a_leer);

			enviar_string_por(LEIDO, valor, socket_cliente);
			break;
		case ESCRIBIR_EN_MEMORIA :
			recv_escribir_en_memoria(socket_cliente);
			break;
		default:
			log_error(logger, "[ERROR] Operación desconocida, arreglate hermano.");
		}
	}
}


// FUNCIONES AUXILIARES - Gero, si queres las pasamos a otro archivo
void recv_leer_de_memoria(int socket_cliente){

	int size;
	int offset = 0;

	void *buffer = recibir_buffer(&size, socket_cliente);

	int direccion_fisica = deserializar_int(buffer, &offset);
	int tamanio = deserializar_int(buffer, &offset);

	//buscar_valor_enviar_CPU(direccion_fisica, tamanio, socket_cliente);
	log_info(logger, "Se busca el valor de la direccion fisica: %d", direccion_fisica);

	free(buffer);
}

void recv_escribir_en_memoria(int socket_cliente){
	int size;
	int offset = 0;

	void *buffer = recibir_buffer(&size, socket_cliente);

	int direccion_fisica = deserializar_int(buffer, &offset);
	int tamanio = deserializar_int(buffer, &offset);
	char* valor = deserializar_string(tamanio, buffer, &offset);

	log_info(logger, "Se escribe el valor <%s> de tamaño <%d> en la direccion fisica: %d", valor, tamanio, direccion_fisica);
//	escribir_valor_en_direccion_fisica(direccion_fisica, tamanio, valor);

	free(buffer);
}



void buscar_valor_enviar_CPU(int direccion_fisica_buscada, int tamanio, int socket_cliente){

	char* valor_buscado;

	if((memoria + direccion_fisica_buscada) != NULL){
		usleep(config_memoria.retardo_memoria); // Cada vez que accede al espacio memoria debe retrasarse segun config
		memcpy(valor_buscado, memoria + direccion_fisica_buscada, tamanio);
		t_paquete* paquete = crear_paquete(LEIDO);
		agregar_int_a_paquete(paquete, tamanio);
		agregar_string_a_paquete(paquete, valor_buscado, tamanio+1);
		enviar_paquete(paquete, socket_cliente);
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

	usleep(config_memoria.retardo_memoria);
	memcpy(memoria + direccion_fisica, valor, tamanio_valor);

	char *aux = malloc(tamanio_valor); // ver si se saca
	usleep(config_memoria.retardo_memoria);
	memcpy(aux, memoria + direccion_fisica, tamanio_valor);
	log_info(logger, "%s", aux);

	free(aux);
	free(valor);
	send_op(socket_cliente, OK_VALOR_ESCRITO); // agregar en CPU esto de recibir el cod_op
	log_info(logger, "OK, se escribio el valor en memoria");
}

//No hace falta porque en la config lo contempla en milisegundos

/*
void msleep(int tiempo_microsegundos) {
	usleep(tiempo_microsegundos * 1000);
}*/

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
