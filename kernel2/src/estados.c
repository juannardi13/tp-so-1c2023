#include"utils_kernel2.h"
#include<shared-2.h>

// ------------------------------------- ESTADO DE EJECUCIÓN, LO MÁS IMPORTANTE --------------------------------------------------------------------------------------

void estado_ejecutar(void) {

	while (1) {
		sem_wait(&sem_exec);
		pthread_mutex_lock(&mutex_exec);
		t_proceso* proceso_a_ejecutar = list_get(cola_exec, 0); //esto ayuda para cuando se necesita devolver el mismo proceso a cpu, como en las instrucciones de memoria
		pthread_mutex_unlock(&mutex_exec);

		struct timespec inicio, final; //Estructuras y definiciones necesarias para calcular el HRRN
		clock_gettime(CLOCK_REALTIME, &inicio);
		long seconds; 
		long nanoseconds;
		double elapsed;
		
		enviar_pcb(socket_cpu, proceso->pcb);
		log_info(logger_kernel, "PCB id[%d] enviada a CPU", proceso_a_ejecutar->pcb->pid);

		op_code respuesta_cpu = recibir_operacion(socket_cpu);

		proceso_a_ejecutar->pcb = recibir_pcb(socket_cpu);

		switch (respuesta_cpu) {
		case IO:
			clock_gettime(CLOCK_REALTIME, &final);
			seconds = final.tv_sec - inicio.tv_sec;
			nanoseconds = final.tv_nsec - inicio.tv_nsec;
			elapsed = seconds + nanoseconds * 1e - 9;
			proceso_a_ejecutar->ultima_rafaga = elapsed;
			
			//TODO buscar la manera de ejecutar IO
			//Quizas convenga mandarlo a dormir con usleep en estado block
			//Tambien ver si conviene crear la funcion get_time();
			//Otra forma de hacerlo puede ser:

			struct timespec inicio_bloqueo;
			clock_gettime(CLOCK_REALTIME, &inicio_bloqueo);
			long segundos_inicio_bloqueo = inicio_bloqueo.tv_sec;
			long nanosegundos_inicio_bloqueo = inicio_bloqueo.tv_nsec;
			double comienzo_bloqueo = segundos_inicio_bloqueo + nanosegundos_inicio_bloqueo * 1e - 9;
			proceso_a_ejecutar->inicio_block = comienzo_bloqueo;

			//Una forma de resolverlo:
			proceso_a_ejecutar->tiempo_bloqueo = recibir_parametro_instruccion(); //Implementar esta fuincion y fijarse si es en microsegundos nanosegundos o lo que sea

			pthread_mutex_lock(&mutex_block_io);
			proceso_a_ejecutar->pcb->estado = BLOCK;
			list_add(cola_block_io, proceso_a_ejecutar);
			pthread_mutex_unlock(&mutex_block_io);

			sem_post(&sem_block_io);
			
			log_warning(logger_kernel, "La última instrucción ejecutada fue IO");
			break;
		case F_OPEN:
			//Abrir o crear el archivo pasado por parámetro
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue F_OPEN");
			break;
		case F_CLOSE:
			//Cerrar el archivo pasado por parámetro
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue F_CLOSE");
			break;
		case F_SEEK:
			//Actualizar el puntero del archivo a la posición pasada por parámetro
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue F_SEEK");
			break;
		case F_READ:
			//Leer del archivo indicado la cantidad de bytes pasados por parámetro y escribirlo en la dirección física de memoria
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue F_READ");
			break;
		case F_WRITE:
			//Esta instrucción solicita al Kernel que se escriba en el archivo indicado la cantidad de bytes pasados por parámetro cuya información es obtenida a partir de la dirección física de Memoria.
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue F_WRITE");
			break;
		case F_TRUNCATE:
			//Modificar el tamaño del archivo al indicado por parámetro.
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue F_TRUNCATE");
			break;
		case WAIT:
			//Se asigne una instancia del recurso indicado por parámetro
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue WAIT");
			break;
		case SIGNAL:
			//Libera una instancia del recurso indicado por parámetro.
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue SIGNAL");
			break;
		case YIELD:
			//Se pone en cola de ready al proceso de nuevo.
			clock_gettime(CLOCK_REALTIME, &final);
			seconds = final.tv_sec - inicio.tv_sec;
			nanoseconds = final.tv_nsec - inicio.tv_nsec;
			elapsed = seconds + nanoseconds * 1e - 9;
			proceso_a_ejecutar->ultima_rafaga = elapsed;
			
			log_warning(logger_kernel, "La última instrucción ejecutada fue YIELD");
			pthread_mutex_lock(&mutex_ready);
			proceso->pcb->estado = READY;
			list_add(cola_ready, proceso);
			pthread_mutex_unlock(&mutex_ready);

			sem_post(&sem_ready);
			break;
		case EXIT:
			//Finalización del proceso.
			pthread_mutex_lock(&mutex_exit);
			proceso->pcb->estado = EXT;
			list_add(cola_exit, proceso);
			pthread_mutex_unlock(&mutex_exit);

			log_info(logger_kernel, "PCB id[%d] sale de EXEC y entra a EXIT", proceso->pcb->pid);
			sem_post(&sem_exit); //despierta el proceso que saca a los procesos del sistema
			sem_post(&sem_ready);

			log_warning(logger_kernel, "La última instrucción ejecutada fue EXIT");
			break;
		default:
			log_error(logger_kernel, "Instrucción desconocida, ocurrió un error");
			break;
		}
	}

}

