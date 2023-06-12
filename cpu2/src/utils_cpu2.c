#include "utils_cpu2.h"

t_log* iniciar_logger(void) {
	t_log* nuevo_logger = log_create("cpu.log", "cpu.log", 1, LOG_LEVEL_INFO);

	if (nuevo_logger == NULL) {
		printf("No se pudo crear el log\n");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(void) {
	t_config* nuevo_config = config_create("cfg/cpu.config");

	if (nuevo_config == NULL) {
		printf("No se pudo leer la config\n");
		exit(2);
	}

	return nuevo_config;
}

//--------------------

t_instruccion* fetch_instruccion(t_contexto_de_ejecucion* contexto){
	t_instruccion* instruccion_a_ejecutar = list_get(contexto->instrucciones, contexto->pc);
	contexto->pc++;
	return instruccion_a_ejecutar;
}

//Revisar
//char** recibir_instrucciones(t_buffer){
//char** lista_instrucciones = deserializar_instrucciones(t_buffer);
//}


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

void asignar_valor_a_registro(char* valor, char* registro, t_registros* registros){
	if(strcmp(registro, "AX") == 0){
			strncpy(registros->ax, valor, strlen(valor)+1);
		}
	else if(strcmp(registro, "BX") == 0){
			strncpy(registros->bx, valor, strlen(valor)+1);
		}
	else if(strcmp(registro, "CX") == 0){
			strncpy(registros->cx, valor, strlen(valor)+1);
		}
	else if(strcmp(registro, "DX") == 0){
			strncpy(registros->ax, valor, strlen(valor)+1);
		}
	else if(strcmp(registro, "EAX") == 0){
			strncpy(registros->eax, valor, strlen(valor)+1);
		}
	else if(strcmp(registro, "EBX") == 0){
			strncpy(registros->ebx, valor, strlen(valor)+1);
		}
	else if(strcmp(registro, "ECX") == 0){
			strncpy(registros->ecx, valor, strlen(valor)+1);
		}
	else if(strcmp(registro, "EDX") == 0){
			strncpy(registros->edx, valor, strlen(valor)+1);
		}
	else if(strcmp(registro, "RAX") == 0){
			strncpy(registros->rax, valor, strlen(valor)+1);
		}
	else if(strcmp(registro, "RBX") == 0){
			strncpy(registros->rbx, valor, strlen(valor)+1);
		}
	else if(strcmp(registro, "RCX") == 0){
			strncpy(registros->rcx, valor, strlen(valor)+1);
		}
	else if(strcmp(registro, "RDX") == 0){
			strncpy(registros->rdx, valor, strlen(valor)+1);
		}
}

void activar_segmentation_fault(t_contexto_de_ejecucion* contexto);

bool desplazamiento_supera_tamanio(int desplazamiento, char* valor){
	int tamanio_valor = strlen(valor)+1;
	return desplazamiento > tamanio_valor;
}

char* leer_de_memoria(int direccion_fisica, t_config* config, int fd_memoria){

	t_paquete* paquete = crear_paquete(LEER_DE_MEMORIA); // IMPORTANTE ver con juani tema op_code y como crear paquete
	agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));

	enviar_paquete_a_servidor(paquete, fd_memoria);

	char* valor;
	strncpy(valor, get_string_value(fd_memoria), strlen(get_string_value(fd_memoria)));
	free(valor);
	eliminar_paquete(paquete);
	return valor; // antes o despues del free?
}

int obtener_direccion_fisica(int direccion_logica, int fd_memoria, t_config* config, t_contexto_de_ejecucion* contexto){
	int tamanio_segmento =  config_get_int_value(config, "TAM_SEGMENTO_0");
	int numero_segmento = floor((float)direccion_logica / (float)tamanio_segmento);
	int desplazamiento_segmento = direccion_logica % tamanio_segmento;
	int direccion_fisica = numero_segmento + desplazamiento_segmento;
	if(desplazamiento_supera_tamanio(desplazamiento_segmento, leer_de_memoria(direccion_fisica, config, fd_memoria))){
			activar_segmentation_fault(contexto);
		}
	return direccion_fisica;
}

void escribir_en_memoria(int direccion_fisica, char* valor, int fd_memoria){
	t_paquete* paquete = crear_paquete(ESCRIBIR_EN_MEMORIA); // IDEM leer_de_memoria
	agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
	agregar_a_paquete(paquete, &valor, sizeof(int));
	enviar_paquete_a_servidor(paquete, fd_memoria);
	eliminar_paquete(paquete);
}

char* mmu_valor_buscado(t_contexto_de_ejecucion* contexto, int direccion_logica, int fd_memoria, t_config* config){
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto);
	char* valor = leer_de_memoria(direccion_fisica, config, fd_memoria);
	return valor;
}


void ejecutar_SET(t_instruccion* instruccion, t_contexto_de_ejecucion* contexto){
	asignar_valor_a_registro(instruccion->parametro_2, instruccion->parametro_1, contexto->registros_pcb);
}

