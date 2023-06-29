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
	pthread_detach(thread_ready);
	pthread_detach(thread_exec);
	pthread_detach(thread_blocked);
}
