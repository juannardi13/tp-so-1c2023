#include"utils_kernel2.h"
#include<shared-2.h>

void iniciar_planificador_largo_plazo(void) {
	//Aca creo todos los semáforos y las estructuras necesarias para inicializar la planificación, por ahora solo tiene esto:
	pthread_mutex_init(&mutex_new, NULL);
	pthread_mutex_init(&mutex_exit, NULL);
	pthread_mutex_init(&mutex_pid, NULL);
	//sem_init(&sem_exit, 0, 0);
	//sem_init(&sem_grado_multiprogramacion, 0, config_kernel.grado_multiprogramacion); //semaforo contador para la cantidad de procesos en ready

	cola_new = list_create();
	cola_exit = list_create();

	pthread_create(&thread_exit, NULL, (void*)estado_exit, NULL);
	pthread_detach(thread_exit);
}

void iniciar_planificador_corto_plazo(void) {
	//pthread_mutex_init(&mutex_ready, NULL);
	//pthread_mutex_init(&mutex_block_io, NULL);
	//pthread_mutex_init(&mutex_exec, NULL);
	//sem_init(&sem_ready, 0, 0);
	//sem_init(&sem_exec, 0, 0);
	//sem_init(&sem_blocked, 0, 0);

	cola_ready = list_create();
	cola_exec = list_create();
	cola_block = list_create();

	//TODO inicializar colas de recursos
	//pthread_create(&thread_ready, NULL, (void*) estado_ready, NULL);
//	pthread_create(&thread_exec, NULL, (void*) ejecutar_proceso, NULL);
//	pthread_create(&thread_blocked, NULL, (void*) estado_bloqueado, NULL);
//	pthread_detach(&thread_ready);
//	pthread_detach(&thread_exec);
//	pthread_detach(&thread_blocked);
}
