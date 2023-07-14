#include"utils_kernel2.h"
#include<shared-2.h>

// ------------------------------------- ESTADO DE EJECUCIÓN, LO MÁS IMPORTANTE --------------------------------------------------------------------------------------

void estado_ejecutar(void) {

	while (1) {
		sem_wait(&sem_exec);
		pthread_mutex_lock(&mutex_exec);
		t_proceso* proceso_a_ejecutar = list_get(cola_exec, 0); //esto ayuda para cuando se necesita devolver el mismo proceso a cpu, como en las instrucciones de memoria
		pthread_mutex_unlock(&mutex_exec);

		proceso_a_ejecutar->pcb->estado = EXEC;
		log_info(logger_kernel, "PID: <%d> - Estado Anterior: <READY> - Estado Actual: <EXEC>", proceso_a_ejecutar->pcb->pid);

//		struct timespec inicio, final; //Estructuras y definiciones necesarias para calcular el HRRN
	//	clock_gettime(CLOCK_REALTIME, &inicio);
	//	long seconds; 
	//	long nanoseconds;
	//	double elapsed;

	// 	if(proceso_a_ejecutar no viene de una instrucción que lo desalojó del CPU)

		int inicio_cpu = get_time();
		proceso_a_ejecutar->principio_ultima_rafaga = inicio_cpu;
		
		enviar_pcb(socket_cpu, proceso_a_ejecutar->pcb);
		log_info(logger_kernel, "PID: <%d> enviada a CPU", proceso_a_ejecutar->pcb->pid);


		//proceso_a_ejecutar->pcb = recibir_pcb(socket_cpu);

		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));
		recv(socket_cpu, &(paquete->codigo_operacion), sizeof(op_code), MSG_WAITALL);
		recv(socket_cpu, &(paquete->buffer->stream_size), sizeof(int), 0);
		paquete->buffer->stream = malloc(paquete->buffer->stream_size);
		recv(socket_cpu, paquete->buffer->stream, paquete->buffer->stream_size, 0);

		op_code respuesta_cpu = paquete->codigo_operacion;

		void *stream = paquete->buffer->stream;

		//-------------------------------------------------------------RECIBO EL PID
		memcpy(&(proceso_a_ejecutar->pcb->pid), stream, sizeof(int));
		stream += sizeof(int);

		//-------------------------------------------------------------RECIBO EL PROGRAM COUNTER
		memcpy(&(proceso_a_ejecutar->pcb->pc), stream, sizeof(int));
		stream += sizeof(int);

		//-------------------------------------------------------------RECIBO INSTRUCCIONES
		memcpy(&(proceso_a_ejecutar->pcb->tamanio_instrucciones), stream, sizeof(int));
		stream += sizeof(int);

		char* instrucciones = malloc(proceso_a_ejecutar->pcb->tamanio_instrucciones);
		memset(instrucciones, 0, proceso_a_ejecutar->pcb->tamanio_instrucciones);

		memcpy(instrucciones, stream, proceso_a_ejecutar->pcb->tamanio_instrucciones);
		stream += proceso_a_ejecutar->pcb->tamanio_instrucciones;

		proceso_a_ejecutar->pcb->instrucciones = malloc(strlen(instrucciones) + 1);
		strcpy(proceso_a_ejecutar->pcb->instrucciones, instrucciones);

		//-------------------------------------------------------------RECIBO REGISTROS
		int tamanio_registro_chico = strlen(registros_iniciados.ax) + 1;
		int tamanio_registro_mediano = strlen(registros_iniciados.eax) + 1;
		int tamanio_registro_grande = strlen(registros_iniciados.rax) + 1;

		memcpy(&(proceso_a_ejecutar->pcb->registros.ax), stream, tamanio_registro_chico);
		stream += tamanio_registro_chico;
		memcpy(&(proceso_a_ejecutar->pcb->registros.bx), stream, tamanio_registro_chico);
		stream += tamanio_registro_chico;
		memcpy(&(proceso_a_ejecutar->pcb->registros.cx), stream, tamanio_registro_chico);
		stream += tamanio_registro_chico;
		memcpy(&(proceso_a_ejecutar->pcb->registros.dx), stream, tamanio_registro_chico);
		stream += tamanio_registro_chico;

		memcpy(&(proceso_a_ejecutar->pcb->registros.eax), stream, tamanio_registro_mediano);
		stream += tamanio_registro_mediano;
		memcpy(&(proceso_a_ejecutar->pcb->registros.ebx), stream, tamanio_registro_mediano);
		stream += tamanio_registro_mediano;
		memcpy(&(proceso_a_ejecutar->pcb->registros.ecx), stream, tamanio_registro_mediano);
		stream += tamanio_registro_mediano;
		memcpy(&(proceso_a_ejecutar->pcb->registros.edx), stream, tamanio_registro_mediano);
		stream += tamanio_registro_mediano;

		memcpy(&(proceso_a_ejecutar->pcb->registros.rax), stream, tamanio_registro_grande);
		stream += tamanio_registro_grande;
		memcpy(&(proceso_a_ejecutar->pcb->registros.rbx), stream, tamanio_registro_grande);
		stream += tamanio_registro_grande;
		memcpy(&(proceso_a_ejecutar->pcb->registros.rcx), stream, tamanio_registro_grande);
		stream += tamanio_registro_grande;
		memcpy(&(proceso_a_ejecutar->pcb->registros.rdx), stream, tamanio_registro_grande);
		stream += tamanio_registro_grande;

		log_info(logger_kernel, "Los registros que trajo PID: <%d> de la CPU son: ", proceso_a_ejecutar->pcb->pid);
		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.ax);
		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.bx);
		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.cx);
		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.dx);
		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.eax);
		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.ebx);
		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.ecx);
		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.edx);
		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.rax);
		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.rbx);
		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.rcx);
		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.rdx);

		//--- FALTA DESERIALIZAR SEGMENTOS

		switch (respuesta_cpu) {
		case IO:
			int finalizacion_cpu = get_time();
			proceso_a_ejecutar->final_ultima_rafaga = finalizacion_cpu;
			proceso_a_ejecutar->ultima_rafaga = proceso_a_ejecutar->final_ultima_rafaga - proceso_a_ejecutar->principio_ultima_rafaga;

			proceso_a_ejecutar->ultima_instruccion = IO;

			int inicio_block = get_time();
			proceso_a_ejecutar->inicio_bloqueo = inicio_block;
			
			memcpy(&(proceso_a_ejecutar->tiempo_bloqueo), stream, sizeof(int));
			stream += sizeof(int);

			pthread_mutex_lock(&mutex_block_io);
			proceso_a_ejecutar->pcb->estado = BLOCK;
			list_add(cola_block_io, proceso_a_ejecutar);
			pthread_mutex_unlock(&mutex_block_io);

			log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCK>", proceso_a_ejecutar->pcb->pid);
			log_info(logger_kernel, "PID: <%d> - Bloqueado por: <IO>", proceso_a_ejecutar->pcb->pid);

			sem_post(&sem_block_io);
			break;
		case F_OPEN:
			//Abrir o crear el archivo pasado por parámetro

			proceso_a_ejecutar->ultima_instruccion = F_OPEN;
			
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue F_OPEN");
			break;
		case F_CLOSE:
			//Cerrar el archivo pasado por parámetro
			proceso_a_ejecutar->ultima_instruccion = F_CLOSE;
			
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue F_CLOSE");
			break;
		case F_SEEK:
			//Actualizar el puntero del archivo a la posición pasada por parámetro

			proceso_a_ejecutar->ultima_instruccion = F_SEEK;
			
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue F_SEEK");
			break;
		case F_READ:
			//Leer del archivo indicado la cantidad de bytes pasados por parámetro y escribirlo en la dirección física de memoria

			proceso_a_ejecutar->ultima_instruccion = F_READ;
			
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue F_READ");
			break;
		case F_WRITE:
			//Esta instrucción solicita al Kernel que se escriba en el archivo indicado la cantidad de bytes pasados por parámetro cuya información es obtenida a partir de la dirección física de Memoria.

			proceso_a_ejecutar->ultima_instruccion = F_WRITE;
			
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue F_WRITE");
			break;
		case F_TRUNCATE:
			//Modificar el tamaño del archivo al indicado por parámetro.

			proceso_a_ejecutar->ultima_instruccion = F_TRUNCATE;
			
			log_warning(logger_kernel,
					"La última instrucción ejecutada fue F_TRUNCATE");
			break;
		case WAIT:
			//Se asigne una instancia del recurso indicado por parámetro

			proceso_a_ejecutar->ultima_instruccion = WAIT;
			
			int tamanio_parametro_wait = 0;

			memcpy(&tamanio_parametro_wait, stream, sizeof(int));
			stream += sizeof(int);

			char* recurso_wait = malloc(tamanio_parametro_wait);
			memset(recurso_wait, 0, tamanio_parametro_wait);

			memcpy(recurso_wait, stream, tamanio_parametro_wait);
			stream += tamanio_parametro_wait;

			if (dictionary_has_key(recursos, recurso_wait)) {
				//Lógica de asignar el recurso TODO
			} else {
				pthread_mutex_lock(&mutex_exit);
				proceso_a_ejecutar->pcb->estado = EXT;
				list_add(cola_exit, proceso_a_ejecutar);
				pthread_mutex_unlock(&mutex_exit);

				log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <EXIT>", proceso_a_ejecutar->pcb->pid);
				log_info(logger_kernel, "Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>", proceso_a_ejecutar->pcb->pid);
				sem_post(&sem_exit); //despierta el proceso que saca a los procesos del sistema
				sem_post(&sem_ready);
			}

			log_warning(logger_kernel,
					"La última instrucción ejecutada fue WAIT");
			break;
		case SIGNAL:
			//Libera una instancia del recurso indicado por parámetro.

			proceso_a_ejecutar->ultima_instruccion = SIGNAL;
			
			int tamanio_parametro_signal = 0;

			memcpy(&tamanio_parametro_signal, stream, sizeof(int));
			stream += sizeof(int);

			char *recurso_signal = malloc(tamanio_parametro_signal);
			memset(recurso_signal, 0, tamanio_parametro_signal);

			memcpy(recurso_signal, stream, tamanio_parametro_signal);
			stream += tamanio_parametro_signal;

			if(dictionary_has_key(recursos, recurso_signal)) {
				//Lógica de liberar el recurso TODO
			} else {
				pthread_mutex_lock(&mutex_exit);
				proceso_a_ejecutar->pcb->estado = EXT;
				list_add(cola_exit, proceso_a_ejecutar);
				pthread_mutex_unlock(&mutex_exit);

				log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <EXIT>", proceso_a_ejecutar->pcb->pid);
				log_info(logger_kernel, "Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>", proceso_a_ejecutar->pcb->pid);
				sem_post(&sem_exit); //despierta el proceso que saca a los procesos del sistema
				sem_post(&sem_ready);
			}

			log_warning(logger_kernel,
					"La última instrucción ejecutada fue SIGNAL");
			break;
		case YIELD: //TERMINADO
			proceso_a_ejecutar->ultima_instruccion = YIELD;

			int fin_cpu = get_time();
			proceso_a_ejecutar->final_ultima_rafaga = fin_cpu;
			proceso_a_ejecutar->ultima_rafaga = proceso_a_ejecutar->final_ultima_rafaga - proceso_a_ejecutar->principio_ultima_rafaga;
			
			log_warning(logger_kernel, "La última instrucción ejecutada fue YIELD");
			pthread_mutex_lock(&mutex_ready);
			proceso_a_ejecutar->pcb->estado = READY;
			list_add(cola_ready, proceso_a_ejecutar);
			pthread_mutex_unlock(&mutex_ready);

			sem_post(&sem_ready);
			break;
		case EXIT: //TERMINADO
			pthread_mutex_lock(&mutex_exit);
			proceso_a_ejecutar->pcb->estado = EXT;
			list_add(cola_exit, proceso_a_ejecutar);
			pthread_mutex_unlock(&mutex_exit);

			log_info(logger_kernel, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <EXIT>", proceso_a_ejecutar->pcb->pid);
			log_info(logger_kernel, "Finaliza el proceso <%d> - Motivo: <SUCCESS>", proceso_a_ejecutar->pcb->pid);
			sem_post(&sem_exit); //despierta el proceso que saca a los procesos del sistema
			sem_post(&sem_ready);
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

		liberar_recursos_asignados(proceso->pcb->recursos_asignados);

		//Liberar las estructuras del proceso en memoria
		//enviar_pcb(socket_memoria, proceso->pcb);
		//log_info(logger_kernel, "Enviando pcb a memoria para liberar estructuras");
		//op_code codigo = esperar_respuesta_memoria(socket_memoria);
		//if(codigo != ESTRUCTURAS_LIBERADAS) {
		//	log_error(logger_kernel, "No se pudo eliminar de memoria a PCB id[%d]", proceso->pcb->pid);
		//}

		avisar_a_modulo(proceso->socket, FINALIZAR_CONSOLA);

		log_info(logger_kernel, "Ordenando a Consola la finalización del PID: <%d>", proceso->pcb->pid);

		eliminar_pcb(proceso->pcb);
		free(proceso);
		sem_post(&sem_grado_multiprogramacion);
	}
}

