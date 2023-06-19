#include"utils_cpu2.h"
#include <math.h>
#include<shared-2.h>

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

char* fetch_instruccion(t_contexto_de_ejecucion* contexto){
	char* unas_intrucciones;
	strncpy(unas_intrucciones, contexto->instrucciones, strlen(contexto->instrucciones)+1);
	char** intrucciones_parseadas = string_split(unas_intrucciones, "\n");
	char* instruccion_a_ejecutar = intrucciones_parseadas[contexto->pc];
	contexto->pc++;
	return instruccion_a_ejecutar;
}

/*//Revisar
char** recibir_instrucciones(t_buffer){
char** lista_instrucciones = deserializar_instrucciones(t_buffer);
}
*/

t_registros registros;

void inicializar_registros(void){
	registros->ax = string_duplicate("0000");
	registros->bx = string_duplicate("0000");
	registros->cx = string_duplicate("0000");
	registros->dx = string_duplicate("0000");
	registros->eax = string_duplicate("00000000");
	registros->ebx = string_duplicate("00000000");
	registros->ecx = string_duplicate("00000000");
	registros->edx = string_duplicate("00000000");
	registros->rax = string_duplicate("0000000000000000");
	registros->rbx = string_duplicate("0000000000000000");
	registros->rcx = string_duplicate("0000000000000000");
	registros->rdx = string_duplicate("0000000000000000");
}


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
char* deserializar_paquete_de_memoria(t_buffer* buffer){
	char* valor = malloc (buffer->stream_size);
	memset(valor, 0, buffer->stream_size);
	void* stream = buffer->stream;
	memcpy(valor, stream, buffer->stream_size);
	return valor;
}
t_contexto_de_ejecucion* deserializar_contexto_de_ejecucion(t_buffer* buffer){
	t_contexto_de_ejecucion* contexto = malloc (buffer->stream_size);
	void* stream = buffer->stream;
	memcpy(&(contexto->instrucciones), stream, contexto->tamanio_instrucciones);
	stream+=contexto->tamanio_instrucciones;
	memcpy(&(contexto->pc), stream, sizeof(int));
	stream+=sizeof(int);
	memcpy(&(contexto->pid), stream, sizeof(int));
	stream+=sizeof(int);
	memcpy(&(contexto->registros_pcb), stream, contexto->tamanio_registros);
	stream+=contexto->tamanio_registros;
	memcpy(&(contexto->segmentos), stream, contexto->tamanio_segmentos);
	stream+=contexto->contexto->tamanio_segmentos;
	return contexto;
}
char* leer_de_memoria(int direccion_fisica, t_config* config, int fd_memoria){

	t_paquete* paquete = crear_paquete(LEER_DE_MEMORIA); // IMPORTANTE ver con juani tema op_code y como crear paquete
	agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
	enviar_paquete(paquete, fd_memoria);
	//crear op_code RESPUESTA_MEMORIA, lo igualo a recibir_operacion => se bloquea proceso hasta que reciba algo con el mismo codigo
	eliminar_paquete(paquete);
	t_paquete* paquete2 = malloc(sizeof(t_paquete));
	paquete2->buffer = malloc(sizeof(t_buffer));
	recv(fd_memoria, &(paquete2->codigo_operacion), sizeof(op_code), MSG_WAITALL);
	recv(fd_memoria, &(paquete2->buffer->stream_size), sizeof(int), 0);
	paquete2->buffer->stream = malloc(paquete2->buffer->stream_size);
	recv(fd_memoria, paquete2->buffer->stream, paquete2->buffer->stream_size, 0);
	void* valor;
	switch(paquete2->codigo_operacion){
		case LEIDO :
			valor = deserializar_paquete_de_memoria(paquete2->buffer);
			break;
		case NO_LEIDO :
			printf("No se pudo leer el valor del registro dado");
			break;
		default :
			printf("Operacion desconocida");
			break;
	}
	return valor;
}

int obtener_direccion_fisica(int direccion_logica, int fd_memoria, t_config* config, t_contexto_de_ejecucion* contexto){
	int tamanio_segmento =  config_get_int_value(config, "TAM_SEGMENTO_0");
	int numero_segmento = floor((float)contexto->segmento->base / (float)tamanio_segmento);
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
	enviar_paquete(paquete, fd_memoria);
	eliminar_paquete(paquete);
}

char* mmu_valor_buscado(t_contexto_de_ejecucion* contexto, int direccion_logica, int fd_memoria, t_config* config){
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto);
	char* valor = leer_de_memoria(direccion_fisica, config, fd_memoria);
	return valor;
}

void ejecutar_SET(char** instruccion, t_contexto_de_ejecucion* contexto){
	asignar_valor_a_registro(instruccion[2], instruccion[1], contexto->registros_pcb);
}

void ejecutar_MOV_IN(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_memoria, t_config* config){
	char* valor = mmu_valor_buscado(contexto, instruccion[2], fd_memoria, config);
	asignar_valor_a_registro(valor, instruccion[1], contexto->registros_pcb);
}

