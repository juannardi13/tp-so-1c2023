#include"utils_kernel2.h"
#include<shared-2.h>

t_queue* cola_peticiones_file_system;
pthread_t thread_peticiones_fs;
sem_t sem_peticiones_file_system;
pthread_mutex_t mutex_compactacion;
pthread_mutex_t mutex_peticiones_fs;

void iniciar_peticiones_file_system(void) {
	cola_peticiones_file_system = queue_create();

	pthread_mutex_init(&mutex_compactacion, NULL);
	pthread_mutex_init(&mutex_peticiones_fs, NULL);
	sem_init(&sem_peticiones_file_system, 0, 0);

	pthread_create(&thread_peticiones_fs, NULL, (void*) manejar_peticiones_fs, NULL);
	pthread_detach(thread_peticiones_fs);
}

void* manejar_peticiones_fs(void) {
	while(1) {
		sem_wait(&sem_peticiones_file_system);
		pthread_mutex_lock(&mutex_compactacion);

		pthread_mutex_lock(&mutex_peticiones_fs);
		pthread_mutex_lock(&mutex_operacion_file_system);

		t_proceso* proceso = enviar_peticion_file_system();

		pthread_mutex_unlock(&mutex_peticiones_fs);

		respuesta_peticion_file_system();

		pthread_mutex_unlock(&mutex_operacion_file_system);

		pthread_mutex_lock(&mutex_ready);
		list_add(cola_ready, proceso);
		proceso->llegada_ready = get_time();
		pthread_mutex_unlock(&mutex_ready);
		log_info(kernel_principal, "PID: <%d> - Estado Anterior: <BLOCK> - Estado Actual: <READY>", proceso->pcb->pid);


		pthread_mutex_lock(&mutex_compactacion_solicitada);
		pthread_mutex_lock(&mutex_peticiones_fs);
		if(queue_is_empty(cola_peticiones_file_system) && compactacion_solicitada == 1) {
			sem_post(&sem_finalizar_peticiones_fs);
		}
		pthread_mutex_unlock(&mutex_peticiones_fs);
		pthread_mutex_unlock(&mutex_compactacion_solicitada);

		pthread_mutex_unlock(&mutex_compactacion);
		sem_post(&sem_ready);
	}
}

t_proceso* enviar_peticion_file_system(void) {
	t_peticion* peticion = queue_pop(cola_peticiones_file_system);
	t_proceso* proceso = peticion->proceso;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));

	int tamanio_nombre_archivo = strlen(peticion->nombre_archivo) + 1;

	void* stream;
	int offset = 0;

	switch(peticion->codigo_operacion) {
	case F_TRUNCATE:
		buffer->stream_size = sizeof(int) * 2
				+ tamanio_nombre_archivo;

		stream = malloc(buffer->stream_size);

		memcpy(stream + offset, &(tamanio_nombre_archivo), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, peticion->nombre_archivo, tamanio_nombre_archivo);
		offset += tamanio_nombre_archivo;

		memcpy(stream + offset, &(peticion->nuevo_tamanio_truncate), sizeof(int));
		offset += sizeof(int);

		buffer->stream = stream;

		paquete->codigo_operacion = F_TRUNCATE;
		paquete->buffer = buffer;

		break;
	case F_READ:
		buffer->stream_size = sizeof(int) // direccion_física
				+ sizeof(int) // cantidad_bytes
				+ sizeof(int) //int tamanio_nombre_read
				+ sizeof(int) //puntero
				+ tamanio_nombre_archivo;

		stream = malloc(buffer->stream_size);

		memcpy(stream + offset, &(tamanio_nombre_archivo), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, peticion->nombre_archivo, tamanio_nombre_archivo);
		offset += tamanio_nombre_archivo;

		memcpy(stream + offset, &(peticion->puntero), sizeof(int));
		offset += sizeof(int);

		memcpy(stream + offset, &(peticion->direccion_fisica), sizeof(int));
		offset += sizeof(int);

		memcpy(stream + offset, &(peticion->cantidad_bytes), sizeof(int));
		offset += sizeof(int);

		buffer->stream = stream;

		paquete->codigo_operacion = F_READ;
		paquete->buffer = buffer;

		break;
	case F_WRITE:
		buffer->stream_size = sizeof(int) //direccion_física
			+ sizeof(int) //cantidad_bytes
			+ sizeof(int) //int_tamanio_nombre_archivo
			+ sizeof(int) //puntero
			+ tamanio_nombre_archivo;

		stream = malloc(buffer->stream_size);

		memcpy(stream + offset, &(tamanio_nombre_archivo), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, peticion->nombre_archivo, tamanio_nombre_archivo);
		offset += tamanio_nombre_archivo;

		memcpy(stream + offset, &(peticion->puntero), sizeof(int));
		offset += sizeof(int);

		memcpy(stream + offset, &(peticion->direccion_fisica), sizeof(int));
		offset += sizeof(int);

		memcpy(stream + offset, &(peticion->cantidad_bytes), sizeof(int));
		offset += sizeof(int);

		buffer->stream = stream;

		paquete->codigo_operacion = F_WRITE;
		paquete->buffer = buffer;

		break;
	default:
		log_error(logger_kernel, "[ERROR] Error enviando la peticion a FILE SYSTEM.");
		break;
	}

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(socket_filesystem, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);

	free(peticion);

	return proceso;
}

void respuesta_peticion_file_system(void) {
	op_code respuesta_peticion;

	recv(socket_filesystem, &respuesta_peticion, sizeof(op_code), MSG_WAITALL);

	switch(respuesta_peticion) {
	case FINALIZADO:
		log_info(logger_kernel, "Peticion FILE SYSTEM terminada con éxito.");
		break;
	default:
		log_error(logger_kernel, "[ERROR] Error al recibir respuesta de petición de FILE SYSTEM.");
		break;
	}
}
