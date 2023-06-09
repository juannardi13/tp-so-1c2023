#include <shared-2.h>

#include "utils_cpu2.h"

#define MAX_LEN 256


typedef struct {
	int pid;
	t_list* instrucciones;
	int pc;
	t_registros registros_pcb;
}t_contexto_de_ejecucion;

t_contexto_de_ejecucion contexto;
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
	strncpy(registros.ax, "HOLA", length("HELLO"));
	strncpy(registros.bx, "COMO", length("COMO"));
	strncpy(registros.cx, "ESTAS", length("ESTAS"));
	strncpy(registros.dx, "CHAU", length("CHAU"));

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

  strncpy(contexto.instrucciones, instrucciones_prueba, length(instrucciones_prueba));
    contexto.pid = 1;
    contexto.pc = 0;
    strncpy(contexto.registros_pcb, registros, length(registros));


    while(pcb_en_ejecucion.pc < 5) {
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

t_instruccion* fetch_instruccion(t_contexto_de_ejecucion* contexto){
	t_instruccion* instruccion_a_ejecutar = list_get(contexto->instrucciones, contexto->pc);
	contexto->pc++;
	return instruccion_a_ejecutar;
}
// Falta inicializar t_instrucciones con codigos, no me reconoce codigo_operacion VER
void decode_instruccion(t_instruccion* instruccion, t_contexto_de_ejecucion contexto*, t_config* config){
	if(strcmp(instruccion->codigo_operacion, "SET") == 0){
		int tiempo_de_espera;
		sleep(config_get_int_value(config, tiempo_de_espera));
		ejecutar_SET(instruccion, contexto);
	}
	else if(stcrmp(instruccion->codigo_operacion, "MOV_IN") == 0){
		ejecutar_MOV_IN(instruccion, contexto); // ojo hay que traducir direccion logica a fisica
	}
	else if(stcrmp(instruccion->codigo_operacion, "MOV_OUT") == 0){
		ejecutar_MOV_OUT(instruccion, contexto);
	}
	else if(stcrmp(instruccion->codigo_operacion, "IO") == 0){
		ejecutar_IO(instruccion, contexto);
	}
	else if(stcrmp(instruccion->codigo_operacion, "F_OPEN") == 0){
		ejecutar_F_OPEN(instruccion, contexto);
	}
	else if(stcrmp(instruccion->codigo_operacion, "F_CLOSE") == 0){
		ejecutar_F_CLOSE(instruccion, contexto);
	}

	else if(stcrmp(instruccion->codigo_operacion, "F_SEEK") == 0){
			ejecutar_F_SEEK(instruccion, contexto);
		}
	else if(stcrmp(instruccion->codigo_operacion, "F_READ") == 0){
			ejecutar_F_READ(instruccion, contexto);
		}
	else if(stcrmp(instruccion->codigo_operacion, "F_WRITE") == 0){
			ejecutar_F_WRITE(instruccion, contexto);
		}
	else if(stcrmp(instruccion->codigo_operacion, "F_TRUNCATE") == 0){
			ejecutar_F_TRUNCAE(instruccion, contexto);
		}
	else if(stcrmp(instruccion->codigo_operacion, "WAIT") == 0){
			ejecutar_WAIT(instruccion, contexto);
		}
	else if(stcrmp(instruccion->codigo_operacion, "SIGNAL") == 0){
			ejecutar_SIGNAL(instruccion, contexto);
		}
	else if(stcrmp(instruccion->codigo_operacion, "CREATE_SEGMENT") == 0){
			ejecutar_CREATE_SEGMENT(instruccion, contexto);
		}
	else if(stcrmp(instruccion->codigo_operacion, "DELETE_SEGMENT") == 0){
				ejecutar_DELETE_SEGMENT(instruccion, contexto);
			}
	else if(stcrmp(instruccion->codigo_operacion, "YIELD") == 0){
				ejecutar_YIELD(instruccion, contexto);
			}
	else if(stcrmp(instruccion->codigo_operacion, "EXIT") == 0){
				ejecutar_EXIT(instruccion, contexto);
			}
}

// agregye esta funcion en conexion cpu kernel - ver de donde la eliminamos
/*
void enviar_contexto_de_ejecucion(int fd_kernel, t_contexto_de_ejecucion contexto){
	t_paquete paquete = paquete_contexto_de_ejecucion(contexto);
	enviar_paquete(paquete, fd_kernel);
}
*/

void asignar_valor_a_registro(char* valor, char* registro, t_registros* registros){
	if(stcrmp(registro, "AX") == 0){
			strncpy(registros->ax, valor, length(valor));
		}
	else if(stcrmp(registro, "BX") == 0){
			strncpy(registros->bx, valor, length(valor));
		}
	else if(stcrmp(registro, "CX") == 0){
			strncpy(registros->cx, valor, length(valor));
		}
	else if(stcrmp(registro, "DX") == 0){
			strncpy(registros->ax, valor, length(valor));
		}
	else if(stcrmp(registro, "EAX") == 0){
			strncpy(registros->eax, valor, length(valor));
		}
	else if(stcrmp(registro, "EBX") == 0){
			strncpy(registros->ebx, valor, length(valor));
		}
	else if(stcrmp(registro, "ECX") == 0){
			strncpy(registros->ecx, valor, length(valor));
		}
	else if(stcrmp(registro, "EDX") == 0){
			strncpy(registros->edx, valor, length(valor));
		}
	else if(stcrmp(registro, "RAX") == 0){
			strncpy(registros->rax, valor, length(valor));
		}
	else if(stcrmp(registro, "RBX") == 0){
			strncpy(registros->rbx, valor, length(valor));
		}
	else if(stcrmp(registro, "RCX") == 0){
			strncpy(registros->rcx, valor, length(valor));
		}
	else if(stcrmp(registro, "RDX") == 0){
			strncpy(registros->rdx, valor, length(valor));
		}
}
char* valor_de_registro(char* registro, t_registros registros){
	char* valor;
	if (stcrmp(registro, "AX") == 0){
		strncpy(valor, registros->ax, length(registros->ax));
		}
	if (stcrmp(registro, "BX") == 0){
		strncpy(valor, registros->bx, length(registros->bx));
		}
	if (stcrmp(registro, "CX") == 0){
		strncpy(valor, registros->cx, length(registros->cx));
		}
	if (stcrmp(registro, "DX") == 0){
		strncpy(valor, registros->dx, length(registros->dx));
		}
	if (stcrmp(registro, "EAX") == 0){
		strncpy(valor, registros->eax, length(registros->eax));
		}
	if (stcrmp(registro, "EBX") == 0){
		strncpy(valor, registros->ebx, length(registros->ebx));
		}
	if (stcrmp(registro, "ECX") == 0){
		strncpy(valor, registros->ecx, length(registros->ecx));
		}
	if (stcrmp(registro, "EDX") == 0){
		strncpy(valor, registros->edx, length(registros->edx));
		}
	if (stcrmp(registro, "RAX") == 0){
		strncpy(valor, registros->rax, length(registros->rax));
		}
	if (stcrmp(registro, "RBX") == 0){
		strncpy(valor, registros->rbx, length(registros->rbx));
		}
	if (stcrmp(registro, "RCX") == 0){
		strncpy(valor, registros->rcx, length(registros->rcx));
		}
	if (stcrmp(registro, "RDX") == 0){
		strncpy(valor, registros->rdx, length(registros->rdx));
		}
	return valor;
}

void ejecutar_SET(t_instruccion* instruccion, t_contexto_de_ejecucion* contexto){
	// par1 = registro, par2 = valor
	asignar_valor_a_registro(intruccion->parametro_2, intruccion->parametro_1, contexto->registros_pcb);
}

void ejecutar_MOV_IN(t_instruccion* instruccion, t_contexto_de_ejecucion* contexto){
	char* valor;
	strncpy((buscar_en_memoria_direccion_logica(int fd_memoria, instruccion->parametro_2)), valor, length(valor)); //retorna la direccion
	strncpy(valor, intruccion->parametro_1, contexto->registros_pcb);
}
void ejecutar_MOV_OUT(t_instruccion* instruccion, t_contexto_de_ejecucion* contexto){
	char* valor;
	strncpy(valor, instruccion->parametro_2, length(instruccion->parametro_2));
	char* direccion_fisica;
	strncpy(direccion_fisica, buscar_en_memoria_direccion_logica(int fd_memoria, instruccion->parametro_1), length(buscar_en_memoria_direccion_logica(int fd_memoria, instruccion->parametro_1));
	strncpy(direccion_fisica, valor, length(valor));
}
/*void ejecutar_IO(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	// par1 = tiempo
	empaquetar el contexto y enviarlo a kernel junto con el tiempo de espera
}
void ejecutar_F_OPEN(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	// par1 = nombre archivo
}
void ejecutar_F_CLOSE(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	// par1 = nombre archivo
}
void ejecutar_F_SEEK(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	// par1 = nombre archivo, par2 = posicion
}
void ejecutar_F_READ(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	// par1 = nombre archivo, par2 = dir logica, par3 = cant de bytes
}
void ejecutar_F_WRITE(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	// par1 = nombre archivo, par2 = dir logica, par3 = cant de bytes
}
void ejecutar_F_TRUNCATE(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	// par1 = nombre archivo, par2 = tamanio
}
void ejecutar_WAIT(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	// par1 = recurso
}
void ejecutar_SIGNAL(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	// par1 = recurso
}

void ejecutar_CREATE_SEGMENT(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	// par1 = id del segmetno, par2 = tamanio

void ejecutar_DELETE_SEGMENT(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	// par1 = id del segmento
}

void ejecutar_YIELD(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
 // ningun parametro
  *
}

void ejecutar_EXIT(t_instruccion instruccion, t_contexto_de_ejecucion contexto){
	// ningun parametro
}
*/