void ejecutar_MOV_OUT(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_memoria, t_config* config){
	char* valor;
	strncpy(valor, instruccion[2], strlen(instruccion[2]));
	int direccion_fisica = obtener_direccion_fisica(instruccion[1], fd_memoria, config, contexto);
	escribir_en_memoria(direccion_fisica, valor, fd_memoria);
}
void ejecutar_IO(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel, bool cpu_bloqueada){
	t_paquete* paquete = crear_paquete(IO); // IDEM leer_de_memoria
	agregar_a_paquete(paquete, &(instruccion[1]), sizeof(int));
	agregar_a_paquete(paquete, &contexto, (sizeof(int)+sizeof(int)+contexto->tamanio_registros+contexto->tamanio_instrucciones));
	enviar_paquete(paquete, fd_kernel);
	eliminar_paquete(paquete);
	cpu_bloqueada = true;
}
/*
void ejecutar_F_OPEN(char** instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_F_CLOSE(char** instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_F_SEEK(char** instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_F_READ(char** instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_F_WRITE(char** instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_F_TRUNCATE(char** instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_WAIT(char** instruccion, t_contexto_de_ejecucion contexto){

}
void ejecutar_SIGNAL(char** instruccion, t_contexto_de_ejecucion contexto){

}

void ejecutar_CREATE_SEGMENT(char** instruccion, t_contexto_de_ejecucion contexto){

}

void ejecutar_DELETE_SEGMENT(char** instruccion, t_contexto_de_ejecucion contexto){

}

void ejecutar_YIELD(char** instruccion, t_contexto_de_ejecucion contexto){

}

void ejecutar_EXIT(char** instruccion, t_contexto_de_ejecucion contexto){

}
*/

void decode_instruccion(char* instruccion, t_contexto_de_ejecucion* contexto, t_config* config, int fd_memoria, int fd_kernel, bool cpu_bloqueada){

	char** instruccion_parseada = string_split(instruccion, " ");

	if(strcmp(instruccion_parseada[0], "SET") == 0){
		int tiempo_de_espera;
		tiempo_de_espera = config_get_int_value(config, "RETARDO_INSTRUCCION");
		sleep(tiempo_de_espera);
		ejecutar_SET(instruccion_parseada, contexto);
	}
	else if(strcmp(instruccion_parseada[0], "MOV_IN") == 0){
		ejecutar_MOV_IN(instruccion_parseada, contexto, fd_memoria, config);
	}
	else if(strcmp(instruccion_parseada[0], "MOV_OUT") == 0){
		ejecutar_MOV_OUT(instruccion_parseada, contexto, fd_memoria, config);
	}
	else if(strcmp(instruccion_parseada[0], "IO") == 0){
		ejecutar_IO(instruccion_parseada, contexto, fd_kernel, cpu_bloqueada);
	}
	else if(stcrmp(instruccion_parseada[0], "F_OPEN") == 0){
		ejecutar_F_OPEN(instruccion_parseada, contexto);
	}
	else if(stcrmp(instruccion_parseada[0], "F_CLOSE") == 0){
		ejecutar_F_CLOSE(instruccion_parseada, contexto);
	}
	else if(stcrmp(instruccion_parseada[0], "F_SEEK") == 0){
			ejecutar_F_SEEK(instruccion_parseada, contexto);
		}
	else if(stcrmp(instruccion_parseada[0], "F_READ") == 0){
			ejecutar_F_READ(instruccion_parseada, contexto);
		}
	else if(stcrmp(instruccion_parseada[0], "F_WRITE") == 0){
			ejecutar_F_WRITE(instruccion_parseada, contexto);
		}
	else if(stcrmp(instruccion_parseada[0], "F_TRUNCATE") == 0){
			ejecutar_F_TRUNCAE(instruccion_parseada, contexto);
		}
	else if(stcrmp(instruccion_parseada[0], "WAIT") == 0){
			ejecutar_WAIT(instruccion_parseada, contexto);
		}
	else if(stcrmp(instruccion_parseada[0], "SIGNAL") == 0){
			ejecutar_SIGNAL(instruccion_parseada, contexto);
		}
	else if(stcrmp(instruccion_parseada[0], "CREATE_SEGMENT") == 0){
			ejecutar_CREATE_SEGMENT(instruccion_parseada, contexto);
		}
	else if(stcrmp(instruccion_parseada[0], "DELETE_SEGMENT") == 0){
				ejecutar_DELETE_SEGMENT(instruccion_parseada, contexto);
			}
	else if(stcrmp(instruccion_parseada[0], "YIELD") == 0){
				ejecutar_YIELD(instruccion_parseada, contexto);
			}
	else if(stcrmp(instruccion_parseada[0], "EXIT") == 0){
				ejecutar_EXIT(instruccion_parseada, contexto);
			}
}

// agregye esta funcion en conexion cpu kernel - ver de donde la eliminamos
/*
void enviar_contexto_de_ejecucion(int fd_kernel, t_contexto_de_ejecucion contexto){
	t_paquete paquete = paquete_contexto_de_ejecucion(contexto);
	enviar_paquete(paquete, fd_kernel);
}
*/

