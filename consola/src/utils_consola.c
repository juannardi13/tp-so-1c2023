#include "../include/utils_consola.h"
#include "../../shared/include/main_shared.h"


t_log* iniciar_logger(void){

	t_log* nuevo_logger;

		nuevo_logger = log_create("./consola.log", "consola.log", 1, LOG_LEVEL_INFO);

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

FILE* abrir_archivo_instrucciones(char * path, t_log* logger){

	FILE *file_instrucciones = fopen(path,"r");

	if(file_instrucciones == NULL){
		log_error(logger, "Error al abrir el archivo de instrucciones. Terminando ejecucion.");
		exit(4);
	} else {
		log_debug(logger, "Se ha abierto el archivo de instrucciones con exito.");
	}

	return file_instrucciones;
}

void levantar_instrucciones(FILE* archivo, t_log* logger, int conexion) {
	char line[100];
	int i = 1;

	log_warning(logger, "Llegaron las siguientes instrucciones de pseudocodigo del archivo:\n");

	while(fgets(line, 100, archivo) != NULL){

		line[strcspn(line, "\n")] = '\0';

		printf("Instruccion numero %d: %s\n", i, line);
		enviar_mensaje(line, conexion);

		i++;

	}

}

void cargar_valores_config(t_log* logger, char* ruta, char* ip, char* puerto) {
	t_config* config = iniciar_config(ruta);

	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	config_destroy(config);
}

