#include"../include/utils.h"


t_log* logger;
t_config* config;

t_log* iniciar_logger(void) {
	t_log* nuevo_logger;

			nuevo_logger = log_create("./kernel.log", "kernel.log", 1, LOG_LEVEL_INFO);

			if(nuevo_logger == NULL) {
				printf("No se pudo crear al logger.\n");
				exit(1);
			}

		return nuevo_logger;
}

t_config* iniciar_config(char* ruta)
{
	t_config* config;

	if((config = config_create(ruta)) == NULL) {
		printf("No se pudo crear el config. \n");
		exit(2);
	}

	return config;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}

/*
bool generar_conexiones(t_config* config, t_log* logger, int* fd_file_system, int* fd_memoria, int* fd_cpu) {
    char* puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
    char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    char* puerto_file_system = config_get_string_value(config, "PUERTO_FILE_SYSTEM");
    char* ip_cpu = config_get_string_value(config, "IP");
    char* ip_memoria = config_get_string_value(config, "IP");
    char* ip_file_system = config_get_string_value(config, "IP");

    *fd_cpu = crear_conexion(
            logger,
            "CPU",
            ip_cpu,
            puerto_cpu
     );


    *fd_memoria = crear_conexion(
        logger,
        "MEMORIA",
        ip_memoria,
        puerto_memoria
    );

    *fd_file_system = crear_conexion(
        logger,
        "FILE_SYSTEM",
        ip_file_system,
        puerto_file_system
    );

    return *fd_memoria != 0 && *fd_cpu != 0 && *fd_file_system != 0;
}*/





