#include"utils_kernel2.h"
#include<shared-2.h>

int pid_global = 1000;
t_consola* consola;
t_list* cola_new;
t_list* cola_ready;
int socket_cpu;

typedef struct {
    t_log* log;
    int fd;
} t_manejar_conexion_args;

void manejar_conexion(void *void_args) {
	t_manejar_conexion_args *args = (t_manejar_conexion_args*) void_args;
	t_log *logger = args->log;
	int socket_cliente = args->fd;

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
		log_info(logger, "Me llegó una Consola");
		data = deserializar_string(paquete->buffer);

		break;
	default:
		log_warning(logger, "Operacion desconocida \n");
		break;
	}


//	t_manejar_conexion_args *args = (t_manejar_conexion_args*) void_args;
//	t_log *logger = args->log;
//	int socket_cliente = args->fd;
//
//	int codigo_operacion = recibir_operacion(socket_cliente);
//
//	switch (codigo_operacion) {
//	case CONSOLA:
//		char *instrucciones = string_new();
//		instrucciones = recibir_instrucciones_como_string(socket_cliente, logger);
//		consola = deserializar_consola(instrucciones, logger);
//		log_info(logger, "Consola deserializada, armando PCB");
//
//		t_proceso *proceso = malloc(sizeof(t_proceso));
//		proceso->pcb = malloc(sizeof(t_pcb));
//		proceso->pcb = crear_estructura_pcb(consola);
//		proceso->socket = socket_cliente;
//
//		log_info(logger, "PCB id[%d] armada -> agregado el proceso a NEW", proceso->pcb->pid);
//		agregar_pcb_a_new(proceso, logger);
//
////		-------------------------------------------------------------
////		|     														|
////		|		OJO CON LO QUE VIENE ACÁ, ES SOLAMENTE				|
////		|		PARA PROBARLO, EN EL FUTURO TIENE QUE SER UN HILO   |
////		|		QUE PLANIFIQUE TODO SOLO CON SEMÁFOROS Y DEMÁS		|
////		|															|
////		-------------------------------------------------------------
//		// wait(cola_ready);
//		getchar();
//		agregar_proceso_a_ready(logger);
//		getchar();
//		ejecutar_proceso(socket_cpu, logger);
//
//		break;
//	case PAQUETE_CONSOLA:
//		log_info(logger, "Me llegaron el tamanio y las instrucciones");
//		//pthread_mutex_lock(&mutex_consola);
//		//consola = deserializar_consola(socket_cliente);
//		//pthread_mutex_unlock(&mutex_consola);
//		log_info(logger, "Consola deserializada, se arma el PCB\n");
//		t_proceso *procesos = malloc(sizeof(t_proceso));
//		procesos->pcb = malloc(sizeof(t_pcb));
//		//pthread_mutex_lock(&mutex_consola);
//		procesos->pcb = crear_estructura_pcb(consola);
//		//pthread_mutex_unlock(&mutex_consola);
//		procesos->socket = socket_cliente;
//		log_info(logger, "PCB id[%d] armada -> agregar proceso a new", procesos->pcb->pid);
//		agregar_pcb_a_new(procesos, logger);
//		break;
//	case PAQUETE:
//		log_info(logger, "Me llego el paquete:\n");
//		break;
//	case MENSAJE:
//		log_info(logger, "Recibí un mensaje.");
//		break;
//	default:
//		log_warning(logger, "Operacion desconocida \n");
//		break;
//	}

}

char* deserializar_string(t_buffer* buffer) {
	char* string = malloc(buffer->stream_size);
	memset(string, 0, buffer->stream_size);

	void* stream = buffer->stream;
	memcpy(string, stream, buffer->stream_size);

	return string;
}


int atender_clientes_kernel(int socket_servidor, t_log* logger) {

	int socket_cliente = esperar_cliente(logger,"KERNEL", socket_servidor); // se conecta el cliente

		if(socket_cliente != -1) {
			//pthread_t hilo_cliente;
			t_manejar_conexion_args* args = malloc(sizeof(t_manejar_conexion_args));
			args->fd = socket_cliente;
			args->log = logger;
			manejar_conexion(args);
			//pthread_create(&hilo_cliente, NULL, (void*) manejar_conexion, (void *) args); // creo el hilo con la funcion manejar conexion a la que le paso el socket del cliente y sigo en la otra funcion
			//pthread_detach(hilo_cliente);
			free(args);
			return 1;
		} else {
			log_error(logger, "Error al escuchar clientes... Finalizando servidor \n"); // log para fallo de comunicaciones
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

t_list *deserializar_instrucciones(t_list *datos, int longitud_datos) {
	t_list *instrucciones = list_create();


  	for(int i = 0; i < longitud_datos; i += 4) {
  		t_instruccion *instruccion_recibida = malloc(sizeof(t_instruccion));
  		instruccion_recibida->nombre = *(nombre_instruccion *)list_get(datos, i);
  		instruccion_recibida->parametro_1 = list_get(datos, i + 1);
  		instruccion_recibida->parametro_2 = list_get(datos, i + 2);
  		instruccion_recibida->parametro_3 = list_get(datos, i + 3);
  		printf("%d %s %s %s\n", instruccion_recibida->nombre, instruccion_recibida->parametro_1, instruccion_recibida->parametro_2, instruccion_recibida->parametro_3);
  		list_add(instrucciones, instruccion_recibida);
  	}

  	return instrucciones;
}

t_consola *deserializar_consola(char* instrucciones, t_log* logger) {

  	t_consola *consola = malloc(sizeof(t_consola));

  	consola->tamanio_proceso = strlen(instrucciones);

  	printf("\n El tamanio del proceso es: %d", consola->tamanio_proceso);

  	consola->instrucciones = string_duplicate(instrucciones);//deserializar_instrucciones(datos, list_size(datos));

  	free(instrucciones);

  	return consola;
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

char* recibir_instrucciones_como_string(int socket_cliente, t_log* logger) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));


	recv(socket_cliente, &(paquete->buffer->stream_size), sizeof(uint32_t), 0);
	paquete->buffer->stream = malloc(paquete->buffer->stream_size);
	recv(socket_cliente, paquete->buffer->stream, paquete->buffer->stream_size, 0);

	char* mensaje_recibido = malloc(paquete->buffer->stream_size);
	void* stream = paquete->buffer->stream;

	memcpy(mensaje_recibido, stream, paquete->buffer->stream_size);

	mensaje_recibido = string_duplicate(paquete->buffer->stream);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	return mensaje_recibido;
}

