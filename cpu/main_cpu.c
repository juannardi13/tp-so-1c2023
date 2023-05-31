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

// ver si se puede eliminar
/*
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
*/

t_instruccion fetch_instruccion(t_contexto_de_ejecucion contexto){
	t_instruccion instruccion_a_ejecutar = list_get(contexto->instrucciones, contexto->pc);
	return instruccion_a_ejecutar;
}
// Falta inicializar t_instrucciones con codigos, desarrollar ejecutar_instruccion
t_instruccion decode_instruccion(t_instruccion instruccion){
	switch(instruccion.codigo_instruccion){
	case 0:
		// SET
		ejecutar_instruccion(set(instruccion.parametro_registro, instruccion.parametro_numerico));
	}
	case 1:
		// MOV_IN
		ejecutar_instruccion(mov_in(instruccion.parametro_registro, instruccion.parametro_direccion_logica)); // ojo hay que traducir direccion logica a fisica
	case 2:
		// MOV_OUT
		ejecutar_instruccion(mov_out(instruccion.parametro_direccion_logica, instruccion.parametro_registro));
	case 3:
		// I/O
		ejecutar_instruccion(io(instruccion.parametro_numerico));
	case 4:
		// F_OPEN
		ejecutar_instruccion(f_open(instruccion.parametro_1));
	case 5:
		// F_CLOSE
		ejecutar_instruccion(f_close(instruccion.parametro_1));
	case 6:
		// F_SEEK
		ejecutar_instruccion(f_seek(instruccion.parametro_1, instruccion.parametro_numerico));
	case 7:
		// F_READ
		ejecutar_instruccion(f_read(instruccion.parametro_1, instruccion.parametro_direccion_logica, instruccion.parametro_numerico)); // idem anteriores
	case 8:
		// F_WRITE
		ejecutar_instruccion(f_write(instruccion.parametro_1, instruccion.parametro_direccion_logica, instruccion.parametro_numerico));
	case 9:
		// F_TRUNCATE
		ejecutar_instruccion(f_truncate(instruccion.parametro_1, instruccion.parametro_numerico));
	case 10:
		// WAIT
		ejecutar_instruccion(wait(instruccion.parametro_recurso));
	case 11:
		// SIGNAL
		ejecutar_instruccion(signal(instruccion.parametro_recurso));
	case 12:
		// CREATE_SEGMENT
		ejecutar_instruccion(create_segment(instruccion.parametro_numerico, instruccion.parametro_numerico2));
	case 13:
		// DELETE_SEGMENT
		ejecutar_instruccion(delete_segment(instruccion.parametro_numerico));
	case 14:
		// YIELD
		ejecutar_instruccion(yield());
	case 15:
		// EXIT
		ejecutar_instruccion(exit());
}

// agregye esta funcion en conexion cpu kernel - ver de donde la eliminamos
/*
void enviar_contexto_de_ejecucion(int fd_kernel, t_contexto_de_ejecucion contexto){
	t_paquete paquete = paquete_contexto_de_ejecucion(contexto);
	enviar_paquete(paquete, fd_kernel);
}
*/
