#include "./include/utils.h"
#include "../shared/include/main.h"
#include "../shared/src/main.c"
#include <commons/log.h>
#include <commons/config.h>

int main() {
	char* ip_kernel;
	char* puerto_kernel;

	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config("cfg/kernel.config");

	ip_kernel = config_get_string_value(config, "IP");
	puerto_kernel = config_get_string_value(config, "PUERTO_CONSOLA");

	int fd_kernel = iniciar_servidor(logger, "KERNEL", ip_kernel, puerto_kernel);
	log_info(logger, "Kernel inicializado, esperando a recibir a la consola en el PUERTO %s.", puerto_kernel);
	int fd_consola = esperar_cliente(logger, "KERNEL", fd_kernel);

	int j = 0;
	t_list* lista;
			while (j == 0) {
				int cod_op = recibir_operacion(logger, fd_consola);
				switch (cod_op) {
				case MENSAJE:
					recibir_mensaje(logger, fd_consola);
					break;
				case PAQUETE:
					//TODO
					// no puede leer el paquete pero CREO que lo recibe
					lista = recibir_paquete(fd_consola);
					log_info(logger, "Me llegaron los siguientes valores:\n");
					list_iterate(lista, (void*) iterator);
					break;
				case -1:
					log_error(logger, "el cliente se desconecto. Terminando servidor");
					j = 1;
					continue;
				default:
					log_warning(logger,"Operacion desconocida. No quieras meter la pata");
					break;
				}
			}

	int fd_file_system = 0, fd_cpu = 0, fd_memoria = 0;
	if (!generar_conexiones(config, logger, &fd_file_system, &fd_memoria, &fd_cpu)) {
		log_error(logger, "No se pudieron generar las conexiones a Memoria, Cpu y File System");
	    return EXIT_FAILURE;
    }

	enviar_mensaje("Soy el Kernel y me conecté al File System", fd_file_system);
	enviar_mensaje("Soy el Kernel y me conecté al CPU", fd_cpu);
	enviar_mensaje("Soy el Kernel y me conecté a la Memoria", fd_memoria);

	log_error(logger, "Cerrando el módulo Kernel");
    return 0;
}
/*
typedef struct t_pcb{
	int PID;
	char** instrucciones; // lo dejamos en char*?
	int PC;
	t_registros registros_cpu;
	int segmentos[][3];
	float est_prox_rafaga; //iniciaizar con archivo configuracion
	float timestamp;
	int archivos_abiertos[][2];
	
}*/