void eliminar_pcb(t_pcb* pcb) {
	//list_destroy_and_destroy_elements(pcb->segmentos, free);
	//list_destroy_and_destroy_elements(pcb->recursos_asignados, free);
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
		log_info(logger_kernel, "PID: <%d> saliendo de Cola de Ready por FIFO para ejecutar.", proceso->pcb->pid);
		break;
	case HRRN:
		aplicar_hrrn();
		proceso = list_remove(cola_ready, 0);
		log_info(logger_kernel, "PID: <%d> saliendo de Cola de Ready por HRRN para ejecutar.", proceso->pcb->pid);
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
//	struct timespec end;
//	clock_gettime(CLOCK_REALTIME, &end);
//
//	long seconds = end.tv_sec - proceso->pcb->llegada_ready.tv_sec;
//	long nanoseconds = end.tv_nsec - proceso->pcb->llegada_ready.tv_sec;
//	double elapsed = seconds + nanoseconds * 1e-9;

	int fin_espera = get_time();
	int elapsed = fin_espera - proceso->llegada_ready;

	proceso->tiempo_espera = elapsed; //TODO poner tiempo de llegada a ready en el hilo correspondiente estado_ready y estado_block
	proceso->response_ratio = 1 + (proceso->tiempo_espera / proceso->rafaga_estimada);
}

