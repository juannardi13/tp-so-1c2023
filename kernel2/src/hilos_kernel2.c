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
			//pthread_create(&hilo_cliente, NULL, (void*) manejar_conexion, (void *) args); // creo el hilo con la funcion manejar conexion a la que le paso el socket del cliente y sigo en la otra funcion
			//pthread_detach(hilo_cliente);

			return 1;
		} else {
			log_error(logger_kernel, "Error al escuchar clientes... Finalizando servidor \n"); // log para fallo de comunicaciones
		}

	return 0;
}

void agregar_proceso_a_ready(t_log* logger) {
	int grado_de_multiprogramacion = 4;


	if(list_size(cola_ready) >= grado_de_multiprogramacion) { // Esto después tiene que ser implementado con semáforos contadores
		list_add(cola_ready, list_remove(cola_new, 0));
	}

	log_info(logger, "Cola de Ready: ");

	for(int i = 0; i < list_size(cola_ready); i++) {
		t_proceso* un_proceso = malloc(sizeof(t_proceso)) ;
		un_proceso = list_get(cola_ready, i);
		log_info(logger, "PCB id[%d]", un_proceso->pcb->pid);
		free(un_proceso);
	}
}

void ejecutar_proceso(int socket_cpu, t_log* logger) {

	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso = list_get(cola_ready, 0); //obtener_proceso_cola_ready();

	enviar_pcb(socket_cpu, proceso->pcb);

	log_info(logger, "Proceso id[%d] enviado a CPU para ejecutar.", proceso->pcb->pid);

	op_code respuesta_cpu = recibir_operacion(socket_cpu);

	proceso->pcb = recibir_pcb(socket_cpu);

}





t_pcb* crear_estructura_pcb(t_consola* consola) {

	t_pcb* un_pcb = malloc(sizeof(t_pcb));
	//pthread_mutex_lock(&mutex_pid);
	un_pcb->pid = pid_global;
	pid_global++;
	//pthread_mutex_unlock(&mutex_pid);
	un_pcb->tamanio =consola->tamanio_proceso;
	un_pcb->instrucciones = string_duplicate(consola->instrucciones);
	un_pcb->pc = 0;
	un_pcb->estado = NEW;
	//Cada vez que agreguemos algo nuevo al pcb lo hacemos como ariba

	return un_pcb;
}

void iniciar_planificador_largo_plazo(void) {
	//Aca creo todos los semáforos y las estructuras necesarias para inicializar la planificación, por ahora solo tiene esto:

	cola_new = list_create();


}

void iniciar_planificador_mediano_plazo(void) {
	cola_ready = list_create();
}

void iniciar_conexion_cpu(char* ip_cpu, char* puerto_cpu, t_log* logger) {
	socket_cpu = crear_conexion(logger, "KERNEL", ip_cpu, puerto_cpu);
}

void agregar_pcb_a_new(t_proceso* proceso, t_log* logger) {

	list_add(cola_new, proceso);
	proceso->pcb->estado = NEW;
	log_info(logger, "PID[%d] ingresa a NEW \n", proceso->pcb->pid);
	mostrar_cola_new(cola_new, logger);

}

void mostrar_cola_new(t_list* lista, t_log* logger) {

      for (int j = 0; j < list_size(lista); j++){
          t_proceso* proceso = list_get(lista, j);
          log_info(logger,"PCB ID: %d\n",proceso->pcb->pid);
      }
}






