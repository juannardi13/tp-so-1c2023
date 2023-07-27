#include"utils_kernel2.h"
#include<shared-2.h>

pthread_mutex_t mutex_block_io;
pthread_mutex_t mutex_exec;
pthread_mutex_t mutex_exit;
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_operacion_file_system;
pthread_mutex_t mutex_operacion_memoria;
pthread_mutex_t mutex_pid;
pthread_mutex_t mutex_ready;
pthread_mutex_t mutex_procesos_en_el_sistema;
pthread_mutex_t mutex_compactacion_solicitada;
sem_t sem_admitir;
sem_t sem_block_io;
sem_t sem_exec;
sem_t sem_exit;
sem_t sem_grado_multiprogramacion;
sem_t sem_ready;
sem_t sem_recursos;
sem_t sem_archivos;
sem_t sem_finalizar_peticiones_fs;
pthread_t thread_archivos;
pthread_t thread_ready;
pthread_t thread_recursos;
pthread_t thread_exit;
pthread_t thread_exec;
pthread_t thread_ready;
pthread_t thread_blocked;
pthread_t thread_admitir_ready;
t_dictionary* procesos_en_el_sistema;
int compactacion_solicitada;

void admitir_procesos_a_ready(void) { //hilo

	while(1) {
		sem_wait(&sem_admitir);
		sem_wait(&sem_grado_multiprogramacion);

		t_proceso* proceso;

		pthread_mutex_lock(&mutex_new);
		proceso = list_remove(cola_new, 0);
		pthread_mutex_unlock(&mutex_new);

		pthread_mutex_lock(&mutex_ready);
		list_add(cola_ready, proceso);
		proceso->pcb->estado = READY;
		log_info(kernel_principal, "PID: <%d> - Estado Anterior: <NEW> - Estado Actual: <READY>", proceso->pcb->pid);

		switch (config_kernel.algoritmo_planificacion) {
		case FIFO:
			log_info(kernel_principal, "Cola Ready <FIFO>:");
			mostrar_cola(cola_ready);
			break;
		case HRRN:
			log_info(kernel_principal, "Cola Ready <HRRN>:");
			mostrar_cola(cola_ready);
			break;
		default:
			break;
		}
		pthread_mutex_unlock(&mutex_ready);


		proceso->llegada_ready = get_time();

		sem_post(&sem_ready);
	}

}

void iniciar_planificador_largo_plazo(void) {
	//Aca creo todos los semáforos y las estructuras necesarias para inicializar la planificación, por ahora solo tiene esto:
	procesos_en_el_sistema = dictionary_create();
	pthread_mutex_init(&mutex_procesos_en_el_sistema, NULL);
	pthread_mutex_init(&mutex_compactacion_solicitada, NULL);
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_exit, NULL);
	pthread_mutex_init(&mutex_pid, NULL);
	sem_init(&sem_exit, 0, 0);
	sem_init(&sem_grado_multiprogramacion, 0, config_kernel.grado_multiprogramacion); //semaforo contador para la cantidad de procesos en ready

	compactacion_solicitada = 0;

	cola_new = list_create();
	cola_exit = list_create();

	pthread_create(&thread_exit, NULL, (void*)estado_exit, NULL);
	pthread_detach(thread_exit);
}

void iniciar_planificador_corto_plazo(void) {

	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_block_io, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	pthread_mutex_init(&mutex_operacion_memoria, NULL);
	pthread_mutex_init(&mutex_operacion_file_system, NULL);
	sem_init(&sem_archivos, 0, 0);
	sem_init(&sem_recursos, 0, 0);
	sem_init(&sem_ready, 0, 0);
	sem_init(&sem_exec, 0, 0);
	sem_init(&sem_block_io, 0, 0);
	sem_init(&sem_finalizar_peticiones_fs, 0, 0);

	cola_ready = list_create();
	cola_exec = list_create();
	cola_block_io = list_create();

	pthread_create(&thread_archivos, NULL, (void*) hilo_archivos, NULL);
	pthread_create(&thread_ready, NULL, (void*) estado_ready, NULL);
	pthread_create(&thread_recursos, NULL, (void*) hilo_recursos, NULL);
	pthread_create(&thread_exec, NULL, (void*) estado_ejecutar, NULL);
	pthread_create(&thread_blocked, NULL, (void*) estado_block_io, NULL);
	pthread_create(&thread_admitir_ready, NULL, (void*) admitir_procesos_a_ready, NULL);
	pthread_detach(thread_ready);
	pthread_detach(thread_recursos);
	pthread_detach(thread_exec);
	pthread_detach(thread_blocked);
	pthread_detach(thread_admitir_ready);
}
