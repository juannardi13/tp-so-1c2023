#include"utils_kernel2.h"
#include<shared-2.h>

t_list *deserializar_instrucciones(t_list *datos, int longitud_datos) {
	t_list *instrucciones = list_create();


  	for(int i = 0; i < longitud_datos; i += 4) {
  		t_instruccion *instruccion_recibida = malloc(sizeof(t_instruccion));
  		instruccion_recibida->nombre = *(nombre_instruccion *)list_get(datos, i);
  		instruccion_recibida->parametro_1 = list_get(datos, i + 1);
  		instruccion_recibida->parametro_2 = list_get(datos, i + 2);
  		instruccion_recibida->parametro_3 = list_get(datos, i + 3);
  		printf("%d %s %s %s\n", instruccion_recibida->nombre, instruccion_recibida->parametro_1, instruccion_recibida->parametro_2, instruccion_recibida->parametro_3);
  		list_add(instrucciones, instruccion_recibida);
  	}

  	return instrucciones;
}

t_consola *deserializar_consola(int  socket_cliente) {

	t_list *datos = recibir_paquete(socket_cliente);

  	t_consola *consola = malloc(sizeof(t_consola));

  	consola->tamanio_proceso = *(uint32_t *)list_remove(datos, 0);

  	printf("\n El tamanio del proceso es: %d", consola->tamanio_proceso);

  	consola->instrucciones = deserializar_instrucciones(datos, list_size(datos));

  	return consola;
}

char* recibir_instrucciones_como_string(int socket_cliente, t_log* logger) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));


	recv(socket_cliente, &(paquete->buffer->stream_size), sizeof(uint32_t), 0);
	paquete->buffer->stream = malloc(paquete->buffer->stream_size);
	recv(socket_cliente, paquete->buffer->stream, paquete->buffer->stream_size, 0);

	char* mensaje_recibido = malloc(paquete->buffer->stream_size);
	void* stream = paquete->buffer->stream;

	memcpy(mensaje_recibido, stream, paquete->buffer->stream_size);

	mensaje_recibido = string_duplicate(paquete->buffer->stream);

	log_warning(logger, "%s", mensaje_recibido);

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

	memcpy(&(instruccion_recibida->nombre), stream, sizeof(nombre_instruccion));
	stream += sizeof(nombre_instruccion);
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
