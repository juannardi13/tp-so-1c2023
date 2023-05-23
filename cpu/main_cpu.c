#include "../shared/include/main_shared.h"
#include "./include/utils_cpu.h"
#include "../shared/src/main_shared.c"
#define MAX_LEN 256

int fetch_instruccion(char* una_instruccion, t_log* logger);

typedef struct {
	char* ax;
	char* bx;
	char* cx;
	char* dx;
}t_registros;

typedef struct {
	int pid;
	char** instrucciones;
	int pc;
	t_registros registros_pcb;
}t_pcb_cpu;

t_pcb_cpu pcb_en_ejecucion;
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
	char* ip_kernel;
	char* puerto_kernel;
	t_registros registros;
	registros.ax = "HOLA";
	registros.bx = "COMO";
	registros.cx = "ESTAS";
	registros.dx = "CHAU";

    t_log* logger = iniciar_logger();
    t_config* config = iniciar_config();

    log_info(logger, "Hola! Se inicializo el modulo cliente CPU.");

    ip_kernel = config_get_string_value(config, "IP");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

    log_info(logger, "La configuracion de la conexion indica el PUERTO %s y la IP %s", ip_kernel, puerto_kernel);

    int fd_cpu = iniciar_servidor(logger, "CPU", ip_kernel, puerto_kernel);
    log_info(logger, "CPU inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_kernel);
    int fd_kernel = esperar_cliente(logger, "CPU", fd_cpu);

    getchar();

    char** instrucciones_prueba = malloc(sizeof(char*) * 5);
    instrucciones_prueba[0] = "SET";
    instrucciones_prueba[1] = "YIELD";
    instrucciones_prueba[2] = "EXIT";
    instrucciones_prueba[3] = "YIELD";
    instrucciones_prueba[4] = "EXIT";

    pcb_en_ejecucion.instrucciones = instrucciones_prueba;
    pcb_en_ejecucion.pid = 1;
    pcb_en_ejecucion.pc = 0;
    pcb_en_ejecucion.registros_pcb = registros;

    while(pcb_en_ejecucion.pc < 5) {
    	int cod_op = fetch_instruccion(pcb_en_ejecucion.instrucciones[pcb_en_ejecucion.pc], logger);

    	switch (cod_op) {
    		case SET:
    			log_error(logger, "La instrucción %d es SET", pcb_en_ejecucion.pc);
    			pcb_en_ejecucion.pc++;
    			log_warning(logger, "Se actualizo el Program Counter a %d", pcb_en_ejecucion.pc);
    			break;
    		case YIELD:
    			log_error(logger, "La instrucción %d es YIELD", pcb_en_ejecucion.pc);
    			pcb_en_ejecucion.pc++;
    			log_warning(logger, "Se actualizo el Program Counter a %d", pcb_en_ejecucion.pc);
    			break;
    		case EXIT:
    			log_error(logger, "La instrucción %d es EXIT", pcb_en_ejecucion.pc);
    			pcb_en_ejecucion.pc++;
    			log_warning(logger, "Se actualizo el Program Counter a %d", pcb_en_ejecucion.pc);
    			break;
    		default:
    			log_error(logger, "Error");
    			break;
    	}

    }

  	return 0;
}

int fetch_instruccion(char* una_instruccion, t_log* logger) {
	int codigo_operacion;

	char* nombre_instruccion = strtok(una_instruccion, " \n");

	if(strcmp(nombre_instruccion, "YIELD") == 0) {
		codigo_operacion = YIELD;
	}

	if(strcmp(nombre_instruccion, "SET") == 0) {
		codigo_operacion = SET;
		log_error(logger, "Parámetro 1: %s", strtok(una_instruccion, " "));
		log_error(logger, "Parámetro 2: %s", strtok(una_instruccion, " "));
	}

	if(strcmp(nombre_instruccion, "EXIT") == 0) {
		codigo_operacion = EXIT;
	}

	return codigo_operacion;
}
