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

		case DELETE_SEGMENT:
			recv_eliminar_segmento(socket_cliente);
			break;

		case COMPACTAR:
			// compactar(); TODO
			log_info(logger, "Inicia compactación");
			break;

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

	send_tabla(socket_cliente, nueva_tabla);

	free(buffer);
}

void send_tabla(int socket_cliente, t_tabla_segmentos *tabla) {

	t_paquete *paquete = crear_paquete(NUEVA_TABLA);

	agregar_tabla_a_paquete(paquete, tabla);

	enviar_paquete(paquete, socket_cliente);
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
		send_op(socket_cliente, OUT_OF_MEMORY);
		free(buffer);
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
		free(buffer);
		return;
	}

	int base = obtener_base_segun_criterio(huecos_suficientes, tamanio);

	dinamitar_listas(huecos_disponibles, huecos_suficientes);

	t_segmento *seg = crear_segmento(id, base, tamanio);
	guardar_en_bitmap(seg);
	agregar_a_lista(pid, seg);

	log_info(logger, "PID: %d - Crear Segmento: %d - Base: %d - Tamaño: %d", pid, id, base, tamanio);

	send_base_segmento(socket_cliente, base);

	free(buffer);
}

void send_base_segmento(int socket_cliente, int base) {

	t_paquete *paquete = crear_paquete(SEGMENTO_CREADO);

	agregar_int_a_paquete(paquete, base);

	enviar_paquete(paquete, socket_cliente);
}

void recv_eliminar_segmento(int socket_cliente) {

	int size;
	int offset = 0;

	void *buffer = recibir_buffer(&size, socket_cliente);

	int pid = deserializar_int(buffer, &offset);
	int id = deserializar_int(buffer, &offset);

	t_segmento *seg = obtener_segmento(pid, id);
	int base = seg->base;
	int tamanio = seg->tamanio;

	quitar_de_lista(pid, seg);

	liberar_segmento(seg);

	log_info(logger, "PID: %d - Eliminar Segmento: %d - Base: %d - Tamaño: %d", pid, id, base, tamanio);

	t_tabla_segmentos *tabla = tabla_por_pid(pid);
	send_tabla(socket_cliente, tabla);

	free(buffer);
}

// AGREGO INICIO DE ATENDER A CPU
void atender_CPU(int *cpu_fd){

	int socket_cliente = *cpu_fd;

	while(1){
		int cod_op = recibir_operacion(socket_cliente);

		/*
		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));
		recv(socket_cliente, &(paquete->codigo_operacion), sizeof(op_code), MSG_WAITALL);
		recv(socket_cliente, &(paquete->buffer->stream_size), sizeof(int), 0);
		paquete->buffer->stream = malloc(paquete->buffer->stream_size);
		recv(socket_cliente, paquete->buffer->stream, paquete->buffer->stream_size, 0);
		*/

		int direccion_fisica_buscada = malloc(sizeof(int));
		int tamanio_valor = malloc(sizeof(int));
		char* valor_a_asignar = malloc(tamanio_valor);

		switch(cod_op){
		case LEER_DE_MEMORIA :
			recv_leer_de_memoria(socket_cliente);
			break;


		case ESCRIBIR_EN_MEMORIA :
			recv_escribir_en_memoria(socket_cliente);
			break;
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

	buscar_valor_enviar_CPU(direccion_fisica, tamanio, socket_cliente);
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

	escribir_valor_en_direccion_fisica(direccion_fisica, tamanio, valor);
	log_info(logger, "Se escribe el valor de la direccion fisica: %d", direccion_fisica);

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




void escribir_valor_en_direccion_fisica(int direccion_fisica, int tamanio_valor, char* valor){

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
	log_info(logger, "OK, se escribio el valor en memoria");
}

//No hace falta porque en la config lo contempla en milisegundos

/*
void msleep(int tiempo_microsegundos) {
	usleep(tiempo_microsegundos * 1000);
}*/
