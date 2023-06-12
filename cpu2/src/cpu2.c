#include <shared-2.h>

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
	t_registros registros;
	strncpy(registros.ax, "HOLA", strlen("HELLO")+1);
	strncpy(registros.bx, "COMO", strlen("COMO")+1);
	strncpy(registros.cx, "ESTAS", strlen("ESTAS")+1);
	strncpy(registros.dx, "CHAU", strlen("CHAU")+1);

    t_log* logger = iniciar_logger();
    t_config* config = iniciar_config();

    log_info(logger, "Hola! Se inicializo el modulo cliente CPU.");

    ip_kernel = config_get_string_value(config, "IP");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

    log_info(logger, "La configuracion de la conexion indica el PUERTO %s y la IP %s", ip_kernel, puerto_kernel);

    int fd_cpu = iniciar_servidor(puerto_kernel);
    log_info(logger, "CPU inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_kernel);
    int fd_kernel = esperar_cliente(logger, "CPU", fd_cpu);

    getchar();

    char** instrucciones_prueba = malloc(sizeof(char*) * 5);
list_add_in_index(instrucciones_prueba, 0, "SET");
    list_add_in_index(instrucciones_prueba, 1, "YIELD");
    list_add_in_index(instrucciones_prueba, 2, "EXIT");
    list_add_in_index(instrucciones_prueba, 3, "YIELD");
    list_add_in_index(instrucciones_prueba, 4, "EXIT");
	
list_add_all(contexto.instrucciones, instrucciones_prueba);
    contexto.pid = 1;
    contexto.pc = 0;
	contexto.registros_pcb = registros;

    while(contexto.pc < 5) {
    	int cod_op = fetch_instruccion(contexto.pc);

    	switch (cod_op) {
    		case SET:
    			log_error(logger, "La instrucción %d es SET", contexto.pc);
    			contexto.pc++;
    			log_warning(logger, "Se actualizo el Program Counter a %d", contexto.pc);
    			break;
    		case YIELD:
    			log_error(logger, "La instrucción %d es YIELD", contexto.pc);
    			contexto.pc++;
    			log_warning(logger, "Se actualizo el Program Counter a %d", contexto.pc);
    			break;
    		case EXIT:
    			log_error(logger, "La instrucción %d es EXIT", contexto.pc);
    			contexto.pc++;
    			log_warning(logger, "Se actualizo el Program Counter a %d", contexto.pc);
    			break;
    		default:
    			log_error(logger, "Error");
    			break;
    	}

    }
int ip_memoria = config_get_string_value(config, "IP");
    int puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    int fd_cpu_memoria = iniciar_servidor(puerto_memoria);
    int fd_memoria = esperar_cliente(logger, "CPU", fd_memoria);
  	return 0;
}



