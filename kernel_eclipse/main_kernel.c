#include "./include/utils_kernel.h"

#include <commons/log.h>
#include <commons/config.h>

int main() {
	char* ip_kernel;
	char* puerto_kernel;

	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config("cfg/kernel.config");

	ip_kernel = config_get_string_value(config, "IP");
	puerto_kernel = config_get_string_value(config, "PUERTO_CONSOLA");

	int fd_kernel = iniciar_servidor(ip_kernel, puerto_kernel);
	log_info(logger, "Kernel inicializado, esperando a recibir a la consola en el PUERTO %s.", puerto_kernel);

	while(atender_clientes_kernel(fd_kernel, logger));

/*
	int fd_file_system = 0, fd_cpu = 0, fd_memoria = 0;
	if (!generar_conexiones(config, logger, &fd_file_system, &fd_memoria, &fd_cpu)) {
		log_error(logger, "No se pudieron generar las conexiones a Memoria, Cpu y File System");
	    return EXIT_FAILURE;
    }

	enviar_mensaje("Soy el Kernel y me conecté al File System", fd_file_system);
	enviar_mensaje("Soy el Kernel y me conecté al CPU", fd_cpu);
	//enviar_pcb(pcb, fd_cpu);
	enviar_mensaje("Soy el Kernel y me conecté a la Memoria", fd_memoria);
*/
	log_error(logger, "Cerrando el módulo Kernel");
    return 0;
}



