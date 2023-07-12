#include"utils_cpu2.h"
#include <math.h>
#include<shared-2.h>

void ejecutar_CREATE_SEGMENT(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+ 2 + strlen(instruccion[2]);
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;
	memcpy(stream + offset, &instruccion[2], strlen(instruccion[2])+1);
	offset += strlen(instruccion[2])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CREATE_SEGMENT;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_DELETE_SEGMENT(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+1;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = DELETE_SEGMENT;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_EXIT(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = EXIT;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_F_CLOSE(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+1;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = F_CLOSE;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_F_OPEN(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+1;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = F_OPEN;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_F_READ(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel, int fd_memoria, t_config* config){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+ sizeof(int) + strlen(instruccion[2]) + 2;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;
	int direccion_logica = convertirAEntero(instruccion[2]);
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto);
	memcpy(stream + offset, &direccion_fisica, sizeof(int));
	offset += strlen(instruccion[2])+1;
	memcpy(stream + offset, &instruccion[3], strlen(instruccion[3])+1);
	offset += strlen(instruccion[3])+1;
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = F_READ;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_F_SEEK(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+ 2 + strlen(instruccion[2]);
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;
	memcpy(stream + offset, &instruccion[2], strlen(instruccion[2])+1);
	offset += strlen(instruccion[2])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = F_SEEK;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
	}

void ejecutar_F_TRUNCATE(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1]) + strlen(instruccion[2]) + 2;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;
	memcpy(stream + offset, &instruccion[2], strlen(instruccion[2])+1);
	offset += strlen(instruccion[2])+1;
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = F_TRUNCATE;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_F_WRITE(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel, int fd_memoria, t_config* config){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+ sizeof(int) + strlen(instruccion[2]) + 2;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;
	int direccion_logica = convertirAEntero(instruccion[2]);
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto);
	memcpy(stream + offset, &direccion_fisica, sizeof(int));
	offset += strlen(instruccion[2])+1;
	memcpy(stream + offset, &instruccion[3], strlen(instruccion[3])+1);
	offset += strlen(instruccion[3])+1;
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = F_WRITE;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_IO(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel, bool cpu_bloqueada){
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->stream_size = sizeof(int)*6 + contexto->tamanio_instrucciones + contexto->tamanio_segmentos + contexto->tamanio_registros;

	void* stream = malloc(buffer->stream_size);
	int offset = 0;
	memcpy(stream + offset, &contexto->pid, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->tamanio_instrucciones, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->pc, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->tamanio_registros, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->tamanio_segmentos, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;
	memcpy(stream + offset, &contexto->instrucciones, contexto->tamanio_instrucciones);
	offset += contexto->tamanio_instrucciones;

	t_registros* registro_actual = contexto->registros_pcb;
	for(int i=0; i<(contexto->tamanio_registros); i++){
		memcpy(stream + offset, registro_actual, sizeof(t_registros));
		offset += sizeof(t_registros);
		registro_actual++;
	}

	for(int a=0; a<(contexto->tamanio_segmentos); a++){
		t_segmento* segmento_actual = list_get(contexto->tabla_segmentos, a);
		serializar_segmentos(segmento_actual, stream, offset);
		segmento_actual++;
	}

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = IO;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_MOV_IN(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_memoria, t_config* config){
	int direccion_logica = convertirAEntero(instruccion[2]);
	asignar_valor_a_registro(mmu_valor_buscado(contexto, direccion_logica, fd_memoria, config), instruccion[1], contexto->registros_pcb);
}

void ejecutar_MOV_OUT(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_memoria, t_config* config){
	int direccion_logica = convertirAEntero(instruccion[1]);
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto);
	escribir_en_memoria(direccion_fisica, instruccion[2], fd_memoria);
}

void ejecutar_SET(char** instruccion, t_contexto_de_ejecucion* contexto){
	asignar_valor_a_registro(instruccion[2], instruccion[1], contexto->registros_pcb);
}

void ejecutar_SIGNAL(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+1;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = SIGNAL;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_WAIT(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+1;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = WAIT;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_YIELD(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = YIELD;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	recibir_contexto(fd_kernel, contexto);
}





