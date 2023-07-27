#include"utils_kernel2.h"
#include<shared-2.h>

t_dictionary* segmentos;

void asignar_segmentos_de_memoria(t_pcb* pcb) {

	enviar_pid_memoria(pcb->pid, CREAR_ESTRUCTURAS);

	t_paquete* paquete_respuesta = malloc(sizeof(t_paquete));
	paquete_respuesta->buffer = malloc(sizeof(t_buffer));

	recv(socket_memoria, &(paquete_respuesta->codigo_operacion), sizeof(op_code), MSG_WAITALL);
	recv(socket_memoria, &(paquete_respuesta->buffer->stream_size), sizeof(int), 0);
	paquete_respuesta->buffer->stream = malloc(paquete_respuesta->buffer->stream_size);
	recv(socket_memoria, paquete_respuesta->buffer->stream, paquete_respuesta->buffer->stream_size, 0);

	switch(paquete_respuesta->codigo_operacion) {
	case NUEVA_TABLA:
		//deserializar tabla de segmentos y tabla de segmentos global
		t_tabla_segmentos tabla; // = malloc(sizeof(t_tabla_segmentos));

		void* stream = paquete_respuesta->buffer->stream;

		int tam_segmentos;
		t_segmento* aux = malloc(sizeof(t_segmento));

		memcpy(&tam_segmentos, stream, sizeof(int));
		stream += sizeof(int);

		tabla.segmentos = list_create();

		for(int i = 0; i < tam_segmentos; i++) {
			memcpy((&aux->id), stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&(aux->base), stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&(aux->tamanio), stream, sizeof(int));
			stream += sizeof(int);

			list_add(tabla.segmentos, aux);
		}

		pcb->tabla_segmentos = tabla;

		break;
	default:
		log_error(logger_kernel, "[ERROR] Error creando las estructuras de memoria para el PID: <%d>", pcb->pid);
	}
}

