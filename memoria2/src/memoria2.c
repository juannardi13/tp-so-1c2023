#include "memoria.h"
#include <pthread.h>

int main(void) {

	inicializar_config();

	logger = log_create("memoria.log", "MEMORIA", 1, LOG_LEVEL_INFO);

	iniciar_memoria(); // se reserva memoria y crea el bitmap para huecos libres
	log_info(logger, "Se inicializaron las estructuras de Memoria");

	int memoria_fd = iniciar_servidor(config_memoria.puerto_escucha);
	log_info(logger, "Servidor Memoria iniciado, esperando conexiones entrantes");

	while(esperar_conexiones(memoria_fd));
	//esperar_conexiones(memoria_fd); // 1-FileSystem 2-CPU 3-Kernel

	//pthread_join(hilos[0], NULL); // Memoria termina cuando terminan los hilos

	int cont = 0, desp = 0;
	while (desp < config_memoria.tam_memoria) {
		if (! bitarray_test_bit(bitmap, desp)) {
			cont++;
		}
		desp++;
	}

	printf("Memoria restante: %dB\n", cont);

	terminar_memoria(); // so far NO memory leaks

	return EXIT_SUCCESS;
}

void inicializar_config(void) {

	config = config_create("./cfg/memoria.config");

	if (config == NULL) {
		perror("No se encontró el path de la config");
		exit(1);
	}

	config_memoria.puerto_escucha 		 = config_get_string_value(config, "PUERTO_ESCUCHA");
	config_memoria.tam_memoria 		 	 = config_get_int_value	  (config, "TAM_MEMORIA");
	config_memoria.tam_segmento_0 		 = config_get_int_value	  (config, "TAM_SEGMENTO_0");
	config_memoria.cant_segmentos 		 = config_get_int_value	  (config, "CANT_SEGMENTOS"); //max x proceso
	config_memoria.retardo_memoria 	 	 = config_get_int_value	  (config, "RETARDO_MEMORIA");
	config_memoria.retardo_compactacion  = config_get_int_value	  (config, "RETARDO_COMPACTACION");
	config_memoria.algoritmo_asignacion  = config_get_string_value(config, "ALGORITMO_ASIGNACION");

	// config_destroy(config);
}

int esperar_conexiones(int socket_servidor) {

//	conexiones[0] = malloc(sizeof(int));
//	*conexiones[0] = accept(socket_servidor, NULL, NULL); // esperar_cliente(socket_servidor);
//	pthread_create(&hilos[0], NULL, (void *) atender_File_System, conexiones[0]);
//	log_info(logger, "Se conectó el File System!");

//	pthread_t hilo_cpu;
//	pthread_t hilo_kernel;
//
//	int*socket_cpu = malloc(sizeof(int));
//	*socket_cpu = accept(socket_servidor, NULL, NULL);
//	pthread_create(&hilo_cpu, NULL, (void*) atender_CPU, socket_cpu);
//	log_info(logger, "Se conectó la CPU!");
//
//	int*socket_kernel = malloc(sizeof(int));
//	*socket_kernel = accept(socket_servidor, NULL, NULL);
//	pthread_create(&hilo_kernel, NULL, (void *) atender_kernel, socket_kernel);
//	log_info(logger, "Se conectó el Kernel!");
//
//	pthread_detach(hilo_kernel);
//	pthread_detach(hilo_cpu);

	//TODO DESPUÉS VEMOS BIEN ESTO
	int* socket_cliente = malloc(sizeof(int));//esperar_cliente(logger_kernel,"KERNEL", socket_servidor); // se conecta el cliente
		*socket_cliente = accept(socket_servidor, NULL, NULL);

	//if(socket_cliente != -1) {
	pthread_t hilo_cliente;
	//manejar_conexion(socket_cliente);
	pthread_create(&hilo_cliente, NULL, (void*) manejar_conexion, socket_cliente); // creo el hilo con la funcion manejar conexion a la que le paso el socket del cliente y sigo en la otra funcion
	pthread_detach(hilo_cliente);

	return 1;

}

