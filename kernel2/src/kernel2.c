#include <commons/log.h>
#include <commons/config.h>
#include "utils_kernel2.h"

archivo_config config_kernel;
t_log* logger_kernel;

void sighandler(int signal) {
	finalizar_kernel();
	exit(0);
}

int main() {
	logger_kernel = log_create("./kernel.log", "kernel.log", 1, LOG_LEVEL_INFO);
	cargar_valores_config("cfg/kernel.config");

	iniciar_planificador_largo_plazo(); //Esto despues tiene que ir en main_kernel.c
	iniciar_planificador_corto_plazo();
	iniciar_registros();

	//Iniciarse como cliente de los siguientes servidores: (COMENTAR PARA PROBAR EL KERNEL, SI NO TENÉS QUE INICIAR TODO)
	//socket_filesystem = crear_conexion(logger, "KERNEL", config_kernel.ip_filesystem, config_kernel.puerto_filesystem);
	//socket_memoria = crear_conexion(logger, "KERNEL", config_kernel.ip_memoria, config_kernel.puerto_memoria);
	//socket_cpu = crear_conexion(logger, "KERNEL", config_kernel.ip_cpu, config_kernel.puerto_cpu);

	//iniciar_conexion_cpu(ip_cpu, puerto_kernel, logger);

	int fd_kernel = iniciar_servidor(config_kernel.puerto_escucha);

	log_info(logger_kernel, "Kernel inicializado, esperando a recibir a la consola en el PUERTO %s.", config_kernel.puerto_escucha);

	while(atender_clientes_kernel(fd_kernel));

	log_error(logger_kernel, "Cerrando el módulo Kernel");

    return 0;
}

void cargar_valores_config(char* ruta_config) {
	t_config* config = config_create(ruta_config);

	if(config == NULL) {
		perror("Archivo config no encontrado");
		abort();
	}

	tipo_algoritmo algoritmo_auxiliar;
	char* algoritmo_literal;
	algoritmo_literal = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	if(strcmp(algoritmo_literal, "HRRN") == 0) {
		algoritmo_auxiliar = HRRN;
	} else if(strcmp(algoritmo_literal, "FIFO") == 0) {
		algoritmo_auxiliar = FIFO;
	} else {
		perror("Algoritmo de planificación en CONFIG no válido");
		abort();
	}
	free(algoritmo_literal);

	config_kernel.ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	config_kernel.puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	config_kernel.ip_filesystem = config_get_string_value(config, "IP_FILESYSTEM");
	config_kernel.puerto_filesystem = config_get_string_value(config, "PUERTO_FILESYSTEM");
	config_kernel.ip_cpu = config_get_string_value(config, "IP_CPU");
	config_kernel.puerto_cpu = config_get_string_value(config, "PUERTO_CPU");
	config_kernel.puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	config_kernel.algoritmo_planificacion = algoritmo_auxiliar;
	config_kernel.estimacion_inicial = config_get_int_value(config, "ESTIMACION_INICIAL");
	config_kernel.alfa_hrrn = config_get_double_value(config, "HRRN_ALFA");
	config_kernel.grado_multiprogramacion = config_get_int_value(config, "GRADO_MAX_MULTIPROGRAMACION");
	//TODO ver que hacer con el tema de los recursos

}
