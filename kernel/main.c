#include "./include/utils.h"
#include "../shared/include/main.h"
#include <commons/log.h>
#include <commons/config.h>
#include <pthread.h>
/*
void* recibir_instrucciones();
void* enviar_instrucciones();

pthread_create(&hilo1, NULL, &recibir_instrucciones, NULL);
pthread_create(&hilo3, NULL, &enviar_instrucciones, NULL);
*/
int main() {
	char* ip_kernel;
	char* puerto_kernel;

	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config();

	ip_kernel = config_get_string_value(config, "IP");
	puerto_kernel = config_get_string_value(config, "PUERTO_CONSOLA");

	int fd_kernel = iniciar_servidor(logger, "KERNEL", ip_kernel, puerto_kernel);
	log_info(logger, "Kernel inicializado, esperando a recibir a la consola en el PUERTO %s.", puerto_kernel);
	int fd_consola = esperar_cliente(logger, "KERNEL", fd_kernel);

	t_list* lista;
			while (1) {
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
					break;
				default:
					log_warning(logger,"Operacion desconocida. No quieras meter la pata");
					break;
				}
				break;
			}

	printf("HOLAAA\n\n");

	int fd_cpu = 0, fd_memoria = 0, fd_file_system = 0;
		    if (!generar_conexiones(config, logger, &fd_file_system, &fd_memoria, &fd_cpu)) {

		        return EXIT_FAILURE;
	}

    return 0;
}
/*
typedef struct t_PCB{
	int PID;
	char* instrucciones; // lo dejamos en char*? o en forma de array
	int PC;
	char* registros_cpu; // revisar porque dice estructura
	int segmentos[][3];
	float est_prox_rafaga; //iniciaizar con archivo configuracion
	float timestamp;
	int archivos_abiertos[][2];
	
}*/

t_log* iniciar_logger(void) {
	t_log* nuevo_logger;

			nuevo_logger = log_create("./kernel.log", "kernel.log", 1, LOG_LEVEL_INFO);

			if(nuevo_logger == NULL) {
				printf("No se pudo crear al logger.\n");
				exit(1);
			}

		return nuevo_logger;
}

t_config* iniciar_config(void) {
	t_config* config;

	if((config = config_create("./cfg/kernel.config")) == NULL) {
		printf("No se pudo crear el config. \n");
		exit(2);
	}

	return config;
}

void iterator(char* value) {
	log_info(logger,"%s", value);
}



