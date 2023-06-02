#include "../shared/include/main_shared.h"
#include "./include/utils_cpu.h"
#include "../shared/src/main_shared.c"
#define MAX_LEN 256

int fetch_instruccion(char* una_instruccion, t_log* logger);


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
	contexto->pc++;
	return instruccion_a_ejecutar;
}
// Falta inicializar t_instrucciones con codigos, desarrollar ejecutar_instruccion
t_instruccion decode_instruccion(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	switch(instruccion.codigo_instruccion){
	case SET:
		//TIENE UN RETARDO, CODEARLO USANDO LA CONFIG
		//esperar_tiempo()
		ejecutar_SET(instruccion, contexto);
	}
	case MOV_IN:
		ejecutar_MOV_IN(instruccion, contexto); // ojo hay que traducir direccion logica a fisica
	case MOV_OUT:
		ejecutar_MOV_OUT(instruccion, contexto);
	case I/O:
		ejecutar_I/O(instruccion, contexto);
	case F_OPEN:
		ejecutar_F_OPEN(instruccion, contexto);
	case F_CLOSE:
		ejecutar_F_CLOSE(instruccion, contexto);
	case F_SEEK:
		ejecutar_F_SEEK(instruccion, contexto);
	case F_READ:
		ejecutar_F_READ(instruccion, contexto); // idem anteriores
	case F_WRITE:
		ejecutar_F_WRITE(instruccion, contexto);
	case F_TRUNCATE:
		ejecutar_F_TRUNCATE(instruccion, contexto);
	case WAIT:
		ejecutar_WAIT(instruccion, contexto);
	case SIGNAL:
		ejecutar_SIGNAL(instruccion, contexto);
	case CREATE_SEGMENT:
		ejecutar_CREATE_SEGMENT(instruccion, contexto);
	case DELETE_SEGMENT:
		ejecutar_DELETE_SEGMENT(instruccion, contexto);
	case YIELD:
		ejecutar_YIELD(instruccion, contexto);
	case EXIT:
		ejecutar_EXIT(instruccion, contexto);
}

// agregye esta funcion en conexion cpu kernel - ver de donde la eliminamos
/*
void enviar_contexto_de_ejecucion(int fd_kernel, t_contexto_de_ejecucion contexto){
	t_paquete paquete = paquete_contexto_de_ejecucion(contexto);
	enviar_paquete(paquete, fd_kernel);
}
*/

void asignar_valor_a_registro(char* valor, char* registro, t_registros registros){
	if (registro == "AX"){
			strcpy(registros->ax, valor);
		}
	if (registro == "BX"){
			strcpy(registros->bx, valor);
		}
	if (registro == "CX"){
			strcpy(registros->cx, valor);
		}
	if (registro == "DX"){
			strcpy(registros->ax, valor);
		}
	if (registro == "EAX"){
			strcpy(registros->eax, valor);
		}
	if (registro == "EBX"){
			strcpy(registros->ebx, valor);
		}
	if (registro == "ECX"){
			strcpy(registros->ecx, valor);
		}
	if (registro == "EDX"){
			strcpy(registros->edx, valor);
		}
	if (registro == "RAX"){
			strcpy(registros->rax, valor);
		}
	if (registro == "RBX"){
			strcpy(registros->rbx, valor);
		}
	if (registro == "RCX"){
			strcpy(registros->rcx, valor);
		}
	if (registro == "RDX"){
			strcpy(registros->rdx, valor);
		}
}

void ejecutar_SET(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	asignar_valor_a_registro(intruccion->parametro_2, intruccion->parametro_1, contexto->registros);
}

void ejecutar_MOV_IN(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	char* valor;
	valor = buscar_en_memoria_direccion_logica(fd_memoria, instruccion->parametro_2); //retorna la direccion
	asignar_valor_a_registro(valor, intruccion->parametro_1, contexto->registros);
}
void ejecutar_MOV_IN(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	char* valor = buscar_en_memoria_direccion_logica(fd_memoria, instruccion->parametro_1);
	strcpy(valor, instruccion->parametro_2);
}
