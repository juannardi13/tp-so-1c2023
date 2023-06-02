#include "../include/utils_consola.h"
#include "../../shared/include/main_shared.h"
#include <commons/log.h>
#include <commons/config.h>


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

t_list* parsear_instrucciones(char* ruta_archivo_pseudocodigo, t_log* logger) {
	// Decidí parsear las instrucciones en la consola para pasarlas como paquete, así las manejamos más facilmente si viajan como paquete
	t_list* instrucciones = list_create();
	char* parametro1;
	char* parametro2;
	char* parametro3;

	char* pseudo_codigo_leido = leer_archivo_pseudocodigo(ruta_archivo_pseudocodigo, logger); //Lee el archivo de pseudocodigo y lo transforma en una cadena
	char** lista_instrucciones = string_split(pseudo_codigo_leido, "\n"); //Divide la cadena creada arriba en un array de Strings donde cada String es una función con sus parámetros
	int indice_split = 0;

	while(lista_instrucciones[indice_split] != NULL) {

		char** palabras = string_split(lista_instrucciones[indice_split], " "); //Acá se divide a cada instrucción en un array con todas las palabras que la componen: el nombre y sus parámetros.

		if(strcmp(palabras[0], "SET") == 0){
			parametro1 = palabras[1];
			parametro2 = palabras[2];
			list_add(instrucciones, armar_instruccion(SET, parametro1, parametro2, "0"));
		} else if(strcmp(palabras[0], "YIELD") == 0) {
			list_add(instrucciones, armar_instruccion(YIELD, "0", "0", "0"));
		} else if(string_equals_ignore_case(palabras[0], "EXIT")) {
			list_add(instrucciones, armar_instruccion(EXIT, "0", "0", "0"));
		} //Cada vez que quiera agregar instrucciones anido mas else if's

		indice_split++;

		free(palabras);
	}

	log_info(logger, "Las instrucciones fueron parseadas, listas para enviar a Kernel!");

	return instrucciones;
}

t_instruccion* armar_instruccion(nombre_instruccion id, char* parametro_1, char* parametro_2, char* parametro_3) {
	t_instruccion* instruccion= malloc(sizeof(t_instruccion));
	instruccion->nombre = id;
	instruccion->parametro_1 = parametro_1;
	instruccion->parametro_2 = parametro_2;
	instruccion->parametro_3 = parametro_3;

	return instruccion;
}

void serializar_instrucciones(t_list *instrucciones, t_paquete *paquete) {

	for(int i=0; i<list_size(instrucciones); i++) {
		t_instruccion* instruccion = list_get(instrucciones, i);
		agregar_a_paquete(paquete, &(instruccion->nombre), sizeof(nombre_instruccion));
		agregar_a_paquete(paquete, &(instruccion->parametro_1), sizeof(char*));
		agregar_a_paquete(paquete, &(instruccion->parametro_2), sizeof(char*));
		agregar_a_paquete(paquete, &(instruccion->parametro_3), sizeof(char*));
	}

}

char* leer_archivo_pseudocodigo(char *ruta, t_log* logger) {
	//Esta función lo que hace es leer el archivo de pseudocódigo y crear un String con todas las líneas del mismo
    FILE *archivo;
    archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        log_error(logger, "No se pudo abrir el archivo.");
    }

    fseek(archivo, 0, SEEK_END);
    long tamano = ftell(archivo);
    rewind(archivo);

    char *contenido = (char *)malloc(tamano + 1);
    if (contenido == NULL) {
        log_error(logger, "No se pudo asignar memoria.");
    }

    fread(contenido, tamano, 1, archivo);
    contenido[tamano] = '\0';

    fclose(archivo);

    // Convertir a un string
    char *cadena = strdup(contenido);
    if (cadena == NULL) {
        log_error(logger, "No se pudo asignar memoria para el string.");
    }

    // Imprimir el string
    printf("Contenido: \n%s\n", cadena);

    free(cadena);

    return contenido;
}
