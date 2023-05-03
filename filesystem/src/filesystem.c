#include "filesystem.h"

int main(void) {
	char* ip_memoria;
	char* puerto_memoria;

	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config();

	log_info(logger, "Se inicializa el módulo File System");

    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");

	log_info(logger, "IP Memoria: %s, PUERTO Memoria: %s", ip_memoria, puerto_memoria);

	int conexion = crear_conexion(ip_memoria, puerto_memoria);
}

t_log* iniciar_logger(void) {
	t_log* nuevo_logger = log_create("fileSystem.log", "fileSystem.log", 1, LOG_LEVEL_INFO);

	if (nuevo_logger == NULL) {
		printf("No se pudo crear el log\n");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(void) {
	t_config* nuevo_config = config_create("./fileSystem.config");

	if (nuevo_config == NULL) {
		printf("No se pudo leer la config\n");
		exit(2);
	}

	return nuevo_config;
}
