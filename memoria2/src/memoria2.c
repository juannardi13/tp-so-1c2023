#include "memoria.h"
#include <pthread.h>

int main(int argc, char **argv) {

	if (argc < 2) {
		printf("ALTO BOT AMIGO\n");
		return EXIT_FAILURE;
	}

	inicializar_config(argv[1]);

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

void inicializar_config(char *ruta_config) {

	config = config_create(ruta_config);

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

	//TODO DESPUÉS VEMOS BIEN ESTO
	int* socket_cliente = malloc(sizeof(int));//esperar_cliente(logger_kernel,"KERNEL", socket_servidor); // se conecta el cliente
		*socket_cliente = accept(socket_servidor, NULL, NULL);

	//if(socket_cliente != -1) {
	pthread_t hilo_cliente;
	//manejar_conexion(socket_cliente);
	pthread_create(&hilo_cliente, NULL, (void*) manejar_conexion, socket_cliente); // creo el hilo con la funcion manejar conexion a la que le paso el socket del cliente y sigo en la otra funcion
	pthread_detach(hilo_cliente);

	log_info(logger, "Se conectó un cliente!");

	return 1;

}

void manejar_conexion(int* fd_cliente) {
	int socket_cliente = *fd_cliente;

	while(1) {
		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));
		recv(socket_cliente, &(paquete->codigo_operacion), sizeof(op_code), MSG_WAITALL);
		recv(socket_cliente, &(paquete->buffer->stream_size), sizeof(int), 0);
		paquete->buffer->stream = malloc(paquete->buffer->stream_size);
		recv(socket_cliente, paquete->buffer->stream, paquete->buffer->stream_size, 0);
		int tamanio_valor;
		int direccion_fisica;
		void* stream = paquete->buffer->stream;

		printf("%i\n",paquete->codigo_operacion);

		switch(paquete->codigo_operacion) {
		case LEER_DE_MEMORIA :

			memcpy(&direccion_fisica, stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&tamanio_valor, stream, sizeof(int));
			stream += sizeof(int);

			log_info(logger, "CPU quiere leer en la dirección <%d> un tamaño de <%d>", direccion_fisica, tamanio_valor);

			recv_leer_de_memoria(tamanio_valor, direccion_fisica, socket_cliente);

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
			log_info(logger, "Solicitud de compactación");
			compactar(socket_cliente);
			break;


		case LIBERAR_ESTRUCTURAS:
			recv_liberar_estructuras(socket_cliente, stream);
			break;

		case ESCRIBIR_EN_MEMORIA_FS:
			memcpy(&direccion_fisica, stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&tamanio_valor, stream, sizeof(int));
			stream += sizeof(int);

			char* valor = malloc(tamanio_valor);
			memset(valor, 0, tamanio_valor);

			memcpy(valor, stream, tamanio_valor);
			stream += tamanio_valor;

			log_info(logger, "File System quiere escribir en la dirección <%d> el valor <%s>, de tamaño <%d>", direccion_fisica, valor, tamanio_valor);

			recv_escribir_en_memoria(direccion_fisica, tamanio_valor, valor, socket_cliente);
			break;

			case LEER_DE_MEMORIA_FS:

			memcpy(&direccion_fisica, stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&tamanio_valor, stream, sizeof(int));
			stream += sizeof(int);

			log_info(logger, "File System quiere leer en la dirección <%d> un tamaño de <%d>", direccion_fisica, tamanio_valor);

			recv_leer_de_memoria(tamanio_valor, direccion_fisica, socket_cliente);
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
