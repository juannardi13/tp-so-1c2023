#include"utils_kernel2.h"
#include<shared-2.h>

int pid_global = 1000;
t_consola* consola;
t_list* cola_new;

typedef struct {
    t_log* log;
    int fd;
} t_manejar_conexion_args;

void manejar_conexion(void* void_args) {

	t_manejar_conexion_args* args = (t_manejar_conexion_args*) void_args;
	t_log* logger = args->log;
	int socket_cliente = args->fd;

	int codigo_operacion = recibir_operacion(socket_cliente);


	  	switch (codigo_operacion) {
	  	case MENSAJE:
	  		recibir_mensaje(logger, socket_cliente);
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
	  		agregar_pcb_a_new(procesos, logger);
	  		break;
	  	case PAQUETE:
			log_info(logger, "Me llego el paquete:\n");
	  		break;

	      default:
	          log_warning(logger, "Operacion desconocida \n");
	          break;
	  	}

}


int atender_clientes_kernel(int socket_servidor, t_log* logger) {

	iniciar_planificador_largo_plazo(); //Esto despues tiene que ir en main_kernel.c
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

t_consola *deserializar_consola(int  socket_cliente) {

	t_list *datos = recibir_paquete(socket_cliente);

  	t_consola *consola = malloc(sizeof(t_consola));

  	consola->tamanio_proceso = *(uint32_t *)list_remove(datos, 0);

  	printf("\n El tamanio del proceso es: %d", consola->tamanio_proceso);

  	consola->instrucciones = deserializar_instrucciones(datos, list_size(datos));

  	return consola;
}

t_pcb* crear_estructura_pcb(t_consola *consola) {

	t_pcb* un_pcb = malloc(sizeof(t_pcb));
	//pthread_mutex_lock(&mutex_pid);
	un_pcb->pid = pid_global;
	pid_global++;
	//pthread_mutex_unlock(&mutex_pid);
	un_pcb->tamanio =consola->tamanio_proceso;
	un_pcb->instrucciones = list_duplicate(consola->instrucciones);
	un_pcb->pc = 0;
	//Cada vez que agreguemos algo nuevo al pcb lo hacemos como ariba

	return un_pcb;
}

void iniciar_planificador_largo_plazo(void) {
	//Aca creo todos los semáforos y las estructuras necesarias para inicializar la planificación, por ahora solo tiene esto:

	cola_new = list_create();


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
          log_info(logger, "instrucciones del proceso [%d]: ", proceso->pcb->pid);
          t_list* lista_instrucciones = list_create();
          lista_instrucciones = list_duplicate(proceso->pcb->instrucciones);

          for(int i = 0; i < list_size(lista_instrucciones); i++) {
            		t_instruccion *instruccion_recibida = malloc(sizeof(t_instruccion));
            		instruccion_recibida = list_get(lista_instrucciones, i);
            		log_info(logger, "%d, %s, %s, %s.", instruccion_recibida->nombre, instruccion_recibida->parametro_1, instruccion_recibida->parametro_2, instruccion_recibida->parametro_3);
            		free(instruccion_recibida);
            	}
      }

  }

