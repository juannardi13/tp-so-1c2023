#include"utils_kernel2.h"
#include<shared-2.h>

//Definición de las variables globales
int socket_cpu;
int socket_memoria;
int socket_filesystem;
int pid_global = 1000;
t_consola* consola;
t_list* cola_new;
t_list* cola_ready;
t_list* cola_exit;

void manejar_conexion(int socket_cliente) {

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	recv(socket_cliente, &(paquete->codigo_operacion), sizeof(int), 0);

	recv(socket_cliente, &(paquete->buffer->stream_size), sizeof(int), 0);
	paquete->buffer->stream = malloc(paquete->buffer->stream_size);
	recv(socket_cliente, paquete->buffer->stream, paquete->buffer->stream_size, 0);

	void* data;

	switch(paquete->codigo_operacion)
	{
	case CONSOLA:
		log_info(logger_kernel, "Me llegó una Consola");
		data = deserializar_string(paquete->buffer);
		log_info(logger_kernel, "Consola deserializada, se arma el PCB a continuación");

		t_proceso* proceso = malloc(sizeof(t_proceso));
		proceso->pcb = malloc(sizeof(t_pcb));

		proceso->pcb = crear_estructura_pcb(data);
		proceso->socket = socket_cliente;

		log_info(logger_kernel, "PCB id[%d] armada, agregando a cola NEW", proceso->pcb->pid);

//		avisar_a_memoria_crear_estructuras();

		agregar_proceso_a_new(proceso);
//
//		-----------------------------------------------------------------
//		|																 |
//		|		CUIDADO! LO QUE VIENE A PARTIR DE AHORA LO TENDRÍAN	     |
//		|		QUE MANEJAR LOS HILOS QUE TODAVÍA NO SE CREARON			 |
//		|		PARA PROBAR SOLAMENTE SE HACE ESTO.						 |
//		|																 |
//		------------------------------------------------------------------
//
	//	agregar_proceso_a_ready();

	//	ejecutar_proceso();

		break;
	default:
		log_warning(logger_kernel, "Operacion desconocida \n");
		break;
	}

}

int atender_clientes_kernel(int socket_servidor) {

	int socket_cliente = esperar_cliente(logger_kernel,"KERNEL", socket_servidor); // se conecta el cliente

		if(socket_cliente != -1) {
			//pthread_t hilo_cliente;
			manejar_conexion(socket_cliente);
			//pthread_create(&hilo_cliente, NULL, (void*) manejar_conexion, (void *) socket_cliente); // creo el hilo con la funcion manejar conexion a la que le paso el socket del cliente y sigo en la otra funcion
			//pthread_detach(hilo_cliente);

			return 1;
		} else {
			log_error(logger_kernel, "Error al escuchar clientes... Finalizando servidor \n"); // log para fallo de comunicaciones
		}

	return 0;
}

void agregar_proceso_a_ready(void) {

	if(list_size(cola_ready) <= config_kernel.grado_multiprogramacion) { // TODO Esto después tiene que ser implementado con semáforos contadores
		t_proceso* proceso_auxiliar = malloc(sizeof(t_proceso));
		proceso_auxiliar = list_remove(cola_new, 0);
		list_add(cola_ready, proceso_auxiliar);

		log_info(logger_kernel, "PCB id[%d] agregado a Cola de Ready", proceso_auxiliar->pcb->pid);
		//free(proceso_auxiliar);
	}

	log_info(logger_kernel, "Cola de Ready: ");

	for(int i = 0; i < list_size(cola_ready); i++) {
		t_proceso* un_proceso = malloc(sizeof(t_proceso)) ;
		un_proceso = list_get(cola_ready, i);
		log_info(logger_kernel, "PCB id[%d]", un_proceso->pcb->pid);
	}
}

