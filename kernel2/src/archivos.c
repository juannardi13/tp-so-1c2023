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
	archivo->instancias_totales = 0;
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

	bool buscar_archivo(void* un_archivo) {
		char* nombre_archivo = (char*) un_archivo;
		return string_equals_ignore_case(nombre_archivo, nombre);
	}

	list_remove_by_condition(proceso->pcb->tabla_archivos, buscar_archivo);
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