void recibir_instruccion_serializada(int socket_cliente) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	recv(socket_cliente, &(paquete->buffer->stream_size), sizeof(uint32_t), 0);

	paquete->buffer->stream = malloc(paquete->buffer->stream_size);

	recv(socket_cliente, paquete->buffer->stream, paquete->buffer->stream_size, 0);

	t_instruccion* instruccion_recibida = malloc(sizeof(t_instruccion));

	void* stream = paquete->buffer->stream;
	int desplazamiento = 0;

	memcpy(&(instruccion_recibida->nombre), stream, sizeof(nombre_instruccion));
	stream += sizeof(nombre_instruccion);
	printf("\nLa isntruccion recibida fue: %d, ", instruccion_recibida->nombre);

	memcpy(&(instruccion_recibida->parametro_1_length), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(instruccion_recibida->parametro_1, stream, instruccion_recibida->parametro_1_length);
	desplazamiento = (strlen(instruccion_recibida->parametro_1) + 1);
	stream += desplazamiento;
	printf("%s, ", instruccion_recibida->parametro_1);

	memcpy(&(instruccion_recibida->parametro_2_length), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(instruccion_recibida->parametro_2, stream, instruccion_recibida->parametro_2_length);
	desplazamiento = (strlen(instruccion_recibida->parametro_2) + 1);
	printf("El largo del string fue de %d, debería ser de %d", desplazamiento, instruccion_recibida->parametro_2_length);
	stream += instruccion_recibida->parametro_2_length;

	printf("%s, ", instruccion_recibida->parametro_2);
	memcpy(&(instruccion_recibida->parametro_3_length), stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	memcpy(instruccion_recibida->parametro_3, stream, instruccion_recibida->parametro_3_length);
	printf("%s", instruccion_recibida->parametro_3);

	printf("\nLa isntruccion recibida fue: %d, %s, %s, %s", instruccion_recibida->nombre, instruccion_recibida->parametro_1, instruccion_recibida->parametro_2, instruccion_recibida->parametro_3);
}

void enviar_pcb(int socket_servidor, t_pcb* pcb) {
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream_size = sizeof(int) * 4 + sizeof(estado_proceso) + sizeof(t_registros) + (strlen(pcb->instrucciones) + 1);

	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, &(pcb->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(pcb->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, pcb->instrucciones, strlen(pcb->instrucciones) + 1);
	offset += (strlen(pcb->instrucciones) + 1);
	memcpy(stream + offset, &(pcb->pc), sizeof(estado_proceso));
	offset += sizeof(estado_proceso);
	memcpy(stream + offset, &(pcb->tamanio), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(pcb->registros), sizeof(t_registros));

	buffer->stream = stream;

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = PCB;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(uint8_t) + sizeof(uint32_t));
	int desplazamiento = 0;

	memcpy(a_enviar + desplazamiento, &(paquete->codigo_operacion), sizeof(uint8_t));
	desplazamiento +=sizeof(uint8_t);
	memcpy(a_enviar + desplazamiento, &(paquete->buffer->stream_size), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(a_enviar + desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(socket_servidor, a_enviar, buffer->stream_size + sizeof(uint8_t) + sizeof(uint32_t), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

t_pcb* recibir_pcb(int socket_servidor) {

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	recv(socket_servidor, &(paquete->codigo_operacion), sizeof(uint8_t), 0);

	recv(socket_servidor, &(paquete->buffer->stream_size), sizeof(uint32_t), 0);
	paquete->buffer->stream = malloc(paquete->buffer->stream_size);
	recv(socket_servidor, paquete->buffer->stream, paquete->buffer->stream_size, 0);

	switch(paquete->codigo_operacion) {
	case PCB:
		t_pcb* pcb = deserializar_pcb(paquete->buffer);
		return pcb;
	default:
		printf("ERROR");
		return 0;
	}
}

t_pcb* deserializar_pcb(t_buffer* buffer) {
	t_pcb* pcb = malloc(sizeof(t_pcb));

	void* stream = buffer->stream;

	memcpy(&(pcb->pid), stream, sizeof(int));
	stream += sizeof(int);
	memcpy(&(pcb->tamanio_instrucciones), stream, sizeof(int));
	stream += sizeof(int);
	pcb->instrucciones = malloc(pcb->tamanio_instrucciones);
	memcpy(pcb->instrucciones, stream, pcb->tamanio_instrucciones);
	stream += pcb->tamanio_instrucciones;
	memcpy(&(pcb->estado), stream, sizeof(estado_proceso));
	stream += sizeof(estado_proceso);
	memcpy(&(pcb->pc), stream, sizeof(int));
	stream += sizeof(int);
	memcpy(&(pcb->tamanio), stream, sizeof(int));
	stream += sizeof(int);
	memcpy(&(pcb->registros), stream, sizeof(t_registros));

	return pcb;
}
