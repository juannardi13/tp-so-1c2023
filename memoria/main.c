#include "./include/utils.h"
#include <commons/log.h>
#include <commons/config.h>


int main() {
	char* ip_memoria;
	char* puerto_memoria;

	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config();

	ip_memoria = config_get_string_value(config, "IP");
	puerto_memoria = config_get_string_value(config, "PUERTO_ESCUCHA");

	int fd_memoria = iniciar_servidor(logger, "MEMORIA", ip_memoria, puerto_memoria);
	log_info(logger, "MEMORIA inicializada, esperando a recibir al CPU en el PUERTO %s.", puerto_memoria);
	int fd_consola = esperar_cliente(logger, "MEMORIA", fd_memoria);

	t_list* lista;
			while (1) {
				int cod_op = recibir_operacion(logger, fd_consola);
				switch (cod_op) {
				case MENSAJE:
					recibir_mensaje(logger, fd_consola);
					break;
				case PAQUETE:
					//TODO
					// no puede leer el paquete pero CREO que lo recibe
					lista = recibir_paquete(fd_consola);
					log_info(logger, "Me llegaron los siguientes valores:\n");
					list_iterate(lista, (void*) iterator);
					break;
				case -1:
					log_error(logger, "el cliente se desconecto. Terminando servidor");
					return EXIT_FAILURE;
				default:
					log_warning(logger,"Operacion desconocida. No quieras meter la pata");
					break;
				}
			}

	free(logger);
	// esperar conexiones de kernel
		
	log_info(logger, "MEMORIA inicializada, esperando a recibir al Kernel en el PUERTO %s.", puerto_memoria);
	fd_kernel = esperar_cliente(logger, "MEMORIA", fd_memoria);

	free(logger);
	// esperar conexiones de file system
	log_info(logger, "MEMORIA inicializada, esperando a recibir al File System en el PUERTO %s.", puerto_memoria);
	fd_file_system = esperar_cliente(logger, "MEMORIA", fd_memoria);
		

    return 0;
}

t_log* iniciar_logger(void) {
	t_log* nuevo_logger;

			nuevo_logger = log_create("./memoria.log", "memoria.log", 1, LOG_LEVEL_INFO);

			if(nuevo_logger == NULL) {
				printf("No se pudo crear al logger.\n");
				exit(1);
			}

		return nuevo_logger;
}

t_config* iniciar_config(void) {
	t_config* config;

	if((config = config_create("./cfg/memoria.config")) == NULL) {
		printf("No se pudo crear el config. \n");
		exit(2);
	}

	return config;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