void manejar_conexion(int* fd_cliente) {
	int socket_cliente = *fd_cliente;
	pthread_mutex_t mutex_atendiendo_fs = PTHREAD_MUTEX_INITIALIZER;

	while(1) {
		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));
		recv(socket_cliente, &(paquete->codigo_operacion), sizeof(op_code), MSG_WAITALL);
		recv(socket_cliente, &(paquete->buffer->stream_size), sizeof(int), 0);
		paquete->buffer->stream = malloc(paquete->buffer->stream_size);
		recv(socket_cliente, paquete->buffer->stream, paquete->buffer->stream_size, 0);
		int tamanio_valor;
		int direccion_fisica;
		char* valor;
		void* stream = paquete->buffer->stream;

		switch(paquete->codigo_operacion) {
		case LEER_DE_MEMORIA :

			memcpy(&direccion_fisica, stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&tamanio_valor, stream, sizeof(int));
			stream += sizeof(int);

			log_info(logger, "CPU quiere leer en la dirección <%d> un tamaño de <%d>", direccion_fisica, tamanio_valor);


			recv_leer_de_memoria(tamanio_valor, direccion_fisica, socket_cliente);

			//Esto de acá es simplemente para probarlo
			char* valor = malloc(tamanio_valor);
			valor = "HOLA";

			enviar_string_por(LEIDO, valor, socket_cliente);

			break;
		case ESCRIBIR_EN_MEMORIA :
			memcpy(&direccion_fisica, stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&tamanio_valor, stream, sizeof(int));
			stream += sizeof(int);

			char* valor_a_escribir = malloc(tamanio_valor);
			memset(valor_a_escribir, 0, tamanio_valor);

			memcpy(valor_a_escribir, stream, tamanio_valor);
			stream += tamanio_valor;

			log_info(logger, "CPU quiere escribir en la dirección <%d> el valor <%s>, de tamaño <%d>", direccion_fisica, valor_a_escribir, tamanio_valor);

			recv_escribir_en_memoria(direccion_fisica, tamanio_valor, valor_a_escribir, socket_cliente);
			//TODO acá van las operaciones que hacen para escribir en memoria.

			break;

		case CREAR_ESTRUCTURAS:

			int pid_a_inicializar;

			memcpy(&pid_a_inicializar, stream, sizeof(int));
			stream += sizeof(int);

			recv_nuevo_proceso(pid_a_inicializar, socket_cliente); //Modifiqué esta función para que ande bien

			break;

		case CREATE_SEGMENT:
			recv_crear_segmento(socket_cliente, stream);
			break;

		case DELETE_SEGMENT:
			recv_eliminar_segmento(socket_cliente, stream);
			break;

		case COMPACTAR:
			log_info(logger, "Inicia compactación");

//			int offset = 0;
//			int algo = deserializar_int(stream, &offset); // no se usa, creo que igual no hace falta
			pthread_mutex_lock(&mutex_atendiendo_fs);
			compactar(socket_cliente);
			pthread_mutex_unlock(&mutex_atendiendo_fs);
			break;


		case LIBERAR_ESTRUCTURAS:
			recv_liberar_estructuras(socket_cliente, stream);
			break;

		case ESCRIBIR_EN_MEMORIA_FS:
			pthread_mutex_lock(&mutex_atendiendo_fs);
			memcpy(&direccion_fisica, stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&tamanio_valor, stream, sizeof(int));
			stream += sizeof(int);

			valor = malloc(tamanio_valor);
			memset(valor_a_escribir, 0, tamanio_valor);

			memcpy(valor, stream, tamanio_valor);
			stream += tamanio_valor;

			log_info(logger, "File System quiere escribir en la dirección <%d> el valor <%s>, de tamaño <%d>", direccion_fisica, valor_a_escribir, tamanio_valor);

			recv_escribir_en_memoria(direccion_fisica, tamanio_valor, valor_a_escribir, socket_cliente);

			pthread_mutex_unlock(&mutex_atendiendo_fs);
			break;

			case LEER_DE_MEMORIA_FS:
			pthread_mutex_lock(&mutex_atendiendo_fs);

			memcpy(&direccion_fisica, stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&tamanio_valor, stream, sizeof(int));
			stream += sizeof(int);

			log_info(logger, "File System quiere leer en la dirección <%d> un tamaño de <%d>", direccion_fisica, tamanio_valor);

			recv_leer_de_memoria(tamanio_valor, direccion_fisica, socket_cliente);
			pthread_mutex_unlock(&mutex_atendiendo_fs);
			break;

		default:
			log_error(logger, "Operación desconocida.");
			abort();
		}
		eliminar_paquete(paquete);
	}
}

void terminar_memoria(void) {

	config_destroy(config);
	log_destroy(logger);
	free(conexiones[0]);//No iria mas si hacemos lo que juani dijo del case
	list_destroy_and_destroy_elements(tablas_segmentos, (void *) destruir_tabla);
	free(segmento_0);
	destruir_bitmap();
	free(memoria);
}

void destruir_bitmap(void) {
	free(bitmap->bitarray);
	bitarray_destroy(bitmap);
}
