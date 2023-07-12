#include"utils_kernel2.h"
#include<shared-2.h>

//Deserealización de Strings, con esta función recibimos las instrucciones desde Consola
char* deserializar_string(t_buffer* buffer) {
	char* string = malloc(buffer->stream_size);
	memset(string, 0, buffer->stream_size);

	void* stream = buffer->stream;
	memcpy(string, stream, buffer->stream_size);

	//log_info(logger_kernel, "Las instrucciones que llegaron son: %s", string);

	return string;
}

//Serialización y deserialización de PCBs
void enviar_pcb(int socket_servidor, t_pcb* pcb) {

	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));

	int tamanio_instrucciones = pcb->tamanio_instrucciones;

	buffer->stream_size = sizeof(int) * 3
//			+ sizeof(t_registros)
//			+ tamanio_segmentos  <-- Agregar el tamaño de los segmentos cuando sepamos que carajo es
			+ tamanio_instrucciones; //El tamaño del String con todas las instrucciones

	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, &(pcb->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(pcb->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, pcb->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;
	memcpy(stream + offset, &(pcb->pc), sizeof(int));
	offset += sizeof(int);
//	memcpy(stream + offset, &(pcb->tamanio), sizeof(int));
//	offset += sizeof(int);
//	memcpy(stream + offset, &(pcb->registros), sizeof(t_registros));
//	offset += sizeof(t_registros);
//	memcpy(stream + offset, &(pcb->tamanio_segmentos), sizeof(int));
//	offset += sizeof(int);
//	memcpy(stream + offset, pcb->segmentos, pcb->tamanio_segmentos);

	buffer->stream = stream;

	paquete->codigo_operacion = CONTEXTO_DE_EJECUCION;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(socket_servidor, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

int agregar_a_stream(void *stream, int* offset, void *src, int size) {
	memcpy(stream + *offset, src, size);
	*offset += size;
}

t_pcb* recibir_pcb(int socket_servidor) {

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	recv(socket_servidor, &(paquete->codigo_operacion), sizeof(int), 0);

	recv(socket_servidor, &(paquete->buffer->stream_size), sizeof(int), 0);
	paquete->buffer->stream = malloc(paquete->buffer->stream_size);
	recv(socket_servidor, paquete->buffer->stream, paquete->buffer->stream_size, 0);


	return 0;
}

t_pcb* deserializar_pcb(t_buffer* buffer) {
	t_pcb* pcb = malloc(sizeof(t_pcb));

	void* stream = buffer->stream;

	memcpy(&(pcb->pid), stream, sizeof(int));
	stream += sizeof(int);
	memcpy(&(pcb->tamanio_instrucciones), stream, sizeof(int));
	stream += sizeof(int);
	pcb->instrucciones = malloc(pcb->tamanio_instrucciones);
	memcpy(pcb->instrucciones, stream, pcb->tamanio_instrucciones);
	stream += pcb->tamanio_instrucciones;
	memcpy(&(pcb->estado), stream, sizeof(estado_proceso));
	stream += sizeof(estado_proceso);
	memcpy(&(pcb->pc), stream, sizeof(int));
	stream += sizeof(int);
	//memcpy(&(pcb->tamanio), stream, sizeof(int));
	//stream += sizeof(int);
	memcpy(&(pcb->registros), stream, sizeof(t_registros));

	return pcb;
}

void avisar_a_modulo(int socket, op_code codigo) {
	enviar_datos(socket, &codigo, sizeof(op_code));
}

//Funciones de deserialización de estructuras que ya no se usan, como t_instruccion (NO ANDAN DEL TODO BIEN)
t_list *deserializar_instrucciones(t_list *datos, int longitud_datos) {
	t_list *instrucciones = list_create();


  	for(int i = 0; i < longitud_datos; i += 4) {
  		t_instruccion *instruccion_recibida = malloc(sizeof(t_instruccion));
  		instruccion_recibida->nombre = *(op_code *)list_get(datos, i);
  		instruccion_recibida->parametro_1 = list_get(datos, i + 1);
  		instruccion_recibida->parametro_2 = list_get(datos, i + 2);
  		instruccion_recibida->parametro_3 = list_get(datos, i + 3);
  		printf("%d %s %s %s\n", instruccion_recibida->nombre, instruccion_recibida->parametro_1, instruccion_recibida->parametro_2, instruccion_recibida->parametro_3);
  		list_add(instrucciones, instruccion_recibida);
  	}

  	return instrucciones;
}

t_consola *deserializar_consola(char* instrucciones) {

  	t_consola *consola = malloc(sizeof(t_consola));

  	consola->tamanio_proceso = strlen(instrucciones);

  	log_info(logger_kernel, "\n El tamanio del proceso es: %d", consola->tamanio_proceso);

  	consola->instrucciones = string_duplicate(instrucciones);//deserializar_instrucciones(datos, list_size(datos));

  	free(instrucciones);

  	return consola;
}

char* recibir_instrucciones_como_string(int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));


	recv(socket_cliente, &(paquete->buffer->stream_size), sizeof(uint32_t), 0);
	paquete->buffer->stream = malloc(paquete->buffer->stream_size);
	recv(socket_cliente, paquete->buffer->stream, paquete->buffer->stream_size, 0);

	char* mensaje_recibido = malloc(paquete->buffer->stream_size);
	void* stream = paquete->buffer->stream;

	memcpy(mensaje_recibido, stream, paquete->buffer->stream_size);

	mensaje_recibido = string_duplicate(paquete->buffer->stream);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	return mensaje_recibido;
}

void recibir_instruccion_serializada(int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	recv(socket_cliente, &(paquete->buffer->stream_size), sizeof(uint32_t), 0);

	paquete->buffer->stream = malloc(paquete->buffer->stream_size);

	recv(socket_cliente, paquete->buffer->stream, paquete->buffer->stream_size, 0);

	t_instruccion* instruccion_recibida = malloc(sizeof(t_instruccion));

	void* stream = paquete->buffer->stream;
	int desplazamiento = 0;

	memcpy(&(instruccion_recibida->nombre), stream, sizeof(op_code));
	stream += sizeof(op_code);
	printf("\nLa isntruccion recibida fue: %d, ", instruccion_recibida->nombre);

	memcpy(&(instruccion_recibida->parametro_1_length), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(instruccion_recibida->parametro_1, stream, instruccion_recibida->parametro_1_length);
	desplazamiento = (strlen(instruccion_recibida->parametro_1) + 1);
	stream += desplazamiento;
	printf("%s, ", instruccion_recibida->parametro_1);

	memcpy(&(instruccion_recibida->parametro_2_length), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(instruccion_recibida->parametro_2, stream, instruccion_recibida->parametro_2_length);
	desplazamiento = (strlen(instruccion_recibida->parametro_2) + 1);
	printf("El largo del string fue de %d, debería ser de %d", desplazamiento, instruccion_recibida->parametro_2_length);
	stream += instruccion_recibida->parametro_2_length;

	printf("%s, ", instruccion_recibida->parametro_2);
	memcpy(&(instruccion_recibida->parametro_3_length), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(instruccion_recibida->parametro_3, stream, instruccion_recibida->parametro_3_length);
	printf("%s", instruccion_recibida->parametro_3);

	printf("\nLa isntruccion recibida fue: %d, %s, %s, %s", instruccion_recibida->nombre, instruccion_recibida->parametro_1, instruccion_recibida->parametro_2, instruccion_recibida->parametro_3);
}