void enviar_pid_memoria(int pid, op_code motivo_envio) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream_size = sizeof(int);

	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, &pid, sizeof(int));
	offset += sizeof(int);

	buffer->stream = stream;

	paquete->codigo_operacion = motivo_envio;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(socket_memoria, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void enviar_parametros_a_memoria(int pid, int id_segmento, int tamanio_segmento, op_code operacion) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));

	if(operacion == CREATE_SEGMENT) {
		buffer->stream_size = sizeof(int) //pid
			+ sizeof(int) 	// id_segmento
			+ sizeof(int);	// tamanio_segmento
	} else {
		buffer->stream_size = sizeof(int) //pid
				+ sizeof(int); //id_segmento
	}

	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, &pid, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &id_segmento, sizeof(int));
	offset += sizeof(int);

	if(operacion == CREATE_SEGMENT) {
		memcpy(stream + offset, &tamanio_segmento, sizeof(int));
		offset += sizeof(int);
	}

	buffer->stream = stream;

	paquete->codigo_operacion = operacion;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(socket_memoria, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void recibir_tablas_segmentos(t_pcb* pcb) {
	t_paquete* paquete_respuesta = malloc(sizeof(t_paquete));
	paquete_respuesta->buffer = malloc(sizeof(t_buffer));

	recv(socket_memoria, &(paquete_respuesta->codigo_operacion), sizeof(op_code), MSG_WAITALL);
	recv(socket_memoria, &(paquete_respuesta->buffer->stream_size), sizeof(int), 0);
	paquete_respuesta->buffer->stream = malloc(paquete_respuesta->buffer->stream_size);
	recv(socket_memoria, paquete_respuesta->buffer->stream, paquete_respuesta->buffer->stream_size, 0);

	switch(paquete_respuesta->codigo_operacion) {
	case NUEVA_TABLA:
		t_tabla_segmentos tabla; // = malloc(sizeof(t_tabla_segmentos));

		void* stream = paquete_respuesta->buffer->stream;

		memcpy(&(tabla.pid), stream, sizeof(int));
		stream += sizeof(int);

		int tam_segmentos;
		t_segmento* aux = malloc(sizeof(t_segmento));

		memcpy(&tam_segmentos, stream, sizeof(int));
		stream += sizeof(int);

		tabla.segmentos = list_create();

		for(int i = 0; i < tam_segmentos; i++) {
			memcpy((&aux->id), stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&(aux->base), stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&(aux->tamanio), stream, sizeof(int));
			stream += sizeof(int);

			list_add(tabla.segmentos, aux);
		}

		pcb->tabla_segmentos = tabla;
		break;
	default:
		log_error(logger_kernel, "[ERROR] Error recibiendo las tablas de segmentos.");
		break;
	}
}

void recibir_respuesta_create(t_proceso* proceso, int id_segmento, int tamanio_segmento) {
	t_paquete* paquete_respuesta = malloc(sizeof(t_paquete));
	paquete_respuesta->buffer = malloc(sizeof(t_buffer));

	recv(socket_memoria, &(paquete_respuesta->codigo_operacion), sizeof(op_code), MSG_WAITALL);
	recv(socket_memoria, &(paquete_respuesta->buffer->stream_size), sizeof(int), 0);
	paquete_respuesta->buffer->stream = malloc(paquete_respuesta->buffer->stream_size);
	recv(socket_memoria, paquete_respuesta->buffer->stream, paquete_respuesta->buffer->stream_size, 0);

	switch(paquete_respuesta->codigo_operacion){
	case OUT_OF_MEMORY:
		//recibir_tabla_segmentos_global();
		log_info(logger_kernel, "No hay suficiente memoria para crear el segmento solicitado por PID: <%d>", proceso->pcb->pid);
		pthread_mutex_lock(&mutex_exit);
		proceso->pcb->estado = EXT;
		list_add(cola_exit, proceso);
		pthread_mutex_unlock(&mutex_exit);

		log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <EXIT>", proceso->pcb->pid);
		log_info(logger_kernel, "Finaliza el proceso <%d> - Motivo: <OUT_OF_MEMORY>", proceso->pcb->pid);
		sem_post(&sem_exit); //despierta el proceso que saca a los procesos del sistema
		sem_post(&sem_ready);
		break;
	case SEGMENTO_CREADO:
		log_info(logger_kernel, "El segmento solicitado por PID: <%d> fue exitosamente creado", proceso->pcb->pid);
		//recibir_tablas_segmentos_global();
		recibir_tablas_segmentos(proceso->pcb);

		pthread_mutex_lock(&mutex_exec);
		list_add(cola_exec, proceso);
		pthread_mutex_unlock(&mutex_exec);

		sem_post(&sem_exec);
		break;
	case NECESITO_COMPACTAR:
		log_info(logger_kernel, "Compactación: <Se solicitó compactación>");

		log_info(logger_kernel, "Compactación: <Esperando Fin de Operaciones de FS");
		ordenar_compactacion();

		//TODO ordenar create_segment de nuevo
		log_info(logger_kernel, "PID: <%d> - Crear Segmento - Id: <%d> - Tamaño: <%d>", proceso->pcb->pid, id_segmento, tamanio_segmento);
		enviar_parametros_a_memoria(proceso->pcb->pid, id_segmento, tamanio_segmento, CREATE_SEGMENT);
		recibir_tablas_segmentos(proceso->pcb);

		pthread_mutex_lock(&mutex_exec);
		list_add(cola_exec, proceso);
		pthread_mutex_unlock(&mutex_exec);

		sem_post(&sem_exec);
		break;
	default:
		log_error(logger_kernel, "[ERROR] Error al recibir respuesta de CREATE_SEGMENT");
		break;
	}
}

void ordenar_compactacion(void) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));

	int numero_totalmente_arbitrario = 912;

	buffer->stream_size = sizeof(int);

	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, &numero_totalmente_arbitrario, sizeof(int));
	offset += sizeof(int);

	buffer->stream = stream;

	paquete->codigo_operacion = COMPACTAR;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(socket_memoria, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);

	t_paquete* paquete_respuesta = malloc(sizeof(t_paquete));
	paquete_respuesta->buffer = malloc(sizeof(t_buffer));

	recv(socket_memoria, &(paquete_respuesta->codigo_operacion), sizeof(op_code), MSG_WAITALL);
	recv(socket_memoria, &(paquete->buffer->stream_size), sizeof(int), 0);
	paquete->buffer->stream = malloc(paquete->buffer->stream_size);
	recv(socket_memoria, paquete->buffer->stream, paquete->buffer->stream_size, 0);

	switch(paquete_respuesta->codigo_operacion) {
	case COMPACTACION_TERMINADA:
		log_info(logger_kernel, "Se finalizó el proceso de compactación");

		//TODO acá va la recepción de las tablas de segmentos actualizadas

		break;
	default:
		log_error(logger_kernel, "[ERROR] Error al saber estado de la compactación.");
		break;
	}
}
