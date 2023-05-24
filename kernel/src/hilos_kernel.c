#include"../include/utils_kernel.h"
#include"../../shared/include/main_shared.h"
#include"../../shared/src/main_shared.c"

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
