#include"utils_kernel2.h"
#include<shared-2.h>

pthread_mutex_t mutex_block_io;
pthread_mutex_t mutex_exec;
pthread_mutex_t mutex_exit;
pthread_mutex_t mutex_new;
pthread_mutex_t mutex_pid;
pthread_mutex_t mutex_ready;
sem_t sem_admitir;
sem_t sem_block_io;
sem_t sem_exec;
sem_t sem_exit;
sem_t sem_grado_multiprogramacion;
sem_t sem_ready;
sem_t sem_recursos;
pthread_t thread_ready;
pthread_t thread_exit;
pthread_t thread_exec;
pthread_t thread_ready;
pthread_t thread_blocked;
pthread_t thread_admitir_ready;

void admitir_procesos_a_ready(void) { //hilo

	while(1) {
		sem_wait(&sem_admitir);
		sem_wait(&sem_grado_multiprogramacion);

		t_proceso* proceso;

		pthread_mutex_lock(&mutex_new);
		proceso = list_remove(cola_new, 0);
		pthread_mutex_unlock(&mutex_new);

		//Puedo hacer que los segmentos de memoria se los demos al proceso acá o apenas entra a cola new TODO

		pthread_mutex_lock(&mutex_ready);
		list_add(cola_ready, proceso);
		pthread_mutex_unlock(&mutex_ready);

		proceso->pcb->estado = READY;
		log_info(logger_kernel, "PID: <%d> - Estado Anterior: <NEW> - Estado Actual: <READY>", proceso->pcb->pid);

		log_info(logger_kernel,"Cola READY: ");
		mostrar_cola(cola_ready);

		proceso->llegada_ready = get_time();

		sem_post(&sem_ready);
	}
}

void iniciar_planificador_largo_plazo(void) {
	//Aca creo todos los semáforos y las estructuras necesarias para inicializar la planificación, por ahora solo tiene esto:
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_exit, NULL);
	pthread_mutex_init(&mutex_pid, NULL);
	sem_init(&sem_exit, 0, 0);
	sem_init(&sem_grado_multiprogramacion, 0, config_kernel.grado_multiprogramacion); //semaforo contador para la cantidad de procesos en ready

	cola_new = list_create();
	cola_exit = list_create();

	pthread_create(&thread_exit, NULL, (void*)estado_exit, NULL);
	pthread_detach(thread_exit);
}

void iniciar_planificador_corto_plazo(void) {

	pthread_mutex_init(&mutex_ready, NULL);
	pthread_mutex_init(&mutex_block_io, NULL);
	pthread_mutex_init(&mutex_exec, NULL);
	sem_init(&sem_ready, 0, 0);
	sem_init(&sem_exec, 0, 0);
	sem_init(&sem_block_io, 0, 0);

	cola_ready = list_create();
	cola_exec = list_create();
	cola_block_io = list_create();

	pthread_create(&thread_ready, NULL, (void*) estado_ready, NULL);
	pthread_create(&thread_exec, NULL, (void*) estado_ejecutar, NULL);
	pthread_create(&thread_blocked, NULL, (void*) estado_block_io, NULL);
	pthread_create(&thread_admitir_ready, NULL, (void*) admitir_procesos_a_ready, NULL);
	pthread_detach(thread_ready);
	pthread_detach(thread_exec);
	pthread_detach(thread_blocked);
	pthread_detach(thread_admitir_ready);
}
