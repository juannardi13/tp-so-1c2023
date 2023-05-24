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

typedef struct {
    t_log* log;
    int fd;
    char* server_name;
} t_procesar_conexion_args;

static void procesar_conexion(void* void_args) {
    t_procesar_conexion_args* args = (t_procesar_conexion_args*) void_args;
    t_log* logger = args->log;
    int cliente_socket = args->fd;
    char* server_name = args->server_name;
    free(args);

    //Creo pcb y pongo el proceso en la cola de NEW
    int i = 0;
    char** instrucciones = malloc(sizeof(char*) * 5);

    op_code cop;
    while (cliente_socket != -1) {

        if (recv(cliente_socket, &cop, sizeof(op_code), 0) != sizeof(op_code)) {
            log_info(logger, "Se terminaron de leer las instrucciones recibidas");
            break;
        }

        switch (cop) {
            case MENSAJE:
            	instrucciones[i] = recibir_mensaje(logger, cliente_socket);
            	i++;
            // Errores
         /*   case -1:
                log_error(logger, "Cliente desconectado de %s...", server_name);
                return;*/
          /*  default:
                log_error(logger, "Algo anduvo mal en el server de %s", server_name);
                log_info(logger, "Cop: %d", cop);
                return;*/
        }
    }
log_error(logger, "Antes de armar el PCB");
    t_pcb* pcb = malloc(sizeof(t_pcb*));
    pcb = armar_pcb(instrucciones, logger);
    log_info(logger, "%d", pcb->PID);
    log_info(logger, "%d", pcb->PC);

    log_error(logger, "Después de armar el PCB");

    queue_push(cola_new, pcb);
    log_error(logger, "Después de agregar el PCB a cola NEW");




    log_warning(logger, "El cliente se desconecto de %s server", server_name);
    return;
}

int server_escuchar(t_log* logger, char* server_name, int server_socket) {
    int cliente_socket = esperar_cliente(logger, server_name, server_socket);

    if (cliente_socket != -1) {
        pthread_t hilo;
        t_procesar_conexion_args* args = malloc(sizeof(t_procesar_conexion_args));
        args->log = logger;
        args->fd = cliente_socket;
        args->server_name = server_name;
        pthread_create(&hilo, NULL, (void*) procesar_conexion, (void*) args);
        pthread_detach(hilo);
        return 1;
    }
    return 0;
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


// Serializar y enviar paquete - revisar
typedef struct {
uint32_t size;
void* stream;
} t_buffer;

typedef {
uint8_t codigo_operacion
t_buffer* buffer;
} t_paquete;


t_paquete* serializar_instrucciones(char** lista_instrucciones){
t_paquete* paquete = malloc(sizeof(paquete));
paquete->buffer = malloc(sizeof(t_buffer));

int tamanio_lista = string_length(lista_instrucciones);
int offset = 0;

int tamanio_buffer = tamanio_lista + sizeof(int) + 1;
paquete->buffer->size = tamanio_buffer;
void* stream = malloc(tamanio_buffer);

memcpy(stream + offset, &tamanio_lista, sizeof(int));
offset += sizeof(int);
memcpy(stream + offset, lista_instrucciones, tamanio_lista + 1);

paquete->buffer->stream = stream;

return paquete;
}

char** deserializar_instrucciones(t_buffer* buffer){
char** lista_instrucciones;
void* stream = buffer->stream;

int tamanio_lista;

memcpy(&tamanio_lista, stream, sizeof(int));
stream += sizeof(int);
lista_instrucciones = malloc(tamanio_lista);
memcpy(lista_instrucciones, stream, tamanio_lista);
return lista_instrucciones;
}

void enviar_instrucciones(char** lista_instrucciones, int fd_cpu){
t_paquete* paquete_a_enviar = serializar_paquete(lista_instrucciones);
enviar_paquete(paquete, fd_cpu);
free(paquete);
eliminar_paquete(paquete);
}
