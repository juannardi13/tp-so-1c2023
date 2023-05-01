
#include "./include/utils.h"
#define MAX_LEN 256

int main() {
	char* ip_memoria;
	char* puerto_memoria;

    t_log* logger = iniciar_logger();
    t_config* config = iniciar_config();

    log_info(logger, "Hola! Se inicializo el modulo cliente CPU.");

    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");

    log_info(logger, "La configuracion de la conexion indica el PUERTO %s y la IP %s", ip_memoria, puerto_memoria);

    int conexion_memoria = crear_conexion(logger,"MEMORIA" ,ip_memoria, puerto_memoria);

    log_info(logger, "Conexion exitosa con la memoria.");
    enviar_mensaje(puerto_memoria, conexion_memoria);

	//Esperar conexiones de kernel

    int fd_cpu = iniciar_servidor(logger, "CPU", ip_memoria, puerto_memoria);
    log_info(logger, "CPU inicializada, esperando a recibir al kernel en el PUERTO %s.", puerto_memoria);
    int fd_kernel = esperar_cliente(logger, "CPU", fd_cpu);

    return EXIT_SUCCESS;
}

t_log* iniciar_logger(void){

	t_log* nuevo_logger;

		nuevo_logger = log_create("./cpu.log", "cpu.log", 1, LOG_LEVEL_INFO);

		if(nuevo_logger == NULL) {
			printf("No se pudo crear al logger.\n");
			exit(1);
		}

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* config;

	if((config = config_create("./cfg/cpu.config")) == NULL) {
		printf("No se pudo crear el config. \n");
		exit(2);
	}

	return config;
}