void ejecutar_MOV_IN(t_instruccion* instruccion, t_contexto_de_ejecucion* contexto, int fd_memoria, t_config* config){
	char* valor = mmu_valor_buscado(contexto, instruccion->parametro_2, fd_memoria, config);
	asignar_valor_a_registro(valor, instruccion->parametro_1, contexto->registros_pcb);
}

void ejecutar_MOV_OUT(t_instruccion* instruccion, t_contexto_de_ejecucion* contexto, int fd_memoria, t_config* config){
	char* valor;
	strncpy(valor, instruccion->parametro_2, strlen(instruccion->parametro_2));
	int direccion_fisica = obtener_direccion_fisica(instruccion->parametro_1, fd_memoria, config, contexto);
	escribir_en_memoria(direccion_fisica, valor, fd_memoria);
}
void ejecutar_IO(t_instruccion* instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel, bool cpu_bloqueada){
	t_paquete* paquete = crear_paquete(EJECUTAR_IO); // IDEM leer_de_memoria
	agregar_a_paquete(paquete, &(instruccion->parametro_1), sizeof(int));
	agregar_a_paquete(paquete, &contexto, (sizeof(int)+sizeof(int)+contexto->tamanio_registros+contexto->tamanio_instrucciones));
	enviar_paquete_a_servidor(paquete, fd_kernel);
	eliminar_paquete(paquete);
	cpu_bloqueada = true;
}
/*
void ejecutar_F_OPEN(t_instruccion instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_F_CLOSE(t_instruccion instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_F_SEEK(t_instruccion instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_F_READ(t_instruccion instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_F_WRITE(t_instruccion instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_F_TRUNCATE(t_instruccion instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_WAIT(t_instruccion instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_SIGNAL(t_instruccion instruccion, t_contexto_de_ejecucion contexto){

}

void ejecutar_CREATE_SEGMENT(t_instruccion instruccion, t_contexto_de_ejecucion contexto){

}

void ejecutar_DELETE_SEGMENT(t_instruccion instruccion, t_contexto_de_ejecucion contexto){

}

void ejecutar_YIELD(t_instruccion instruccion, t_contexto_de_ejecucion contexto){

}

void ejecutar_EXIT(t_instruccion instruccion, t_contexto_de_ejecucion contexto){

}
*/




// Falta inicializar t_instrucciones con codigos, no me reconoce codigo_operacion VER
void decode_instruccion(t_instruccion* instruccion, t_contexto_de_ejecucion* contexto, t_config* config, int fd_memoria, int fd_kernel, bool cpu_bloqueada){
	if(strcmp(instruccion->codigo_operacion, "SET") == 0){
		int tiempo_de_espera;
		tiempo_de_espera = config_get_int_value(config, "RETARDO_INSTRUCCION");
		sleep(tiempo_de_espera);
		ejecutar_SET(instruccion, contexto);
	}
	else if(strcmp(instruccion->codigo_operacion, "MOV_IN") == 0){
		ejecutar_MOV_IN(instruccion, contexto, fd_memoria, config);
	}
	else if(strcmp(instruccion->codigo_operacion, "MOV_OUT") == 0){
		ejecutar_MOV_OUT(instruccion, contexto, fd_memoria, config);
	}
	else if(strcmp(instruccion->codigo_operacion, "IO") == 0){
		ejecutar_IO(instruccion, contexto, fd_kernel, cpu_bloqueada);
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




char* valor_de_registro(char* registro, t_registros registros){
	char* valor;
	if (stcrmp(registro, "AX") == 0){
		strncpy(valor, registros->ax, strlen(registros->ax)+1);
		}
	if (stcrmp(registro, "BX") == 0){
		strncpy(valor, registros->bx, strlen(registros->bx)+1);
		}
	if (stcrmp(registro, "CX") == 0){
		strncpy(valor, registros->cx, strlen(registros->cx)+1);
		}
	if (stcrmp(registro, "DX") == 0){
		strncpy(valor, registros->dx, strlen(registros->dx)+1);
		}
	if (stcrmp(registro, "EAX") == 0){
		strncpy(valor, registros->eax, strlen(registros->eax)+1);
		}
	if (stcrmp(registro, "EBX") == 0){
		strncpy(valor, registros->ebx, strlen(registros->ebx)+1);
		}
	if (stcrmp(registro, "ECX") == 0){
		strncpy(valor, registros->ecx, strlen(registros->ecx)+1);
		}
	if (stcrmp(registro, "EDX") == 0){
		strncpy(valor, registros->edx, strlen(registros->edx)+1);
		}
	if (stcrmp(registro, "RAX") == 0){
		strncpy(valor, registros->rax, strlen(registros->rax)+1);
		}
	if (stcrmp(registro, "RBX") == 0){
		strncpy(valor, registros->rbx, strlen(registros->rbx)+1);
		}
	if (stcrmp(registro, "RCX") == 0){
		strncpy(valor, registros->rcx, strlen(registros->rcx)+1);
		}
	if (stcrmp(registro, "RDX") == 0){
		strncpy(valor, registros->rdx, strlen(registros->rdx)+1);
		}
	return valor;
}


