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
		agregar_proceso_a_ready();

		ejecutar_proceso();

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

void ejecutar_proceso(void) {

	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso = obtener_proceso_cola_ready();

	//enviar_pcb(socket_cpu, proceso->pcb);

	log_info(logger_kernel, "Proceso id[%d] enviado a CPU para ejecutar.", proceso->pcb->pid);

	op_code respuesta_cpu = recibir_operacion(socket_cpu);

	proceso->pcb = recibir_pcb(socket_cpu);

	switch(respuesta_cpu) {
	case IO:
		// Bloquear la cantidad de tiempo indicada
		log_warning(logger_kernel, "La última instrucción ejecutada fue IO");
		break;
	case F_OPEN:
		//Abrir o crear el archivo pasado por parámetro
		log_warning(logger_kernel, "La última instrucción ejecutada fue F_OPEN");
		break;
	case F_CLOSE:
		//Cerrar el archivo pasado por parámetro
		log_warning(logger_kernel, "La última instrucción ejecutada fue F_CLOSE");
		break;
	case F_SEEK:
		//Actualizar el puntero del archivo a la posición pasada por parámetro
		log_warning(logger_kernel, "La última instrucción ejecutada fue F_SEEK");
		break;
	case F_READ:
		//Leer del archivo indicado la cantidad de bytes pasados por parámetro y escribirlo en la dirección física de memoria
		log_warning(logger_kernel, "La última instrucción ejecutada fue F_READ");
		break;
	case F_WRITE:
		//Esta instrucción solicita al Kernel que se escriba en el archivo indicado la cantidad de bytes pasados por parámetro cuya información es obtenida a partir de la dirección física de Memoria.
		log_warning(logger_kernel, "La última instrucción ejecutada fue F_WRITE");
		break;
	case F_TRUNCATE:
		//Modificar el tamaño del archivo al indicado por parámetro.
		log_warning(logger_kernel, "La última instrucción ejecutada fue F_TRUNCATE");
		break;
	case WAIT:
		//Se asigne una instancia del recurso indicado por parámetro
		log_warning(logger_kernel, "La última instrucción ejecutada fue WAIT");
		break;
	case SIGNAL:
		//Libera una instancia del recurso indicado por parámetro.
		log_warning(logger_kernel, "La última instrucción ejecutada fue SIGNAL");
		break;
	case YIELD:
		//Se pone en cola de ready al proceso de nuevo.
		log_warning(logger_kernel, "La última instrucción ejecutada fue YIELD");
		break;
	case EXIT:
		//Finalización del proceso.
		//pthread_mutex_lock(&mutex_exit);
		proceso->pcb->estado = EXT;
		list_add(cola_exit, proceso);
		//pthread_mutex_unlock(&mutex_exit);
		log_info(logger_kernel, "PCB id[%d] sale de EXEC y entra a EXIT", proceso->pcb->pid);
		//sem_post(&sem_exit); //despierta el proceso que saca a los procesos del sistema
		//sem_post(&sem_ready);

		log_warning(logger_kernel, "La última instrucción ejecutada fue EXIT");
		break;
	default:
		log_error(logger_kernel, "Instrucción desconocida, ocurrió un error");
		break;
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
	//pthread_mutex_lock(&mutex_pid);
	un_pcb->pid = pid_global;
	pid_global++;
	//pthread_mutex_unlock(&mutex_pid);
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

void iniciar_planificador_largo_plazo(void) {
	//Aca creo todos los semáforos y las estructuras necesarias para inicializar la planificación, por ahora solo tiene esto:
	//pthread_mutex_init(&mutex_new, NULL);
	//pthread_mutex_init(&mutex_exit, NULL);
	//pthread_mutex_init(&mutex_pid, NULL);
	//sem_init(&sem_exit, 0, 0);
	//sem_init(&sem_grado_multiprogramacion, 0, config_kernel.grado_multiprogramacion); //semaforo contador para la cantidad de procesos en ready


	cola_new = list_create();
	cola_exit = list_create();

	//pthread_create(&thread_exit, NULL, (void*)estado_exit, NULL);
	//pthread_detach(thread_exit);
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

void avisar_a_modulo(int socket, op_code codigo) {
	enviar_datos(socket, &codigo, sizeof(op_code));
}

int enviar_datos(int socket, void* datos, int size) {
	return send(socket, datos, size, 0);
}

int recibir_datos(int socket, void* destino, int size) {
	return send(socket, destino, size, 0);
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

void iniciar_conexion_cpu(char* ip_cpu, char* puerto_cpu) {
	socket_cpu = crear_conexion(logger_kernel, "KERNEL", ip_cpu, puerto_cpu);
}

void agregar_proceso_a_new(t_proceso* proceso) {

	proceso->pcb->estado = NEW;
	list_add(cola_new, proceso);
	log_info(logger_kernel, "PID[%d] ingresa a NEW \n", proceso->pcb->pid);
	mostrar_cola_new(cola_new);

}

void mostrar_cola_new(t_list* lista) {

	log_warning(logger_kernel, "PCBs en lista NEW:");

    for (int j = 0; j < list_size(lista); j++){
    	t_proceso* proceso = list_get(lista, j);
        log_info(logger_kernel,"PCB ID: %d",proceso->pcb->pid);
    }
}