void estado_ejecutar(void) {

	while (1) {
		sem_wait(&sem_exec);
		pthread_mutex_lock(&mutex_exec);
		t_proceso* proceso_a_ejecutar = list_get(cola_exec, 0); //esto ayuda para cuando se necesita devolver el mismo proceso a cpu, como en las instrucciones de memoria
		pthread_mutex_unlock(&mutex_exec);

		enviar_pcb(socket_cpu, proceso->pcb);
		log_info(logger_kernel, "PCB id[%d] enviada a CPU", proceso_a_ejecutar->pcb->pid);

		op_code respuesta_cpu = recibir_operacion(socket_cpu);

		proceso_a_ejecutar->pcb = recibir_pcb(socket_cpu);

		switch (respuesta_cpu) {
		case IO:
			pthread_mutex_lock(&mutex_block);
			proceso_a_ejecutar->pcb->estado = BLOCK;
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

t_proceso* obtener_proceso_cola_ready(void) {

	t_proceso* proceso = malloc(sizeof(t_proceso));

	switch(config_kernel.algoritmo_planificacion){
	case FIFO:
		proceso = list_remove(cola_ready, 0);
		log_info(logger_kernel, "PCB id[%d] saliendo de Cola de Ready para ejecutar.", proceso->pcb->pid);
		break;
	case HRRN:
		//Lógica de selección del proceso mediante el HRRN
		break;
	default:
		log_error(logger_kernel, "Error al elegir un proceso de la cola de Ready para ejecutar");
		break;
	}

	return proceso;
}

t_pcb* crear_estructura_pcb(char* instrucciones) {

	int tamanio_proceso = strlen(instrucciones) + 1;

	t_pcb* un_pcb = malloc(sizeof(t_pcb));
	pthread_mutex_lock(&mutex_pid);
	un_pcb->pid = pid_global;
	pid_global++;
	pthread_mutex_unlock(&mutex_pid);
	un_pcb->tamanio_instrucciones = tamanio_proceso;
	un_pcb->instrucciones = string_duplicate(instrucciones);
	un_pcb->pc = 0;
	un_pcb->estado = NEW;
	un_pcb->tamanio = tamanio_proceso;
	un_pcb->registros = registros_iniciados;
	//un_pcb->estimado_proxima_rafaga = config_kernel.estimacion_inicial;
	//un_pcb->llegada_ready = ; //ver función get_time()
	//Cada vez que agreguemos algo nuevo al pcb lo hacemos como ariba

	free(instrucciones);
	return un_pcb;
}

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
		//log_info(logger_kernel, "Enviando pcb a memoria para liberar estructuras
		//op_code codigo = esperar_respuesta_memoria(socket_memoria);
		//if(codigo != ESTRUCTURAS_LIBERADAS) {
		//	log_error(logger_kernel, "No se pudo eliminar de memoria a PCB id[%d]", proceso->pcb->pid);
		//}

		avisar_a_modulo(proceso->socket, FINALIZAR_CONSOLA);

		log_info(logger_kernel, "Ordenando a Consola la finalización del proceso");

		eliminar_pcb(proceso->pcb);
		free(proceso);
		//sem_post(&sem_grado_multiprogramacion);
	}
}

void eliminar_pcb(t_pcb pcb) {
	//list_destroy_and_destroy_elements(pcb->segmentos, free);
	//TODO seguir destruyendo las eventuales listas de la pcb.
}

void iniciar_conexion_cpu(char* ip_cpu, char* puerto_cpu) {
	socket_cpu = crear_conexion(logger_kernel, "KERNEL", ip_cpu, puerto_cpu);
}

void agregar_proceso_a_new(t_proceso* proceso) {

	pthread_mutex_lock(&mutex_new);
	proceso->pcb->estado = NEW;
	list_add(cola_new, proceso);

	log_info(logger_kernel, "PCB id[%d] ingresa a NEW", proceso->pcb->pid);

	mostrar_cola_new(cola_new);
	pthread_mutex_unlock(&mutex_new);

	//ATENCAO EMPIEZO A TIRAR MAGIA Y FILOSOFAR CREO QUE ES POR ACA
	sem_post(&sem_admitir);

}

void admitir_procesos_a_ready(void) { //hilo

	while(1) {
		sem_wait(&sem_admitir);
		sem_wait(&sem_grado_multiprogramacion);

		t_proceso* proceso;

		pthread_mutex_lock(&mutex_new);
		proceso = list_remove(cola_new, 0);
		pthread_mutex_unlock(&mutex_new);

		//Puedo hacer que los segmentos de memoria se los demos al proceso acá o apenas entra a cola new TODO

		log_info(logger_kernel, "PCB id[%d] ingresa a READY desde NEW", proceso->pcb->pid);

		pthread_mutex_lock(&mutex_ready);
		list_add(cola_ready, proceso);
		pthread_mutex_unlock(&mutex_ready);

		sem_post(&sem_ready);
	}

}

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

void mostrar_cola_new(t_list* lista) {

	log_warning(logger_kernel, "PCBs en lista NEW:");

    for (int j = 0; j < list_size(lista); j++){
    	t_proceso* proceso = list_get(lista, j);
        log_info(logger_kernel,"PCB ID: %d",proceso->pcb->pid);
    }
}
