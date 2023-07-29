#include "utils_consola2.h"
#include <shared-2.h>

t_log* iniciar_logger(void) {

	t_log* nuevo_logger;

	nuevo_logger = log_create("./consola.log", "consola.log", 1, LOG_LEVEL_INFO);

	if(nuevo_logger == NULL) {
		printf("No se pudo crear al logger.\n");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(char* ruta) {

	t_config* config;

	if((config = config_create(ruta)) == NULL) {
		printf("No se pudo crear el config. \n");
		exit(2);
	}

	return config;
}

FILE* abrir_archivo_instrucciones(char* path, t_log* logger) {

	FILE* file_instrucciones = fopen(path, "r");

	if(file_instrucciones == NULL){
		log_error(logger, "Error al abrir el archivo de instrucciones. Terminando ejecucion.");
		exit(4);
	} else {
		log_debug(logger, "Se ha abierto el archivo de instrucciones con exito.");
	}

	return file_instrucciones;
}

void enviar_string(int conexion_servidor, char* string, op_code codigo_op) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));
	int tamanio_string = strlen(string) + 1;

	buffer->stream_size = tamanio_string;
	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, string, tamanio_string);
	offset += tamanio_string;

	buffer->stream = stream;

	paquete->codigo_operacion = codigo_op;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	offset = 0;

	agregar_a_stream(a_enviar, &offset, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &offset, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &offset, paquete->buffer->stream, paquete->buffer->stream_size);

	send(conexion_servidor, a_enviar, (paquete->buffer->stream_size) + sizeof(int) + sizeof(int), 0);
	free(a_enviar);
	eliminar_paquete(paquete);
}

int agregar_a_stream(void *stream, int* offset, void *src, int size) {
	memcpy(stream + *offset, src, size);
	*offset += size;
}

void* serializar_string(char* string, t_paquete* paquete, op_code codigo_op) {
	t_buffer* buffer = malloc(sizeof(t_buffer));
	int tamanio_string = strlen(string) + 1;

	buffer->stream_size = tamanio_string;
	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, string, tamanio_string);
	offset += tamanio_string;

	buffer->stream = stream;

	paquete->codigo_operacion = codigo_op;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	offset = 0;

	memcpy(a_enviar, &(paquete->codigo_operacion), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar, &(paquete->buffer->stream_size), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->stream_size);
	offset += paquete->buffer->stream_size;

	free(string);
	return a_enviar;
}

void levantar_instrucciones(FILE* archivo, t_log* logger, int conexion) {
	
	char line[100];
	int i = 1;

	log_warning(logger, "Llegaron las siguientes instrucciones de pseudocodigo del archivo:\n");

	while(fgets(line, 100, archivo) != NULL){

		line[strcspn(line, "\n")] = '\0';

		printf("Instruccion numero %d: %s\n", i, line);

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
	//char* parametro3; COMENTO PARA QUE NO JODA EL WARNING DE QUE NO LO USO, ESTA FUNCIÓN QUEDÓ OBSOLETA POR LA FORMA EN LA QUE LO PENSAMOS

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

void parsear_instrucciones_y_enviar(char* ruta_archivo_pseudocodigo, int socket_servidor, t_log* logger) {
	
	char* pseudocodigo_leido = leer_archivo_pseudocodigo(ruta_archivo_pseudocodigo, logger);

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream_size = strlen(pseudocodigo_leido) + 1;

	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, pseudocodigo_leido, strlen(pseudocodigo_leido) + 1);

	buffer->stream = stream;

	free(pseudocodigo_leido);

	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = CONSOLA;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(uint8_t) + sizeof(uint32_t));
	int desplazamiento = 0;

	memcpy(a_enviar + desplazamiento, &(paquete->codigo_operacion), sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);
	memcpy(a_enviar + desplazamiento, &(paquete->buffer->stream_size), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(a_enviar + desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(socket_servidor, a_enviar, buffer->stream_size + sizeof(uint8_t) + sizeof(uint32_t), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

t_instruccion* armar_instruccion(op_code id, char* parametro_1, char* parametro_2, char* parametro_3) {
	t_instruccion* instruccion= malloc(sizeof(t_instruccion));
	instruccion->nombre = id;
	instruccion->parametro_1 = parametro_1;
	instruccion->parametro_1_length = strlen(parametro_1) + 1;
	instruccion->parametro_2 = parametro_2;
	instruccion->parametro_2_length = strlen(parametro_2) + 1;
	instruccion->parametro_3 = parametro_3;
	instruccion->parametro_3_length = strlen(parametro_3) + 1;

	return instruccion;
}

void serializar_instrucciones(t_list *instrucciones, t_paquete *paquete) {
		
	for(int i=0; i<list_size(instrucciones); i++) {
		t_instruccion* instruccion = list_get(instrucciones, i);
	
		agregar_a_paquete(paquete, &(instruccion->nombre), sizeof(op_code));
		agregar_a_paquete(paquete, &(instruccion->parametro_1), instruccion->parametro_1_length);
		agregar_a_paquete(paquete, &(instruccion->parametro_2), instruccion->parametro_2_length);
		agregar_a_paquete(paquete, &(instruccion->parametro_3), instruccion->parametro_3_length);
	}
}

void serializar_y_enviar_instruccion(int conexion_kernel, t_list *instrucciones, t_paquete *paquete) {

	//Serializo una sola instrucción primero

	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_instruccion* instruccion_a_serializar = malloc(sizeof(t_instruccion*));
	instruccion_a_serializar = list_get(instrucciones, 0);

	printf("\nLa instruccion a serializar es la siguiente: %d, %s, %s, %s\n", instruccion_a_serializar->nombre, instruccion_a_serializar->parametro_1, instruccion_a_serializar->parametro_2, instruccion_a_serializar->parametro_3);

	buffer->stream_size = sizeof(op_code)
			  + strlen(instruccion_a_serializar->parametro_1) + 1
			  + strlen(instruccion_a_serializar->parametro_2) + 1
			  + strlen(instruccion_a_serializar->parametro_3) + 1;

	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, &instruccion_a_serializar->nombre, sizeof(op_code));
	offset += sizeof(op_code);
	memcpy(stream + offset, &instruccion_a_serializar->parametro_1_length, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, instruccion_a_serializar->parametro_1, strlen(instruccion_a_serializar->parametro_1) + 1);
	offset += (strlen(instruccion_a_serializar->parametro_1) + 1);
	memcpy(stream + offset, &instruccion_a_serializar->parametro_2_length, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, instruccion_a_serializar->parametro_2, strlen(instruccion_a_serializar->parametro_2) + 1);
	offset += (strlen(instruccion_a_serializar->parametro_2) + 1);
	memcpy(stream + offset, &instruccion_a_serializar->parametro_3_length, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, instruccion_a_serializar->parametro_3, strlen(instruccion_a_serializar->parametro_3) + 1);

	buffer->stream = stream;

	paquete->codigo_operacion = MENSAJE; // Para probar la serializacion
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(uint8_t) + sizeof(uint32_t));
	int desplazamiento = 0;

	memcpy(a_enviar + desplazamiento, &(paquete->codigo_operacion), sizeof(uint8_t));
	desplazamiento += sizeof(uint8_t);
	memcpy(a_enviar + desplazamiento, &(paquete->buffer->stream_size), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(a_enviar + desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(conexion_kernel, a_enviar, buffer->stream_size + sizeof(uint8_t) + sizeof(uint32_t), 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
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

void finalizar_programa(int conexion, t_log* logger, t_config* config) {
	log_destroy(logger);
	config_destroy(config);
	liberar_conexion(conexion);
}
