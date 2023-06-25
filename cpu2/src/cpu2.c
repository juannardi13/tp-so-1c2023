#include "utils_cpu2.h"

#define MAX_LEN 256

/*
//Semáforos

recibir_de_kernel = 1; //Semáforo Binario
devolver_pcb = 0; //Semáforo Binario
ejecutar_proceso = 0;

//Hilos

void* conexion_kernel(void* args){

		//Lo que viene es pseudocodigo
		generar_conexion_con(kernel);

	while(1) {
		wait(recibir_de_kernel);
		recibir_pcb(kernel);
		signal(ejecutar_proceso);
		wait(devolver_pcb);
		devolver_pcb(kernel);
	}
}

void* conexion_memoria(void* args){
	generar_conexion_con(memoria);
}

void* ejecutar_proceso(void* args){
	while(1) {
		wait(ejecutar_proceso);

	}
}*/
int main() {
	bool cpu_bloqueada = false;
	char* ip_kernel;
	char* puerto_kernel;
    t_log* logger = iniciar_logger();
    t_config* config = iniciar_config();
    //creo paquete para recibir info
    t_paquete* paquete = malloc(sizeof(t_paquete));

    log_info(logger, "Hola! Se inicializo el modulo cliente CPU.");

    ip_kernel = config_get_string_value(config, "IP");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

    log_info(logger, "La configuracion de la conexion indica el PUERTO %s y la IP %s", ip_kernel, puerto_kernel);

    int fd_cpu = iniciar_servidor(puerto_kernel);
    log_info(logger, "CPU inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_kernel);
    int fd_kernel = esperar_cliente(logger, "CPU", fd_cpu);


    int ip_memoria = config_get_string_value(config, "IP");
    int puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    int fd_cpu_memoria = iniciar_servidor(puerto_memoria);
    int fd_memoria = esperar_cliente(logger, "CPU", fd_memoria);


    paquete->buffer = malloc(sizeof(t_buffer));
    recv(fd_kernel, &(paquete->codigo_operacion), sizeof(op_code), MSG_WAITALL);
    recv(fd_kernel, &(paquete->buffer->stream_size), sizeof(int), 0);
    paquete->buffer->stream = malloc(paquete->buffer->stream_size);
    recv(fd_kernel, paquete->buffer->stream, paquete->buffer->stream_size, 0);
    t_contexto_de_ejecucion* contexto;
    switch(paquete->codigo_operacion){
    	case CONTEXTO_DE_EJECUCION :
    		contexto = deserializar_contexto_de_ejecucion(paquete->buffer);
		inicializar_registros(contexto->registros_pcb);
    		char* instruccion_a_ejecutar;
    		strncpy(instruccion_a_ejecutar, fetch_instruccion(contexto), strlen(fetch_instruccion(contexto))+1);
    		decode_instruccion(instruccion_a_ejecutar, contexto, config, fd_memoria, fd_kernel, cpu_bloqueada);
    		break;
    	default:
    		log_warning(logger,"Operacion desconocida. No quieras meter la pata");
    		break;
    }





    return 0;
}



