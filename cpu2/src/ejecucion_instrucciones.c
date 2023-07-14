#include"utils_cpu2.h"
#include <math.h>
#include<shared-2.h>

t_log* logger_principal;

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
}

void ejecutar_EXIT(char* instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;

	buffer->stream_size = sizeof(int) * 3 //PID, PC, TAMANIO_INSTRUCCIONES
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4;

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
}

void ejecutar_IO(char* instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel, bool cpu_bloqueada) {
	char** instruccion_con_parametros = string_split(instruccion, " ");

	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;

	buffer->stream_size = sizeof(int) * 4 //PID, PC, TAMANIO_INSTRUCCIONES, PARAMETRO IO
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4;

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

void ejecutar_MOV_IN(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_memoria, t_config* config){
	int direccion_logica = convertirAEntero(instruccion[2]);
	asignar_valor_a_registro(mmu_valor_buscado(contexto, direccion_logica, fd_memoria, config), instruccion[1]);
}

void ejecutar_MOV_OUT(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_memoria, t_config* config){
	int direccion_logica = convertirAEntero(instruccion[1]);
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto);
	escribir_en_memoria(direccion_fisica, instruccion[2], fd_memoria);
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

	buffer->stream_size = sizeof(int) * 3 //PID, PC, TAMANIO_INSTRUCCIONES
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
	+ tamanio_instrucciones
	+ tamanio_parametro
	+ tamanio_registro_chico * 4
	+ tamanio_registro_mediano * 4
	+ tamanio_registro_grande * 4;

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

	buffer->stream_size = sizeof(int) * 3 //PID, PC, TAMANIO_INSTRUCCIONES
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
			+ tamanio_instrucciones
			+ tamanio_parametro
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4;

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

	buffer->stream_size = sizeof(int) * 3 //PID, PC, TAMANIO_INSTRUCCIONES
	//		+ sizeof(t_registros)
//			+ tamanio_segmentos
			+ tamanio_instrucciones
			+ tamanio_registro_chico * 4
			+ tamanio_registro_mediano * 4
			+ tamanio_registro_grande * 4;

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



