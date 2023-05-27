#include "./include/main.h"
#include "./include/utils_consola.h"
#include "../shared/include/main_shared.h"
#include "../shared/src/main_shared.c"

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
    puerto_kernel = config_get_string_value(config, "PUERTO");

	log_info(logger, "Se ha leido el archivo de config con exito, con los valores PUERTO: %s e IP: %s.", puerto_kernel, ip_kernel);

	//Inicialización de Consola como cliente

    int conexion_kernel = crear_conexion(logger, "KERNEL", ip_kernel, puerto_kernel);

    // Es para que consola espere nuestra aprobación para mandar las instrucciones. Al ejecutarlo va a frenarse, después de eso apretar ENTER
    getchar();

    //Creacion del paquete

    int tamanio_proceso = atoi(argv[1]); //Calcula el peso del proceso para poder serializar y deserializar

    t_paquete* paquete_de_instrucciones = crear_paquete_como(PAQUETE_CONSOLA);

    agregar_a_paquete(paquete_instrucciones, &tamanio_proceso,sizeof(int)); //Agrega al paquete el tamanio del proceso como primer dato

    t_list *instrucciones = parsear_instrucciones(argv[1], logger);

	serializar_instrucciones(instrucciones, paquete_de_instrucciones); //Esta función ya agrega al paquete las instrucciones.

	//Envío de instrucciones a Kernel
	log_warning(logger,"Cantidad de Instrucciones enviadas: %d", list_size(instrucciones));
	enviar_paquete(paquete_de_instrucciones, conexion_kernel);
	log_info(logger, "Paquete de instrucciones enviado correctamente!");

	//Finalización de consola abajo...


    return 0;
}

