#include "./include/main.h"
#include "./include/utils.h"
#include "../shared/include/main.h"
#include "../shared/src/main.c"

#define MAX_LEN 256

int main(int argc, char** argv) {
	char* ip_kernel;
	char* puerto_kernel;
	FILE* archivo_pseudocodigo = NULL;

    t_log* logger = iniciar_logger();

    if(argc != 3){
    		log_error(logger, "Uso: consola path tamanio");
    		exit(1);
    }

    log_info(logger, "Hola! Se inicializo el modulo cliente Consola.");

    t_config* config = iniciar_config(argv[2]);

   	ip_kernel = config_get_string_value(config, "IP");
    puerto_kernel = config_get_string_value(config, "PUERTO");

   // cargar_valores_config(logger, argv[2], ip_kernel, puerto_kernel);

	log_info(logger, "Se ha leido el archivo de config con exito, con los valores PUERTO: %s e IP: %s.", puerto_kernel, ip_kernel);

	archivo_pseudocodigo = abrir_archivo_instrucciones(argv[1], logger);

    int conexion_kernel = crear_conexion(logger, "KERNEL", ip_kernel, puerto_kernel);

    log_info(logger, "Conexion exitosa con el Kernel.");

    // Es para que consola espere nuestra aprobación para mandar las instrucciones. Al ejecutarlo va a frenarse, después de eso apretar ENTER
    getchar();

    levantar_instrucciones(archivo_pseudocodigo, logger, conexion_kernel);

    return EXIT_SUCCESS;
}


