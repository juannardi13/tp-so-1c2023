#include "utils_memoria.h"
#include "../../shared-2/src/shared-2.h"


int main(void) {
	char* ip_kernel;
	char* puerto_kernel;

	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config();

	log_info(logger, "Se inicializa el módulo Memoria");

    ip_kernel = config_get_string_value(config, "IP");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

	log_info(logger, "IP: %s, PUERTO KERNEL: %s", ip_kernel, puerto_kernel);

	int fd_memoria = iniciar_servidor(logger, "MEMORIA", ip_kernel, puerto_kernel);
	log_info(logger, "Memoria inicializada, esperando a recibir al Kernel en el PUERTO %s.", puerto_kernel);
	int fd_kernel = esperar_cliente(logger, "MEMORIA", fd_memoria);

	while (1) {
		int cod_op = recibir_operacion(logger, fd_kernel);
		switch (cod_op) {
			case MENSAJE:
				recibir_mensaje(logger, fd_kernel);
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