//----------------------------------ESTADO EXIT---------------------------------------------------------------
// Este estado se encarga de avisar a la consola que se cierre y de eliminar las estructuras del PCB para liberar memoria

void estado_exit(void) {

	while(1) {
		sem_wait(&sem_exit);
		t_proceso* proceso;

		pthread_mutex_lock(&mutex_exit);
		proceso = list_remove(cola_exit, 0);
		pthread_mutex_unlock(&mutex_exit);

		log_info(logger_kernel, "[EXIT] PCB id[%d] sale de Exit y finaliza el proceso", proceso->pcb->pid);

		//Liberar las estructuras del proceso en memoria
		//enviar_pcb(socket_memoria, proceso->pcb);
		//log_info(logger_kernel, "Enviando pcb a memoria para liberar estructuras");
		//op_code codigo = esperar_respuesta_memoria(socket_memoria);
		//if(codigo != ESTRUCTURAS_LIBERADAS) {
		//	log_error(logger_kernel, "No se pudo eliminar de memoria a PCB id[%d]", proceso->pcb->pid);
		//}

		avisar_a_modulo(proceso->socket, FINALIZAR_CONSOLA);

		log_info(logger_kernel, "Ordenando a Consola la finalización del proceso");

		eliminar_pcb(proceso->pcb);
		free(proceso);
		sem_post(&sem_grado_multiprogramacion);
	}
}

void eliminar_pcb(t_pcb pcb) {
	//list_destroy_and_destroy_elements(pcb->segmentos, free);
	//TODO seguir destruyendo las eventuales listas de la pcb.
}

//---------------------------------------------------ESTADO READY----------------------------------------------------
//Este estado lo que hace es obtener el proceso necesario en base al algoritmo de la config para agregarlo a la lista EXEC

void estado_ready(void) {

	while(1) {
		sem_wait(&sem_ready);

		pthread_mutex_lock(&mutex_ready);
		if(list_is_empty(cola_ready)) { //Si la lista de ready está vacía vuelve y vuelve a entrar al while hasta que haya procesos.
			pthread_mutex_unlock(&mutex_ready);
			continue;
		}
		pthread_mutex_unlock(&mutex_ready);

		t_proceso* siguiente_proceso = obtener_proceso_cola_ready();

		pthread_mutex_lock(&mutex_exec);
		list_add(cola_exec, siguiente_proceso);
		pthread_mutex_unlock(&mutex_exec);

		log_info(logger_kernel, "PCB id[%d] ingresa a EXECUTE", siguiente_proceso->pcb->pid);
		sem_post(&sem_exec);
	}

}

