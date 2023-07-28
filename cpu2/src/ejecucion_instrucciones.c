#include"utils_cpu2.h"
#include <math.h>
#include<shared-2.h>

void ejecutar_CREATE_SEGMENT(char* instruccion_grande, t_contexto_de_ejecucion* contexto, int fd_kernel){
	char** instruccion = string_split(instruccion_grande, " ");

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int id_segmento = atoi(instruccion[1]);
	int tamanio_segmento = atoi(instruccion[2]);
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 5 //PID, PC, TAMANIO_INSTRUCCIONES, TAMANIO_SEGMENTO(SEGUNDO PARAMETRO), ID_SEGMENTO
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4
			+ sizeof(int)
			+ (cantidad_segmentos * 3 * sizeof(int));;

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	//---------------------SERIALIZACIÓN DE LOS PARÁMETROS DE LA FUNCIÓN CREATE_SEGMENT
	memcpy(stream + offset, &id_segmento, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &tamanio_segmento, sizeof(int));
	offset += sizeof(int);

	buffer->stream = stream;

	paquete->codigo_operacion = CREATE_SEGMENT;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void ejecutar_DELETE_SEGMENT(char* instruccion_grande, t_contexto_de_ejecucion* contexto, int fd_kernel){
	char** instruccion = string_split(instruccion_grande, " ");

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int id_segmento = atoi(instruccion[1]);
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 4 //PID, PC, TAMANIO_INSTRUCCIONES, TAMANIO_SEGMENTO(SEGUNDO PARAMETRO)
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4
			+ sizeof(int)
			+ (cantidad_segmentos * 3 * sizeof(int));;

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	//---------------------SERIALIZACIÓN DE LOS PARÁMETROS DE LA FUNCIÓN CREATE_SEGMENT
	memcpy(stream + offset, &id_segmento, sizeof(int));
	offset += sizeof(int);

	buffer->stream = stream;

	paquete->codigo_operacion = DELETE_SEGMENT;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void ejecutar_EXIT(char* instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 3 //PID, PC, TAMANIO_INSTRUCCIONES
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4
			+ sizeof(int)
			+ (cantidad_segmentos * 3 * sizeof(int));

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	buffer->stream = stream;

	paquete->codigo_operacion = EXIT;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion),
			sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size),
			sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream,
			paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void ejecutar_F_CLOSE(char* instruccion_grande, t_contexto_de_ejecucion* contexto, int fd_kernel) {
	char** instruccion = string_split(instruccion_grande, " ");

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int tamanio_nombre_archivo = strlen(instruccion[1]) + 1;
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 4 //PID, PC, TAMANIO_INSTRUCCIONES, TAMANIO_NOMBRE_ARCHIVO
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_nombre_archivo
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4
			+ sizeof(int)
			+ (cantidad_segmentos * 3 * sizeof(int));;

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	//---SERIALIZACION DEL PARAMETRO DE F_CLOSE
	memcpy(stream + offset, &tamanio_nombre_archivo, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, instruccion[1], tamanio_nombre_archivo);
	offset += tamanio_nombre_archivo;

	buffer->stream = stream;

	paquete->codigo_operacion = F_CLOSE;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void ejecutar_F_OPEN(char* instruccion_grande, t_contexto_de_ejecucion* contexto, int fd_kernel){
	char** instruccion = string_split(instruccion_grande, " ");

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int tamanio_nombre_archivo = strlen(instruccion[1]) + 1;
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 4 //PID, PC, TAMANIO_INSTRUCCIONES, TAMANIO_NOMBRE_ARCHIVO
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_nombre_archivo
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4
			+ sizeof(int)
			+ (cantidad_segmentos * 3 * sizeof(int));

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	//---SERIALIZACION DEL PARAMETRO DE F_CLOSE
	memcpy(stream + offset, &tamanio_nombre_archivo, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, instruccion[1], tamanio_nombre_archivo);
	offset += tamanio_nombre_archivo;

	buffer->stream = stream;

	paquete->codigo_operacion = F_OPEN;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void ejecutar_F_READ(char* instruccion_grande, t_contexto_de_ejecucion* contexto, int fd_kernel, int fd_memoria, t_config* config, t_log* logger_principal){
	char **instruccion = string_split(instruccion_grande, " ");

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int tamanio_nombre_archivo = strlen(instruccion[1]) + 1;
	int direccion_logica = atoi(instruccion[2]);
	int cantidad_bytes = atoi(instruccion[3]);
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto, logger_principal, fd_kernel, cantidad_bytes);
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 6 //PID, PC, TAMANIO_INSTRUCCIONES, TAMANIO_NOMBRE_ARCHIVO, DIRECCION_LOGICA, CANTIDAD_BYTES
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_nombre_archivo
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4
			+ sizeof(int)
			+ (cantidad_segmentos * 3 * sizeof(int));;

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	//---SERIALIZACION DE LOS PARAMETROS DE F_READ
	memcpy(stream + offset, &tamanio_nombre_archivo, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, instruccion[1], tamanio_nombre_archivo);
	offset += tamanio_nombre_archivo;
	memcpy(stream + offset, &direccion_fisica, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &cantidad_bytes, sizeof(int));

	buffer->stream = stream;

	paquete->codigo_operacion = F_READ;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void ejecutar_F_SEEK(char* instruccion_grande, t_contexto_de_ejecucion* contexto, int fd_kernel) {
	char **instruccion = string_split(instruccion_grande, " ");

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int tamanio_nombre_archivo = strlen(instruccion[1]) + 1;
	int puntero_en_bytes = atoi(instruccion[2]);
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 5 //PID, PC, TAMANIO_INSTRUCCIONES, TAMANIO_NOMBRE_ARCHIVO, SEGUNDO PARAMETRO DE LA INSTRUCCION
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_nombre_archivo
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4
			+ sizeof(int)
			+ (cantidad_segmentos * 3 * sizeof(int));;

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	//---SERIALIZACION DE LOS PARAMETROS DE F_SEEK
	memcpy(stream + offset, &tamanio_nombre_archivo, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, instruccion[1], tamanio_nombre_archivo);
	offset += tamanio_nombre_archivo;
	memcpy(stream + offset, &puntero_en_bytes, sizeof(int));
	offset += sizeof(int);

	buffer->stream = stream;

	paquete->codigo_operacion = F_SEEK;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void ejecutar_F_TRUNCATE(char* instruccion_grande, t_contexto_de_ejecucion* contexto, int fd_kernel){
	char **instruccion = string_split(instruccion_grande, " ");

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int tamanio_nombre_archivo = strlen(instruccion[1]) + 1;
	int nuevo_tamanio = atoi(instruccion[2]);
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 5 //PID, PC, TAMANIO_INSTRUCCIONES, TAMANIO_NOMBRE_ARCHIVO, SEGUNDO PARAMETRO DE LA INSTRUCCION
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_nombre_archivo
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4
			+ sizeof(int)
			+ (cantidad_segmentos * 3 * sizeof(int));;

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	//---SERIALIZACION DE LOS PARAMETROS DE F_TRUNCATE
	memcpy(stream + offset, &tamanio_nombre_archivo, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, instruccion[1], tamanio_nombre_archivo);
	offset += tamanio_nombre_archivo;
	memcpy(stream + offset, &nuevo_tamanio, sizeof(int));
	offset += sizeof(int);

	buffer->stream = stream;

	paquete->codigo_operacion = F_TRUNCATE;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void ejecutar_F_WRITE(char* instruccion_grande, t_contexto_de_ejecucion* contexto, int fd_kernel, int fd_memoria, t_config* config, t_log* logger_principal){
	char **instruccion = string_split(instruccion_grande, " ");

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int tamanio_nombre_archivo = strlen(instruccion[1]) + 1;
	int direccion_logica = atoi(instruccion[2]);
	int cantidad_bytes = atoi(instruccion[3]);
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto, logger_principal, fd_kernel, cantidad_bytes);
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 6 //PID, PC, TAMANIO_INSTRUCCIONES, TAMANIO_NOMBRE_ARCHIVO, DIRECCION_LOGICA, CANTIDAD_BYTES
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
	+ tamanio_nombre_archivo
	+ tamanio_instrucciones
	+ tamanio_registro_chico * 4
	+ tamanio_registro_mediano * 4
	+ tamanio_registro_grande * 4
	+ sizeof(int)
	+ (cantidad_segmentos * 3 * sizeof(int));;

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	//---SERIALIZACION DE LOS PARAMETROS DE F_READ
	memcpy(stream + offset, &tamanio_nombre_archivo, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, instruccion[1], tamanio_nombre_archivo);
	offset += tamanio_nombre_archivo;
	memcpy(stream + offset, &direccion_fisica, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &cantidad_bytes, sizeof(int));

	buffer->stream = stream;

	paquete->codigo_operacion = F_WRITE;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void ejecutar_IO(char* instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel) {
	char** instruccion_con_parametros = string_split(instruccion, " ");

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 4 //PID, PC, TAMANIO_INSTRUCCIONES, PARAMETRO IO
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4
			+ sizeof(int)
			+ (cantidad_segmentos * 3 * sizeof(int));;

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	//TODO ACORDARSE QUE LOS PARÁMETROS DE LA INSTRUCCIÓN SE DEVUELVEN AL FINAL
	int parametro_io = atoi(instruccion_con_parametros[1]);
	printf("PID: <%d> irá a IO por %d segundos\n", contexto->pid, parametro_io);

	memcpy(stream + offset, &parametro_io, sizeof(int));
	offset += sizeof(int);

	buffer->stream = stream;

	paquete->codigo_operacion = IO;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

int ejecutar_MOV_IN(char* instruccion_grande, t_contexto_de_ejecucion* contexto, int fd_memoria, t_config* config, t_log* logger_principal, int fd_kernel) {
	char** instruccion = string_split(instruccion_grande, " ");

	int direccion_logica = atoi(instruccion[2]);
	int tam_registro = tamanio_registro(instruccion[1]);
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto, logger_principal, fd_kernel, tam_registro);

	if(direccion_fisica != -1) {
		asignar_valor_a_registro(mmu_valor_buscado(contexto, direccion_logica, tam_registro, fd_memoria, config, logger_principal, fd_kernel, direccion_fisica), instruccion[1]);
		return 1;
	} else {
		return 0;
	}
}

int ejecutar_MOV_OUT(char* instruccion_grande, t_contexto_de_ejecucion* contexto, int fd_memoria, t_config* config, t_log* logger_principal, int fd_kernel) {
	char** instruccion = string_split(instruccion_grande, " ");

	int direccion_logica = atoi(instruccion[1]);
	int tamanio = tamanio_registro(instruccion[2]);

	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto, logger_principal, fd_kernel, tamanio);

//	char* valor = obtener_valor_de_registro(instruccion[2]);

	if(direccion_fisica != -1) {
	escribir_en_memoria(direccion_fisica,obtener_valor_de_registro(instruccion[2]), fd_memoria, logger_principal, contexto, direccion_logica, config);

	return 1;
	} else {
		return 0;
	}
}

void ejecutar_SET(char* instruccion_grande, t_contexto_de_ejecucion* contexto) {
	char** instruccion = string_split(instruccion_grande, " ");

	asignar_valor_a_registro(instruccion[2], instruccion[1]);
}

void ejecutar_SIGNAL(char* instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel) {
	char **instruccion_con_parametros = string_split(instruccion, " ");

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_parametro = strlen(instruccion_con_parametros[1]) + 1;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 4 //PID, PC, TAMANIO_INSTRUCCIONES
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
	+ tamanio_instrucciones
	+ tamanio_parametro
	+ tamanio_registro_chico * 4
	+ tamanio_registro_mediano * 4
	+ tamanio_registro_grande * 4
	+ sizeof(int)
	+ (cantidad_segmentos * 3 * sizeof(int));;

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	//SERIALIZACION DEL PARAMETRO DE SIGNAL
	memcpy(stream + offset, &tamanio_parametro, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, instruccion_con_parametros[1], tamanio_parametro);
	offset += tamanio_parametro;

	buffer->stream = stream;

	paquete->codigo_operacion = SIGNAL;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void ejecutar_WAIT(char* instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel) {
	char** instruccion_con_parametros = string_split(instruccion, " ");

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_parametro = strlen(instruccion_con_parametros[1]) + 1;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 4 //PID, PC, TAMANIO_INSTRUCCIONES
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_instrucciones
			+ tamanio_parametro
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4
			+ sizeof(int)
			+ (cantidad_segmentos * 3 * sizeof(int));;

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	//SERIALIZACION DEL PARAMETRO DE WAIT
	memcpy(stream + offset, &tamanio_parametro, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, instruccion_con_parametros[1], tamanio_parametro);
	offset += tamanio_parametro;

	buffer->stream = stream;

	paquete->codigo_operacion = WAIT;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void ejecutar_YIELD(char* instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel) {

	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_paquete* paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 3 //PID, PC, TAMANIO_INSTRUCCIONES
	//		+ sizeof(t_registros)
//			+ tamanio_segmentos
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4
			+ sizeof(int) //el entero que dice cuántos segmentos hay
			+ (cantidad_segmentos * 3 * sizeof(int)); //el peso total de cada segmento

	void* stream = malloc(buffer->stream_size);
	int offset = 0;
//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;


	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	buffer->stream = stream;

	paquete->codigo_operacion = YIELD;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

int agregar_a_stream(void* stream, int* offset, void* src, int size) {
	memcpy(stream + *offset, src, size);
	*offset +=size;
}
