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

void atender_kernel(int *kernel_fd) {

	int socket_cliente = *kernel_fd;

	while (1) {
		int cod_op = recibir_operacion(socket_cliente);

		switch(cod_op) {

		case INICIAR_PROCESO:
			recv_nuevo_proceso(socket_cliente);
			break;

		case CREATE_SEGMENT:
			recv_crear_segmento(socket_cliente);
			break;
//
//		case DELETE_SEGMENT:
//			recv_eliminar_segmento(socket_cliente);
//			break;

		case -1:
			log_warning(logger, "Se desconectó el File System!");
			return;
		}
	}
}

void recv_nuevo_proceso(int socket_cliente) {

	int size;
	int offset = 0;
	void *buffer = recibir_buffer(&size, socket_cliente);

	int pid = deserializar_int(buffer, &offset);

	t_tabla_segmentos *nueva_tabla = nueva_tabla_segmentos(pid);

	list_add(tablas_segmentos, nueva_tabla);

	log_info(logger, "Creación de Proceso PID: %d", pid);

	free(buffer);
}

void recv_crear_segmento(int socket_cliente) {

	int size;
	int offset = 0;

	void *buffer = recibir_buffer(&size, socket_cliente);

	int pid = deserializar_int(buffer, &offset);
	int id = deserializar_int(buffer, &offset);
	int tamanio = deserializar_int(buffer, &offset);

	if (! hay_espacio(tamanio)) { // NO hay espacio, no importa compactación
		log_info(logger, "No hay espacio suficiente en memoria para crear el segmento solicitado");
//		send_sin_espacio_disponible(); TODO
		return;
	}

	t_list *huecos_disponibles = buscar_huecos_libres(); // incluidos los que no alcanzan para size

	bool es_suficiente_para(t_segmento *hueco) { // TRAMOYA
		return (hueco->tamanio >= tamanio);
	}

	t_list *huecos_suficientes = list_filter(huecos_disponibles, (void *) es_suficiente_para);

	if (list_is_empty(huecos_suficientes)) {
		log_info(logger, "No hay suficiente espacio contiguo para crear el segmento, requiere compactación");
//		send_necesita_compactacion(); TODO
		return;
	}

	int base = obtener_base_segun_criterio(huecos_suficientes, tamanio);

	dinamitar_listas(huecos_disponibles, huecos_suficientes);

	t_segmento *seg = crear_segmento(id, base, tamanio);
	guardar_en_bitmap(seg);
	agregar_a_lista(pid, seg);

	log_info(logger, "PID: %d - Crear Segmento: %d - Base: %d - Tamaño: %d", pid, id, base, tamanio);

	free(buffer);
}

// AGREGO INICIO DE ATENDER A CPU
void atender_CPU(int *cpu_fd){

	int socket_cliente = *cpu_fd;

	while(1){
		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));
		recv(socket_cliente, &(paquete->codigo_operacion), sizeof(op_code), MSG_WAITALL);
		recv(socket_cliente, &(paquete->buffer->stream_size), sizeof(int), 0);
		paquete->buffer->stream = malloc(paquete->buffer->stream_size);
		recv(socket_cliente, paquete->buffer->stream, paquete->buffer->stream_size, 0);

		int direccion_fisica_buscada = malloc(sizeof(int));
		int tamanio_valor = malloc(sizeof(int));
		char* valor_a_asignar = malloc(sizeof(t_registros));

		switch(paquete->codigo_operacion){
		case LEER_DE_MEMORIA :
			// RECIBO LA DIRECCION FISICA BUSCADA POR CPU, BUSCO SU VALOR ASOCIADO Y LO ENVIO A CPU
			void *stream1 = paquete->buffer->stream;
			memcpy(&(direccion_fisica_buscada), stream1, sizeof(int));
			stream1 += sizeof(int);
			memcpy(&(tamanio_valor), stream1, sizeof(int));
			stream1 += sizeof(int);
			buscar_valor_enviar_CPU(direccion_fisica_buscada, tamanio, socket_cliente);


		case ESCRIBIR_EN_MEMORIA :
			// RECIBO LA DIRECCION FISICA Y EL VALOR A ESCRIBIR EN ELLA
			void *stream2 = paquete->buffer->stream;
			memcpy(&(direccion_fisica_buscada), stream2, sizeof(int));
			stream2 += sizeof(int);
			memcpy(&(tamanio_valor), stream2, sizeof(int));
			stream2 += sizeof(int);
			memcpy(&(valor_a_asignar), stream2, sizeof(t_registros));
			stream2 += sizeof(t_registros);
			escribir_valor_en_direccion_fisica(direccion_fisica_buscada, tamanio_valor, valor_a_asignar);
		}
		free(paquete->buffer->stream);
		free(paquete->buffer);
		free(paquete);
	}
}

// FUNCIONES SIN DESARROLLAR TODAVIA

void buscar_valor_enviar_CPU(int direccion_fisica_buscada, int tamanio, int socket_cliente){
	// Tenemos que ver como buscar una direccion fisica
	// Si encuentra la direccion fisica, debe devolverle a cpu el valor encontrado como un paquete

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete* paquete_valor = malloc(sizeof(t_paquete));

	char* valor_buscado; // le deberia asignar el valor que encontre

	if(memcpy(valor_buscado, memoria + direccion_fisica_buscada, tamanio) != NULL){ // if encuentra direccion fisica
		msleep(config_memoria.retardo_memoria);
		memcpy(valor_buscado, memoria + direccion_fisica_buscada, tamanio);

		buffer->stream_size = sizeof(tamanio);
		void *stream = malloc(buffer->stream_size);
		int offset = 0;
		memcpy(stream + offset, &tamanio, sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &valor_buscado, sizeof(tamanio));
		offset += tamanio;

		buffer->stream = stream;

		paquete_valor->codigo_operacion = LEIDO;
		paquete_valor->buffer = buffer;
	}
	else{
		paquete_valor->codigo_operacion = NO_LEIDO;
		paquete_valor->buffer->stream = NULL;
		paquete_valor->buffer->stream_size = 0;

	}

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete_valor->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete_valor->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete_valor->buffer->stream, paquete_valor->buffer->stream_size);

	send(socket_cliente, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	free(paquete_valor->buffer->stream);
	free(paquete_valor->buffer);
	free(paquete_valor);

}

void escribir_valor_en_direccion_fisica(int direccion_fisica, int tamanio_valor, char* valor){
	// Buscar direccion fisica, le asigna valor que me paso CPU
	// Ver que pasa si no la encuentra !!!

	log_info(logger, "Dirección física: %d", direccion_fisica);
	log_info(logger, "Valor Registro: %s", valor);

	msleep(config_memoria.retardo_memoria);
	memcpy(memoria + direccion_fisica, valor, tamanio_valor);

	char *aux = malloc(tamanio_valor); // ver si se saca
	msleep(config_memoria.retardo_memoria);
	memcpy(aux, memoria + direccion_fisica, tamanio_valor);
	log_info(logger, "%s", aux);

	free(aux);
	free(valor);
	log_info(logger, "OK, se escribio el valor en memoria");// agreamos log para enviar mensaje de ok
}

int agregar_a_stream(void* stream, int* offset, void* src, int size) {
	memcpy(stream + *offset, src, size);
	*offset +=size;
}
