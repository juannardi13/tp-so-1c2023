#include "./include/utils.h"
#include <commons/log.h>
#include <commons/config.h>


int main() {
	char* ip_kernel;
	char* puerto_kernel;

	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config();

	ip_kernel = config_get_string_value(config, "IP");
	puerto_kernel = config_get_string_value(config, "PUERTO");

	int fd_kernel = iniciar_servidor(logger, "KERNEL", ip_kernel, puerto_kernel);
	log_info(logger, "Kernel inicializado, esperando a recibir a la consola en el PUERTO %s.", puerto_kernel);
	int fd_consola = esperar_cliente(logger, "KERNEL", fd_kernel);

	t_list* lista;
		while (1) {
			int cod_op = recibir_operacion(logger, fd_consola);
			switch (cod_op) {
			case MENSAJE:
				recibir_mensaje(logger, fd_consola);
				break;
			case -1:
				log_error(logger, "el cliente se desconecto. Terminando servidor");
				return EXIT_FAILURE;
			default:
				log_warning(logger,"Operacion desconocida. No quieras meter la pata");
				break;
			}
		}


    return 0;
}

t_log* iniciar_logger(void) {
	t_log* nuevo_logger;

			nuevo_logger = log_create("./kernel.log", "kernel.log", 1, LOG_LEVEL_INFO);

			if(nuevo_logger == NULL) {
				printf("No se pudo crear al logger.\n");
				exit(1);
			}

		return nuevo_logger;
}

t_config* iniciar_config(void) {
	t_config* config;

	if((config = config_create("./cfg/kernel.config")) == NULL) {
		printf("No se pudo crear el config. \n");
		exit(2);
	}

	return config;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}
