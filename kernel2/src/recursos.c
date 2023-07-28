#include"utils_kernel2.h"
#include<shared-2.h>

t_dictionary* recursos;

void iniciar_recursos(void) {

	recursos = dictionary_create();
	log_info(logger_kernel, "Se crea el diccionario con los recursos.");

	t_recurso* recurso;

	for(int i = 0; config_kernel.recursos[i] != NULL; i++) {
		recurso = malloc(sizeof(t_recurso));

		recurso->nombre = config_kernel.recursos[i];
		recurso->instancias_usadas = 0;
		recurso->instancias_totales = atoi(config_kernel.instancias_recursos[i]);
		recurso->cola_espera = queue_create();

		pthread_mutex_t* mutex_cola_espera = malloc(sizeof(pthread_mutex_t));
		if (pthread_mutex_init(mutex_cola_espera, NULL) != 0) {
			log_error(logger_kernel, "Error al iniciar el Mutex");
			exit(3);
		}
		recurso->mutex_cola_espera = mutex_cola_espera;

		pthread_mutex_t* mutex_instancia_usada = malloc(sizeof(pthread_mutex_t));
		if (pthread_mutex_init(mutex_instancia_usada, NULL) != 0) {
			log_error(logger_kernel, "Error al iniciar el Mutex");
			exit(3);
		}
		recurso->mutex_instancia_usada = mutex_instancia_usada;

		dictionary_put(recursos, recurso->nombre, recurso);
		log_info(logger_kernel, "Se agrego el recurso %s al diccionario", recurso->nombre);
	}

}

void liberar_recursos(void) {
	dictionary_iterator(recursos, liberar_recurso);
}

void liberar_recurso(char* key, void* elemento) {

	t_recurso* recurso = (t_recurso*) elemento;

	pthread_mutex_destroy(recurso->mutex_cola_espera);
	pthread_mutex_destroy(recurso->mutex_instancia_usada);
	queue_destroy(recurso->cola_espera);
	free(recurso->mutex_cola_espera);
	free(recurso->mutex_instancia_usada);
	free(recurso);
}

void* hilo_recursos(void* argumento) {

	while(1) {
		sem_wait(&sem_recursos);

		void funcion_recurso_diccionario(char* key, void* elemento) {
			funcion_recurso(key, elemento);
		}

		dictionary_iterator(recursos, funcion_recurso_diccionario);
	}
}

void funcion_recurso(char* key, void* elemento) {

	t_recurso* recurso = (t_recurso*) elemento;

	pthread_mutex_lock(recurso->mutex_instancia_usada);
	if(recurso->instancias_totales > recurso->instancias_usadas && !queue_is_empty(recurso->cola_espera)) {

		pthread_mutex_lock(recurso->mutex_cola_espera);
		t_proceso* proceso = queue_pop(recurso->cola_espera);
		recurso->instancias_usadas++;
		pthread_mutex_unlock(recurso->mutex_cola_espera);

		proceso->pcb->estado = READY;
		log_info(kernel_principal, "PID: <%d> - Estado Anterior: <BLOCK> - Estado Actual: <READY>", proceso->pcb->pid);
		int length = strlen(key) + 1;
		char* aux = malloc(length);
		memcpy(aux, key, length);

		list_add(proceso->pcb->recursos_asignados, aux);

		pthread_mutex_lock(&mutex_ready);
		list_add(cola_ready, proceso);
		proceso->llegada_ready = get_time();
		pthread_mutex_unlock(&mutex_ready);

		sem_post(&sem_ready); //TODO ??? Me parece que esto vuela
	}

	pthread_mutex_unlock(recurso->mutex_instancia_usada);
}

int asignar_recurso(t_recurso* recurso, t_proceso* proceso) { // devuelve 0 = false o 1 = true

	int retorno = 0;

	if(recurso->instancias_totales > recurso->instancias_usadas) {

		pthread_mutex_lock(recurso->mutex_instancia_usada);
		recurso->instancias_usadas++;
		pthread_mutex_unlock(recurso->mutex_instancia_usada);

		int length = strlen(recurso->nombre) + 1;
		char* aux = malloc(length);
		memcpy(aux, recurso->nombre, length);

		list_add(proceso->pcb->recursos_asignados, recurso->nombre);

		retorno = 1;
	}

	return retorno;
}

void liberar_instancia_recurso(t_recurso* recurso) { // TODO esto no saca el recurso del proceso, así que lo tengo que implementar en el execute
	pthread_mutex_lock(recurso->mutex_instancia_usada);
	recurso->instancias_usadas--;
	pthread_mutex_unlock(recurso->mutex_instancia_usada);

	sem_post(&sem_recursos);
}

void liberar_recursos_asignados(t_list* lista) {

	int length_lista = list_size(lista);

	for(int i = 0; i < length_lista; i++) {
		char* key = list_get(lista, i);
		t_recurso* recurso = dictionary_get(recursos, key);
		liberar_instancia_recurso(recurso);
	}

}

void quitar_recurso(t_proceso* proceso, t_recurso* recurso) {

//---------Funcion Lambda para encontrar el recurso en la lista-----------//
	bool buscar_recurso(void* un_recurso) {
		char* nombre_recurso = (char*) un_recurso;
		return string_equals_ignore_case(nombre_recurso, recurso->nombre);
	}
	
	list_remove_by_condition(proceso->pcb->recursos_asignados, buscar_recurso);
}

int string_to_int(char* numero) {

	int r = 0;
	int e = 1;

	for(int i = string_length(numero) - 1; i >= 0; i++) {
		r += (numero[i] - '0') * e;
		e *= 10;
	}

	return e;
}
