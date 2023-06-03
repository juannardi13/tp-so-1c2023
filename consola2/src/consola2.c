#include "consola2.h"
#include "utils_consola2.h"
#include <shared-2.h>


int main(int argc, char** argv) {
	// Inicialización de Consola

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

    // Creacion del paquete

    int tamanio_proceso = atoi(argv[1]); //Calcula el peso del proceso para poder serializar y deserializar

    t_paquete* paquete_de_instrucciones = crear_paquete_consola();

    agregar_a_paquete(paquete_de_instrucciones, &tamanio_proceso,sizeof(int)); //Agrega al paquete el tamanio del proceso como primer dato

    t_list *instrucciones = parsear_instrucciones(argv[1], logger);

	serializar_instrucciones(instrucciones, paquete_de_instrucciones); //Esta función ya agrega al paquete las instrucciones.

	//Inicialización de Consola como cliente
	// int conexion_kernel = crear_conexion(logger, "CONSOLA", ip_kernel, puerto_kernel);
	int conexion_kernel = crear_conexion(logger, "CONSOLA", ip_kernel, puerto_kernel);
	log_info(logger, "Se creo la conexion con el Kernel");

	//Envío de instrucciones a Kernel
	getchar();
	log_warning(logger,"Cantidad de Instrucciones enviadas: %d", list_size(instrucciones));
	enviar_paquete(paquete_de_instrucciones, conexion_kernel);
	log_info(logger, "Paquete de instrucciones enviado correctamente!");

	//Finalización de consola abajo...


    return 0;
}