t_proceso* obtener_proceso_cola_ready(void) {

	t_proceso* proceso = malloc(sizeof(t_proceso));

	switch(config_kernel.algoritmo_planificacion){
	case FIFO:
		proceso = list_remove(cola_ready, 0);
		log_info(logger_kernel, "PCB id[%d] saliendo de Cola de Ready por FIFO para ejecutar.", proceso->pcb->pid);
		break;
	case HRRN:
		aplicar_hrrn();
		proceso = list_remove(cola_ready, 0);
		log_info(logger_kernel, "PCB id[%d] saliendo de Cola de Ready por HRRN para ejecutar.", proceso->pcb->pid);
		break;
	default:
		log_error(logger_kernel, "Error al elegir un proceso de la cola de Ready para ejecutar");
		break;
	}

	return proceso;
}

void aplicar_hrrn(void) {

	list_iterate(cola_ready, (void*) calcular_response_ratio);
	list_iterate(cola_ready, (void*) mostrar_response_ratio);

	list_sort(cola_ready, (void*) comparador_response_ratio);
}

void calcular_response_ratio(t_proceso* proceso) {
	//ATENCIÓN CON LO QUE VIENE ES UNA MAGIC JOHNSON, DESPUÉS INDAGAR BIEN TODO SOBRE FUNCIONES DE CLOCK
	struct timespec end;
	clock_gettime(CLOCK_REALTIME, &end);

	long seconds = end.tv_sec - proceso->pcb->llegada_ready.tv_sec;
	long nanoseconds = end.tv_nsec - proceso->pcb->llegada_ready.tv_sec;
	double elapsed = seconds + nanoseconds * 1e-9;

	proceso->pcb->tiempo_espera = elapsed; //TODO poner tiempo de llegada a ready en el hilo correspondiente estado_ready y estado_block
	proceso->pcb->response_ratio = 1 + (proceso->pcb->tiempo_espera / proceso->pcb->rafaga_estimada);
}

bool comparador_response_ratio(t_proceso* un_proceso, t_proceso* otro_proceso) {
	return un_proceso->pcb->response_ratio >= otro_proceso->pcb->response_ratio;
}

void mostrar_response_ratio(t_proceso* un_proceso) {
	log_info(logger_kernel, "PCB id[%d] tiene un RR de: %f", un_proceso->pcb->response_ratio);
}

void calcular_estimacion(t_proceso* un_proceso) {
	double nueva_rafaga = (config_kernel.alfa_hrrn * un_proceso->pcb->rafaga_estimada) + ((un_proceso->pcb->ultima_rafaga) * (1 - config_kernel.alfa_hrrn));
	un_proceso->pcb->rafaga_estimada = nueva_rafaga;
}

//-----------------------------------------ESTADO BLOCK POR IO--------------------------------------------------------------------------------------
//Este estado es el encargado de gestionar la cola de bloqueados por IO, haciendo que ejecuten la interrupción y se bloqueen por el tiempo indicado.

void estado_block_io(void) {

	while(1) {
		sem_wait(&sem_block_io);

		pthread_mutex_lock(&mutex_block_io);
		t_proceso* proceso = list_remove(cola_block_io, 0);
		pthread_mutex_unlock(&mutex_block_io);

		usleep(proceso->tiempo_bloqueo);

		pthread_mutex_lock(&mutex_ready);
		list_add(cola_ready, proceso);
		pthread_mutex_unlock(&mutex_ready);

		sem_post(&sem_ready);
	}
}
