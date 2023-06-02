#include "utils-fileSystem.h"

t_log* iniciar_logger(void) {
	t_log* nuevo_logger = log_create("fileSystem.log", "File System", 1, LOG_LEVEL_INFO);

	if (nuevo_logger == NULL) {
		printf("No se pudo crear el log\n");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(char* ruta) {
	t_config* nuevo_config = config_create(ruta);

	if (nuevo_config == NULL) {
		printf("No se pudo leer la config\n");
		exit(2);
	}

	return nuevo_config;
}

