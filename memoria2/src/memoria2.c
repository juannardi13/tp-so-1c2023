#include "memoria.h"

int main(void) {

	inicializar_config();

	logger = log_create("memoria.log", "MEMORIA", 1, LOG_LEVEL_INFO);

	int memoria_fd = iniciar_servidor(config_memoria->puerto_escucha);
	log_info(logger, "Servidor Memoria iniciado, esperando conexiones entrantes");

	esperar_conexiones(memoria_fd); // 1-FileSystem 2-CPU 3-Kernel

	iniciar_memoria(); // se reserva memoria y crea el bitmap para huecos libres
//	log_info(logger, "Se inicializaron las estructuras de Memoria");

	pthread_join(hilos[0], NULL); // Memoria termina cuando terminan los hilos

	// terminar_memoria()

	return EXIT_SUCCESS;
}

void inicializar_config(void) {

	t_config *config = config_create("memoria.config");

	if (config == NULL) {
		perror("No se encontró el path de la config");
		exit(1);
	}

	config_memoria = malloc(sizeof(t_config_memoria));

	config_memoria->puerto_escucha 		 = config_get_string_value(config, "PUERTO_ESCUCHA");
	config_memoria->tam_memoria 		 = config_get_int_value	  (config, "TAM_MEMORIA");
	config_memoria->tam_segmento_0 		 = config_get_int_value	  (config, "TAM_SEGMENTO_0");
	config_memoria->cant_segmentos 		 = config_get_int_value	  (config, "CANT_SEGMENTOS"); //max x proceso
	config_memoria->retardo_memoria 	 = config_get_int_value	  (config, "RETARDO_MEMORIA");
	config_memoria->retardo_compactacion = config_get_int_value	  (config, "RETARDO_COMPACTACION");
	config_memoria->algoritmo_asignacion = config_get_string_value(config, "ALGORITMO_ASIGNACION");

	config_destroy(config);
}

void esperar_conexiones(int socket_servidor) {

	conexiones[0] = malloc(sizeof(int));
	*conexiones[0] = esperar_cliente(logger, "Memoria", socket_servidor);
	pthread_create(&hilos[0], NULL, (void *) atender_File_System, conexiones[0]);
	log_info(logger, "Se conectó el File System!");

//	conexiones[1] = malloc(sizeof(int));
//	*conexiones[1] = esperar_cliente(socket_servidor);
//	pthread_create(&hilos[1], NULL, (void *) atender_File_System, (void *) *conexiones[1]);
//	log_info(logger, "Se conectó la CPU!");
//
//	conexiones[2] = malloc(sizeof(int));
//	*conexiones[2] = esperar_cliente(socket_servidor);
//	pthread_create(&hilos[2], NULL, (void *) atender_File_System, (void *) *conexiones[2]);
//	log_info(logger, "Se conectó el Kernel!");
}
