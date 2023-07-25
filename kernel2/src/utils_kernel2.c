#include"utils_kernel2.h"
#include<shared-2.h>

//----------------------------------------------FUNCIONES DE INICIALIZACIÓN-------------------------------------------------------

t_log* logger;
t_config* config;
t_registros registros_iniciados;

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

void iniciar_registros(void) {
	strcpy(registros_iniciados.ax, "0000");
	strcpy(registros_iniciados.bx, "0000");
	strcpy(registros_iniciados.cx, "0000");
	strcpy(registros_iniciados.dx, "0000");
	strcpy(registros_iniciados.eax, "00000000");
	strcpy(registros_iniciados.ebx, "00000000");
	strcpy(registros_iniciados.ecx, "00000000");
	strcpy(registros_iniciados.edx, "00000000");
	strcpy(registros_iniciados.rax, "0000000000000000");
	strcpy(registros_iniciados.rbx, "0000000000000000");
	strcpy(registros_iniciados.rcx, "0000000000000000");
	strcpy(registros_iniciados.rdx, "0000000000000000");
}

//--------------------------------------------------------------FUNCIONES DE TIEMPO-------------------------------------------------------------

int get_time(void) { //Esto te devuelve el tiempo en milisegundos
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    int milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
 
    return milliseconds;
}

//--------------------------------------------------------------FUNCIONES DE FINALIZACIÓN--------------------------------------------------------

void finalizar_kernel(void) {
	log_error(logger_kernel, "Finalizando módulo Kernel");
	log_destroy(logger_kernel);
	log_destroy(kernel_principal);
	destruir_semaforos();
	destruir_listas();
	liberar_conexion(socket_cpu);
	liberar_conexion(socket_memoria);
	liberar_conexion(socket_filesystem);
}

void destruir_semaforos(void) {
	pthread_mutex_destroy(&mutex_new);
	pthread_mutex_destroy(&mutex_ready);
	pthread_mutex_destroy(&mutex_block_io);
	pthread_mutex_destroy(&mutex_exec);
	pthread_mutex_destroy(&mutex_exit);
	pthread_mutex_destroy(&mutex_pid);
	pthread_mutex_destroy(&mutex_operacion_memoria);
	pthread_mutex_destroy(&mutex_tabla_global_archivos);
	sem_destroy(&sem_archivos);
	sem_destroy(&sem_admitir);
	sem_destroy(&sem_exec);
	sem_destroy(&sem_ready);
	sem_destroy(&sem_block_io);
	sem_destroy(&sem_exit);
	sem_destroy(&sem_grado_multiprogramacion);
	sem_destroy(&sem_recursos);
}

void destruir_listas(void) {
	list_destroy_and_destroy_elements(cola_new, free);
	list_destroy_and_destroy_elements(cola_ready, free);
	list_destroy_and_destroy_elements(cola_block_io, free);
	list_destroy_and_destroy_elements(cola_exit, free);
	list_destroy_and_destroy_elements(cola_exec, free);
}

//----------------------------------------FUNCIONES EN DESUSO--------------------------------------------------------------------

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
		"FILE SYSTEM",
        ip_file_system,
        puerto_file_system
    );

    return *fd_memoria != 0 && *fd_cpu != 0 && *fd_file_system != 0;
}

t_instruccion recibir_instruccion(t_log* logger, int conexion_consola) {
	int tamanio;
	t_instruccion instruccion;
	char* mensaje_recibido = recibir_buffer(&tamanio, conexion_consola);
	// solo recibe el codigo de la instrucción, no está pensada para que también reciba los parámetros, cosa del futuro jajajaj

	if(strcmp(mensaje_recibido, "YIELD") == 0) {
		printf("ENTRE A YIELD\n\n");
		instruccion.nombre = YIELD;
	}

	if(strcmp(mensaje_recibido, "EXIT") == 0) {
		instruccion.nombre = EXIT;
	}

	return instruccion;
}

char* int_to_string(int un_numero) {
	char* string = malloc(5);

	snprintf(string, 5, "%d", un_numero);

	return string;
}
