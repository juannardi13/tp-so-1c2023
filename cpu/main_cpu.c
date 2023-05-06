#include "../shared/include/main.h"
#include "./include/utils.h"
#include "../shared/src/main.c"
#define MAX_LEN 256

int main() {
	char* ip_kernel;
	char* puerto_kernel;

    t_log* logger = iniciar_logger();
    t_config* config = iniciar_config();

    log_info(logger, "Hola! Se inicializo el modulo cliente CPU.");

    ip_kernel = config_get_string_value(config, "IP");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

    log_info(logger, "La configuracion de la conexion indica el PUERTO %s y la IP %s", ip_kernel, puerto_kernel);

    int fd_cpu = iniciar_servidor(logger, "CPU", ip_kernel, puerto_kernel);
    log_info(logger, "CPU inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_kernel);
    int fd_kernel = esperar_cliente(logger, "CPU", fd_cpu);

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
