#include"../include/utils_kernel.h"

int pid_global = 1000;
t_consola* consola;

typedef struct {
    t_log* log;
    int fd;
} t_manejar_conexion_args;

void manejar_conexion(void* void_args) {

	t_manejar_conexion_args* args = (t_manejar_conexion_args*) void_args;
	t_log* logger = args->log;
	int socket_cliente = args->fd;

	int codigo_operacion = 2; //recibir_operacion_nuevo(socket_cliente);

	  	switch (codigo_operacion) {
	  	case MENSAJE:
	  		recibir_mensaje(socket_cliente, logger);
	  		break;
	  	case PAQUETE_CONSOLA:
	  		log_info(logger, "Me llegaron el tamanio y las instrucciones");
	  		//pthread_mutex_lock(&mutex_consola);
			consola = deserializar_consola(socket_cliente);
			//pthread_mutex_unlock(&mutex_consola);
	  		log_info(logger, "Consola deserializada, se arma el PCB\n");
	  		t_proceso* procesos = malloc(sizeof(t_proceso));
	  		procesos->pcb = malloc(sizeof(t_pcb));
			//pthread_mutex_lock(&mutex_consola);
	  		procesos->pcb = crear_estructura_pcb(consola);
			//pthread_mutex_unlock(&mutex_consola);
	  		procesos->socket = socket_cliente;
	  		log_info(logger, "PCB id[%d] armada -> agregar proceso a new",procesos->pcb->pid);
	  		//agregar_a_new_pcb(procesos);
	  		break;
	  	case PAQUETE:
			log_info(logger, "Me llego el paquete:\n");
	  		break;

	      default:
	          log_warning(logger, "Operacion desconocida \n");
	          break;
	  	}
}

int atender_clientes_kernel(int socket_servidor, t_log* logger){

	int socket_cliente = esperar_cliente(socket_servidor); // se conecta el cliente

		if(socket_cliente != -1) {
			pthread_t hilo_cliente;
			t_manejar_conexion_args* args = malloc(sizeof(t_manejar_conexion_args));
			args->fd = socket_servidor;
			args->log = logger;
			printf("\n\nHOLAAAASDASD\n\n");
			pthread_create(&hilo_cliente, NULL, (void*) manejar_conexion, (void *) args); // creo el hilo con la funcion manejar conexion a la que le paso el socket del cliente y sigo en la otra funcion
			pthread_detach(hilo_cliente);
			return 1;
		} else {
			log_error(logger, "Error al escuchar clientes... Finalizando servidor \n"); // log para fallo de comunicaciones
		}


	return 0;
}

t_list *deserializar_instrucciones(t_list *datos, int longitud_datos) {
	t_list *instrucciones = list_create();
	printf("HOLAAAA INSTRUCCION\n\n\n\n %d", longitud_datos);

  	for(int i = 0; i < longitud_datos; i += 4) {
  		t_instruccion *instruccion_recibida = malloc(sizeof(t_instruccion));
  		instruccion_recibida->nombre = *(nombre_instruccion *)list_get(datos, i);
  		instruccion_recibida->parametro_1 = *(char**)list_get(datos, i + 1);
  		instruccion_recibida->parametro_2 = *(char**)list_get(datos, i + 2);
  		instruccion_recibida->parametro_3 = *(char**)list_get(datos, i + 3);
  		printf("HOLAAAA INSTRUCCION: %d", instruccion_recibida->nombre);
  		list_add(instrucciones, instruccion_recibida);
  	}

  	return instrucciones;
}

t_consola *deserializar_consola(int  socket_cliente) {

	t_list *datos = recibir_paquete(socket_cliente);
  	t_consola *consola = malloc(sizeof(t_consola));

  	//consola->tamanio_proceso = *(uint32_t *)list_remove(datos, 0);

  	consola->instrucciones = deserializar_instrucciones(datos, list_size(datos));

  	return consola;
}

t_pcb* crear_estructura_pcb(t_consola *consola) {

	t_pcb* un_pcb = malloc(sizeof(t_pcb));
	//pthread_mutex_lock(&mutex_pid);
	un_pcb->pid = pid_global;
	pid_global++;
	//pthread_mutex_unlock(&mutex_generador_id);
	un_pcb->tamanio =consola->tamanio_proceso;
	un_pcb->instrucciones = list_duplicate(consola->instrucciones);
	un_pcb->pc = 0;
	//Cada vez que agreguemos algo nuevo al pcb lo hacemos como ariba

	return un_pcb;
}