bool comparador_response_ratio(t_proceso* un_proceso, t_proceso* otro_proceso) {
	return un_proceso->response_ratio >= otro_proceso->response_ratio;
}

void mostrar_response_ratio(t_proceso* un_proceso) {
	log_info(logger_kernel, "PCB id[%d] tiene un RR de: %f", un_proceso->pcb->pid, un_proceso->response_ratio);
}

void calcular_estimacion(t_proceso* un_proceso) {
	double nueva_rafaga = (config_kernel.alfa_hrrn * un_proceso->rafaga_estimada) + ((un_proceso->ultima_rafaga) * (1 - config_kernel.alfa_hrrn));
	un_proceso->rafaga_estimada = nueva_rafaga;
}

//-----------------------------------------ESTADO BLOCK POR IO--------------------------------------------------------------------------------------
//Este estado es el encargado de gestionar la cola de bloqueados por IO, haciendo que ejecuten la interrupción y se bloqueen por el tiempo indicado.

void estado_block_io(void) {

	while(1) {
		sem_wait(&sem_block_io);

		pthread_mutex_lock(&mutex_block_io);
		t_proceso* proceso = list_remove(cola_block_io, 0);
		pthread_mutex_unlock(&mutex_block_io);

		log_info(logger_kernel, "PID: <%d> - Ejecuta IO: <%d>", proceso->pcb->pid, proceso->tiempo_bloqueo);
		sleep(proceso->tiempo_bloqueo);

		pthread_mutex_lock(&mutex_ready);
		list_add(cola_ready, proceso);
		pthread_mutex_unlock(&mutex_ready);

		proceso->llegada_ready = get_time();
		
		sem_post(&sem_ready); 
	}
}
