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
t_list* cola_block_io;
t_list* cola_exec;

void manejar_conexion(int* fd_cliente) {

	int socket_cliente = *fd_cliente;

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
		data = deserializar_el_string(paquete->buffer);

		t_proceso* proceso = malloc(sizeof(t_proceso));
		proceso->pcb = malloc(sizeof(t_pcb));

		proceso->pcb = crear_estructura_pcb(data);
		proceso->socket = socket_cliente;
		proceso->rafaga_estimada = config_kernel.estimacion_inicial;

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


		eliminar_paquete(paquete);
		break;
	default:
		log_warning(logger_kernel, "Operacion desconocida \n");
		break;
	}
}

int atender_clientes_kernel(int socket_servidor) {

	int* socket_cliente = malloc(sizeof(int));//esperar_cliente(logger_kernel,"KERNEL", socket_servidor); // se conecta el cliente
	*socket_cliente = accept(socket_servidor, NULL, NULL);

		//if(socket_cliente != -1) {
			pthread_t hilo_cliente;
			//manejar_conexion(socket_cliente);
			pthread_create(&hilo_cliente, NULL, (void*) manejar_conexion, socket_cliente); // creo el hilo con la funcion manejar conexion a la que le paso el socket del cliente y sigo en la otra funcion
			pthread_detach(hilo_cliente);

			return 1;
		//} else {
	//		log_error(logger_kernel, "Error al escuchar clientes... Finalizando servidor \n"); // log para fallo de comunicaciones
	//	}

	return 0;
}

t_pcb* crear_estructura_pcb(char* instrucciones) {

	int tamanio_proceso = strlen(instrucciones) + 1;

	t_pcb* un_pcb = malloc(sizeof(t_pcb));
	pthread_mutex_lock(&mutex_pid);
	un_pcb->pid = pid_global;
	un_pcb->pid_string = string_itoa(pid_global);
	pid_global++;
	pthread_mutex_unlock(&mutex_pid);
	un_pcb->tamanio_instrucciones = tamanio_proceso;
	un_pcb->instrucciones = string_duplicate(instrucciones);
	un_pcb->pc = 0;
	un_pcb->estado = NEW;
	un_pcb->registros = registros_iniciados;
	un_pcb->recursos_asignados = list_create();
	un_pcb->tabla_archivos = dictionary_create();

	pthread_mutex_lock(&mutex_operacion_memoria);
	un_pcb->tabla_segmentos.pid = un_pcb->pid;
	un_pcb->tabla_segmentos.segmentos = list_create();
	asignar_segmentos_de_memoria(un_pcb);
	pthread_mutex_unlock(&mutex_operacion_memoria);

	//un_pcb->rafaga_estimada = config_kernel.estimacion_inicial; // TODO las rafagas y todos los tiempos que necesita el proceso para calcular el HRRN ahora estan en la estructura del proceso.
	//un_pcb->rafaga_anterior = 0;
	//un_pcb->llegada_ready = 0; //ver función get_time()
	//un_pcb->response_ratio = 0;
	//un_pcb->tiempo_espera = 0;
	//Cada vez que agreguemos algo nuevo al pcb lo hacemos como ariba

	free(instrucciones);
	return un_pcb;
}

void agregar_proceso_a_new(t_proceso* proceso) {

	pthread_mutex_lock(&mutex_new);
	proceso->pcb->estado = NEW;
	proceso->desalojado = DESALOJADO;
	list_add(cola_new, proceso);

	pthread_mutex_lock(&mutex_procesos_en_el_sistema);
	dictionary_put(procesos_en_el_sistema, proceso->pcb->pid_string, proceso);
	pthread_mutex_unlock(&mutex_procesos_en_el_sistema);

	log_info(kernel_principal, "Se crea el proceso <%d> en NEW", proceso->pcb->pid);

	log_info(kernel_principal, "Cola NEW: ");
	mostrar_cola(cola_new);
	pthread_mutex_unlock(&mutex_new);

	//ATENCAO EMPIEZO A TIRAR MAGIA Y FILOSOFAR CREO QUE ES POR ACA
	sem_post(&sem_admitir);
}

void mostrar_cola(t_list* lista) {

    for (int j = 0; j < list_size(lista); j++){
    	t_proceso* proceso = list_get(lista, j);
        log_info(kernel_principal,"PID: <%d>",proceso->pcb->pid);
    }
}

//-----------------------------------------------------FUNCIONES EN DESUSO--------------------------------------------------------

void iniciar_conexion_cpu(char* ip_cpu, char* puerto_cpu) { //ESTA FUNCIÓN YA NO SE USA, QUEDA POR LAS DUDAS. LO NECESARIO YA ESTÁ EN EL MAIN
	socket_cpu = crear_conexion(logger_kernel, "KERNEL", ip_cpu, puerto_cpu);
}

void agregar_proceso_a_ready(void) { //ESTA FUNCIÓN YA NO LA USO, QUEDA POR LAS RISAS (POR LAS DUDAS)

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
