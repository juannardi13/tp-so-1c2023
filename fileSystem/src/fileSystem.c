#include "fileSystem.h"

int main(void) {
	// char* ip_kernel;
	char* puerto_escucha;

	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config("fileSystem.config");

	t_config* config_super_bloque = iniciar_config("fileSystem.superbloque");

	t_super_bloque super;

	super.block_count = config_get_int_value(config_super_bloque,"BLOCK_COUNT");
	super.block_size = config_get_int_value(config_super_bloque,"BLOCK_SIZE");


	log_info(logger, "El fileSystem posee una cantidad de %d bloques",super.block_count);
	log_info(logger, "Los bloques del fileSystem tiene un tamaño de %d bytes",super.block_size);

	log_info(logger, "Se inicializa el módulo File System");

    // ip_kernel = config_get_string_value(config, "IP");
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");

	log_info(logger, "PUERTO ESCUCHA: %s", puerto_escucha);

	int fd_file_system = iniciar_servidor(puerto_escucha);
	log_info(logger, "File System inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_escucha);
	int fd_kernel = esperar_cliente(logger, "FILE SYSTEM", fd_file_system);

	while (1) {
		int cod_op = recibir_operacion(fd_kernel);
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
