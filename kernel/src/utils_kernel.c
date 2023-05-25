#include"../include/utils_kernel.h"
#include"../../shared/include/main_shared.h"
#include"../../shared/src/main_shared.c"

t_log* logger;
t_config* config;
t_queue* cola_new;
int pid_global = 1000;

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
        "FILE_SYSTEM",
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

	char* codigo = mensaje_recibido;

	if(strcmp(mensaje_recibido, "YIELD") == 0) {
		printf("ENTRE A YIELD\n\n");
		instruccion.nombre = YIELD;
	}

	if(strcmp(mensaje_recibido, "EXIT") == 0) {
		instruccion.nombre = EXIT;
	}

	return instruccion;
}

void agregar_instruccion_a_lista(char ** lista, char* instruccion){

    instruccion[strcspn(instruccion, "\n")] = '\0'; //saco si hay el \n

    //si todavia no se agrego ninguna instruccion
    if (*lista == NULL){
        if ((*lista = malloc(strlen(instruccion) + 1)) == NULL){
            log_error(logger, "Error al hacer malloc en agregar_instruccion_a_lista()");
        }
        strcpy(*lista, instruccion);
    }
    else{
        if ((*lista = realloc(*lista, strlen(*lista) + strlen(instruccion) + 2)) == NULL){
            log_error(logger, "Error al hacer realloc en agregar_instruccion_a_lista()");
        }

        //concateno la instruccion nueva con todas las instrucciones anteriores
        strcat(*lista, "\n");
        strcat(*lista, instruccion);
    }
}



t_pcb* armar_pcb(char** lista_instrucciones, t_log* logger){
    /*
        Gestiona la generacion de un nuevo pcb
    */

    t_pcb * pcb_pointer = NULL;

    t_pcb pcb_nuevo = {
    pid_global,
    lista_instrucciones,
    0
    };

    if ((pcb_pointer = alocar_memoria_pcb(logger)) == NULL){
        log_error(logger, "Error al generar_pcb()");
        return NULL;
    }

    *pcb_pointer = pcb_nuevo;

    pid_global++;


    return pcb_pointer;
}

t_pcb * alocar_memoria_pcb(t_log* logger){

    t_pcb * pcb_pointer = NULL;

    if((pcb_pointer = malloc(sizeof *pcb_pointer)) == NULL){
        log_error(logger, "Fallo al hacer malloc en 'alocar_memoria_todos_pcb()'");
        return NULL;
    }

    return pcb_pointer;
}

