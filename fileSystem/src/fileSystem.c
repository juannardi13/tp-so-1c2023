#include "fileSystem.h"

int main(void) {
	// char* ip_kernel;
	char* puerto_escucha;

	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config();

	log_info(logger, "Se inicializa el módulo File System");

    // ip_kernel = config_get_string_value(config, "IP");
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");

	log_info(logger, "PUERTO ESCUCHA: %s", puerto_escucha);

	int fd_file_system = iniciar_servidor(puerto_escucha);
	log_info(logger, "File System inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_escucha);
	int fd_kernel = esperar_cliente(logger, "FILE SYSTEM", fd_file_system);

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
