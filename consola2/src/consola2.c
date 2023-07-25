#include "consola2.h"
#include "utils_consola2.h"
#include <shared-2.h>


int main(int argc, char** argv) {
	
    char* ip_kernel;
    char* puerto_kernel;

    t_log* logger = iniciar_logger();

    if(argc != 3){
    	log_error(logger, "No se ha inicializado correctamente el módulo consola");
    	exit(1);
    }

    log_info(logger, "Hola! Se inicializo el modulo cliente Consola.");

    //Lectura del archivo Config

    t_config* config = iniciar_config(argv[2]);

    ip_kernel = config_get_string_value(config, "IP");
	puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");
	
	log_info(logger, "Se ha leido el archivo de config con exito, con los valores PUERTO: %s e IP: %s.", puerto_kernel, ip_kernel);

    //Creación de la conexión con Kernel, también lectura del archivo de pseudocódigo

	char* instrucciones = leer_archivo_pseudocodigo(argv[1], logger);

    int conexion_kernel = crear_conexion(logger, "CONSOLA", ip_kernel, puerto_kernel);
	log_info(logger, "Se creo la conexion con el Kernel");

	//Envío de instrucciones a Kernel

	getchar();
	enviar_string(conexion_kernel, instrucciones, CONSOLA);
	
	log_info(logger, "Paquete de instrucciones enviado correctamente!\n");

	//TODO Finalización de consola acá abajo... Comento lo que quizás sirva para terminar la consola, cuando implemente la instruccion EXIT correctamente en Kernel

	log_info(logger, "Esperando respuesta de Kernel para finalizar.");

	op_code respuesta_kernel;
	recv(conexion_kernel, &respuesta_kernel, sizeof(op_code), MSG_WAITALL);

	if(respuesta_kernel != FINALIZAR_CONSOLA)
		log_error(logger, "Error al querer finalizar la consola");

	log_info(logger, "Finalizó el proceso, terminando consola");
//	finalizar_programa(conexion_kernel, logger, config);

	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion_kernel);
	free(instrucciones);

	return 0;
}

