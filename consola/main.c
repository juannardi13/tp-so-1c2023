#include "./include/main.h"
#include "./include/utils.h"

t_log* iniciar_logger(void);

int main() {
	char* ip_kernel;
	char* puerto_kernel;

    t_log* logger = iniciar_logger();
    t_config* config = iniciar_config();

    log_info(logger, "Hola! Se inicializo el modulo cliente Consola.");

    ip_kernel = config_get_string_value(config, "IP");
    puerto_kernel = config_get_string_value(config, "PUERTO");

    log_info(logger, "La configuracion de la conexion indica el PUERTO %s y la IP %s", ip_kernel, puerto_kernel);

    int conexion_kernel = crear_conexion(logger,"KERNEL" ,ip_kernel, puerto_kernel);

    log_info(logger, "Conexion exitosa con el Kernel.");
    enviar_mensaje(puerto_kernel, conexion_kernel);

    return EXIT_SUCCESS;
}

t_log* iniciar_logger(void){

	t_log* nuevo_logger;

		nuevo_logger = log_create("./consola.log", "consola.log", 1, LOG_LEVEL_INFO);

		if(nuevo_logger == NULL) {
			printf("No se pudo crear al logger.\n");
			exit(1);
		}

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* config;

	if((config = config_create("./cfg/consola.config")) == NULL) {
		printf("No se pudo crear el config. \n");
		exit(2);
	}

	return config;
}

