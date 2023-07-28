#include"utils_kernel2.h"
#include<shared-2.h>

t_dictionary* tabla_global_archivos;
pthread_mutex_t mutex_tabla_global_archivos;

void inicializar_tabla_global_archivos(void) {
	tabla_global_archivos = dictionary_create();
	pthread_mutex_init(&mutex_tabla_global_archivos, NULL);

	log_info(logger_kernel, "Se crea la Tabla Global de Archivos");
}

void agregar_archivo_a_tabla_global_archivos(char* nombre) {
	t_archivo* archivo = malloc(sizeof(t_archivo));

	archivo->nombre = nombre;
	archivo->tamanio = 0;
	archivo->instancias_usadas = 0;
	archivo->instancias_totales = 1;
	archivo->cola_bloqueados = queue_create();

	pthread_mutex_t* mutex_cola_bloqueo = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_cola_bloqueo, NULL);

	archivo->mutex_cola_bloqueados = mutex_cola_bloqueo;

	pthread_mutex_t* mutex_instancia_usada = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(mutex_instancia_usada, NULL);

	archivo->mutex_instancia_usada = mutex_instancia_usada;

	dictionary_put(tabla_global_archivos, nombre, archivo);
	log_info(logger_kernel, "Agregado archivo <%s> a la Tabla Global de Archivos.", nombre);
}


void quitar_entrada_archivo_del_proceso(char* nombre, t_proceso* proceso) {
/*
	bool buscar_archivo(void* un_archivo) {
		char* nombre_archivo = (char*) un_archivo;
		return string_equals_ignore_case(nombre_archivo, nombre);
	}

	list_remove_by_condition(proceso->pcb->tabla_archivos, buscar_archivo);*/
	dictionary_remove_and_destroy(proceso->pcb->tabla_archivos, nombre, free);
}


void quitar_entrada_archivo_de_tabla_global_archivos(char* nombre) {
	t_archivo* archivo = dictionary_remove(tabla_global_archivos, nombre);

	pthread_mutex_destroy(archivo->mutex_cola_bloqueados);
	pthread_mutex_destroy(archivo->mutex_instancia_usada);
	queue_destroy(archivo->cola_bloqueados);
	free(archivo->mutex_cola_bloqueados);
	free(archivo->mutex_instancia_usada);
	free(archivo->nombre);
	free(archivo);
}

void* hilo_archivos(void* argumento) {

	while(1) {
		sem_wait(&sem_archivos);


		void funcion_archivos_diccionario(char* key, void* elemento) {
			funcion_archivo(key, elemento);
		}

		dictionary_iterator(tabla_global_archivos, funcion_archivos_diccionario);
	}
}

void funcion_archivo(char* key, void* elemento) {

	t_archivo* archivo = (t_archivo*) elemento;

	pthread_mutex_lock(archivo->mutex_instancia_usada);
	if(archivo->instancias_totales > archivo->instancias_usadas && !queue_is_empty(archivo->cola_bloqueados)) {

		pthread_mutex_lock(archivo->mutex_cola_bloqueados);
		t_proceso* proceso = queue_pop(archivo->cola_bloqueados);
		archivo->instancias_usadas++;
		pthread_mutex_unlock(archivo->mutex_cola_bloqueados);

		proceso->pcb->estado = READY;
		log_info(logger_kernel, "PID: <%d> - Estado Anterior: <BLOCK> - Estado Actual: <READY>", proceso->pcb->pid);

		pthread_mutex_lock(&mutex_ready);
		list_add(cola_ready, proceso);
		proceso->llegada_ready = get_time();
		pthread_mutex_unlock(&mutex_ready);

		sem_post(&sem_ready);
	}

	pthread_mutex_unlock(archivo->mutex_instancia_usada);
}

void liberar_entrada_archivo(t_archivo* archivo) {
	pthread_mutex_lock(archivo->mutex_instancia_usada);
	archivo->instancias_usadas--;
	pthread_mutex_unlock(archivo->mutex_instancia_usada);

	sem_post(&sem_archivos);
}

void actualizar_puntero_archivo_proceso(t_proceso* proceso, char* nombre, int nuevo_puntero) {
/*
	bool buscar_archivo(void* un_archivo) {
		char* nombre_archivo = (char*) un_archivo;
		return string_equals_ignore_case(nombre_archivo, nombre);
	}*/

	t_archivo_proceso* archivo = dictionary_get(proceso->pcb->tabla_archivos, nombre);

	log_info(logger_kernel, "Actualización del puntero a <%d> del archivo: <%s>", nuevo_puntero, archivo->nombre);

	archivo->puntero = nuevo_puntero;
}

op_code consultar_existencia_archivo_a_fs(char* nombre) {
	op_code respuesta;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));

	int tamanio_nombre_archivo = strlen(nombre) + 1;

	buffer->stream_size = sizeof(int)
			+ tamanio_nombre_archivo;

	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, &tamanio_nombre_archivo, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, nombre, tamanio_nombre_archivo);
	offset += tamanio_nombre_archivo;

	buffer->stream = stream;

	paquete->codigo_operacion = F_OPEN;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(socket_filesystem, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);

	recv(socket_filesystem, &respuesta, sizeof(op_code), MSG_WAITALL); //Tendria que ser el op_code EXISTE o NO_EXISTE

	switch(respuesta) {
	case EXISTE:
		log_info(logger_kernel, "El archivo <%s> existe en FileSystem", nombre);
		respuesta = EXISTE;
		break;
	case NO_EXISTE:
		log_info(logger_kernel, "El archivo <%s> no existe en FileSystem", nombre);
		respuesta = NO_EXISTE;
		break;
	default:
		log_error(logger_kernel, "[ERROR] Error al determinar si existe <%s> en FileSystem", nombre);
		break;
	}

	return respuesta;
}

void crear_archivo_en_fs(char* nombre) {

	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));

	int tamanio_nombre_archivo = strlen(nombre) + 1;

	buffer->stream_size = sizeof(int)
			+ tamanio_nombre_archivo;

	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, &tamanio_nombre_archivo, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, nombre, tamanio_nombre_archivo);
	offset += tamanio_nombre_archivo;

	buffer->stream = stream;

	paquete->codigo_operacion = F_CREATE;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(socket_filesystem, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}
