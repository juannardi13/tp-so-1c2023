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

int convertirAEntero(char *cadena) {
    int resultado = 0;

    for (int i = 0; cadena[i] != '\0'; ++i) {
            resultado = resultado * 10 + (cadena[i] - '0');
    }
    return resultado;
}

char* fetch_instruccion(t_contexto_de_ejecucion* contexto){
	char** intrucciones_parseadas = string_split(contexto->instrucciones, "\n");
	char* instruccion_a_ejecutar = intrucciones_parseadas[contexto->pc];
	contexto->pc++;
	return instruccion_a_ejecutar;
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

void activar_segmentation_fault(t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_paquete* paquete = crear_paquete(SEG_FAULT);
		//serializar_contexto_ejecucion(contexto);
		enviar_paquete(paquete, fd_kernel);
		eliminar_paquete(paquete);
}

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

void deserializar_segmentos(t_segmento* segmento_actual, void* stream){
	memcpy(&(segmento_actual->base), stream, sizeof(int));
	stream += sizeof(int);
	memcpy(&(segmento_actual->id), stream, sizeof(int));
	stream += sizeof(int);
	memcpy(&(segmento_actual->tamanio), stream, sizeof(int));
	stream += sizeof(int);
}

void recibir_contexto(int fd_kernel, t_contexto_de_ejecucion* contexto){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	recv(fd_kernel, &(paquete->buffer->stream_size), sizeof(int), 0);
	paquete->buffer->stream = malloc(paquete->buffer->stream_size);
	recv(fd_kernel, paquete->buffer->stream, paquete->buffer->stream_size, 0);

	switch(paquete->codigo_operacion){
	case CONTEXTO_DE_EJECUCION:
		void* stream = paquete->buffer->stream;
		memcpy(&(contexto->pid), stream, sizeof(int));
		stream += sizeof(int);
		memcpy(&(contexto->tamanio_instrucciones), stream, sizeof(int));
		stream += sizeof(int);

		contexto->instrucciones = malloc(contexto->tamanio_instrucciones);
		memset(&(contexto->instrucciones), 0, contexto->tamanio_instrucciones);

		memcpy(&(contexto->instrucciones), stream, contexto->tamanio_instrucciones);
		stream += contexto->tamanio_instrucciones;

		memcpy(&(contexto->pc), stream, sizeof(int));
		stream += sizeof(int);

		memcpy(&(contexto->registros_pcb), stream, sizeof(t_registros));
		stream += sizeof(t_registros);

//		t_registros* registro_actual = contexto->registros_pcb;
//		for(int i=0; i<(contexto->tamanio_registros); i++){
//			memcpy(&(registro_actual), stream, sizeof(t_registros));
//			stream += sizeof(t_registros);
//			registro_actual++;
//		}

//		for(int a=0; a<(list_size(contexto->tabla_segmentos)); a++){
//			t_segmento* segmento_actual = list_get(contexto->tabla_segmentos, a);
//			deserializar_segmentos(segmento_actual, stream);
//				}
			}
	/*
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);*/

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
	//int numero_segmento = floor((float)contexto->segmentos->base / (float)tamanio_segmento);
	int numero_segmento = floor(direccion_logica / tamanio_segmento);
	int desplazamiento_segmento = direccion_logica % tamanio_segmento;
	t_segmento* segmento_buscado = list_get(contexto->tabla_segmentos, numero_segmento);
	int direccion_fisica = segmento_buscado->base + desplazamiento_segmento;
	if(desplazamiento_supera_tamanio(desplazamiento_segmento, leer_de_memoria(direccion_fisica, config, fd_memoria))){
		//	activar_segmentation_fault(contexto);
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
	char* valor;
	strncpy(valor, leer_de_memoria(direccion_fisica, config, fd_memoria), strlen(leer_de_memoria(direccion_fisica, config, fd_memoria)) + 1);
	return valor;
}

void ejecutar_SET(char** instruccion, t_contexto_de_ejecucion* contexto){
	asignar_valor_a_registro(instruccion[2], instruccion[1], contexto->registros_pcb);
}

void ejecutar_MOV_IN(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_memoria, t_config* config){
	int direccion_logica = convertirAEntero(instruccion[2]);
	asignar_valor_a_registro(mmu_valor_buscado(contexto, direccion_logica, fd_memoria, config), instruccion[1], contexto->registros_pcb);
}

void ejecutar_MOV_OUT(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_memoria, t_config* config){
	int direccion_logica = convertirAEntero(instruccion[1]);
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto);
	escribir_en_memoria(direccion_fisica, instruccion[2], fd_memoria);
}

void serializar_segmentos(t_segmento* segmento_actual, void* stream, int offset){
	memcpy(stream + offset, &segmento_actual->base, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &segmento_actual->id, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &segmento_actual->tamanio, sizeof(int));
	offset += sizeof(int);
}


void ejecutar_IO(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel, bool cpu_bloqueada){
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->stream_size = sizeof(int)*6 + contexto->tamanio_instrucciones + contexto->tamanio_segmentos + contexto->tamanio_registros;

	void* stream = malloc(buffer->stream_size);
	int offset = 0;
	memcpy(stream + offset, &contexto->pid, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->tamanio_instrucciones, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->pc, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->tamanio_registros, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->tamanio_segmentos, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;
	memcpy(stream + offset, &contexto->instrucciones, contexto->tamanio_instrucciones);
	offset += contexto->tamanio_instrucciones;

	t_registros* registro_actual = contexto->registros_pcb;
	for(int i=0; i<(contexto->tamanio_registros); i++){
		memcpy(stream + offset, registro_actual, sizeof(t_registros));
		offset += sizeof(t_registros);
		registro_actual++;
	}

	for(int a=0; a<(contexto->tamanio_segmentos); a++){
		t_segmento* segmento_actual = list_get(contexto->tabla_segmentos, a);
		serializar_segmentos(segmento_actual, stream, offset);
		segmento_actual++;
	}

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = IO;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void serializar_contexto(t_contexto_de_ejecucion* contexto, t_buffer* buffer, void* stream, int offset){
	buffer = malloc(sizeof(t_buffer));
	buffer->stream_size += sizeof(int)*5 + contexto->tamanio_instrucciones + contexto->tamanio_segmentos + contexto->tamanio_registros;
	stream = malloc(buffer->stream_size);
	memcpy(stream + offset, &contexto->pid, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->tamanio_instrucciones, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->pc, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->tamanio_registros, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->tamanio_segmentos, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &contexto->instrucciones, contexto->tamanio_instrucciones);
	offset += contexto->tamanio_instrucciones;

	t_registros* registro_actual = contexto->registros_pcb;
	for(int i=0; i<(contexto->tamanio_registros); i++){
		memcpy(stream + offset, registro_actual, sizeof(t_registros));
		offset += sizeof(t_registros);
		registro_actual++;
	}
	t_segmento* segmento_actual = contexto->tabla_segmentos;
	for(int a=0; a<(contexto->tamanio_segmentos); a++){
		serializar_segmentos(segmento_actual, stream, offset);
		segmento_actual++;
	}
}

void ejecutar_F_OPEN(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+1;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = F_OPEN;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_F_CLOSE(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+1;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = F_CLOSE;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_F_SEEK(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+ 2 + strlen(instruccion[2]);
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;
	memcpy(stream + offset, &instruccion[2], strlen(instruccion[2])+1);
	offset += strlen(instruccion[2])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = F_SEEK;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
	}

void ejecutar_F_READ(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel, int fd_memoria, t_config* config){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+ sizeof(int) + strlen(instruccion[2]) + 2;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;
	int direccion_logica = convertirAEntero(instruccion[2]);
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto);
	memcpy(stream + offset, &direccion_fisica, sizeof(int));
	offset += strlen(instruccion[2])+1;
	memcpy(stream + offset, &instruccion[3], strlen(instruccion[3])+1);
	offset += strlen(instruccion[3])+1;
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = F_READ;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_F_WRITE(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel, int fd_memoria, t_config* config){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+ sizeof(int) + strlen(instruccion[2]) + 2;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;
	int direccion_logica = convertirAEntero(instruccion[2]);
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto);
	memcpy(stream + offset, &direccion_fisica, sizeof(int));
	offset += strlen(instruccion[2])+1;
	memcpy(stream + offset, &instruccion[3], strlen(instruccion[3])+1);
	offset += strlen(instruccion[3])+1;
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = F_WRITE;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_F_TRUNCATE(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1]) + strlen(instruccion[2]) + 2;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;
	memcpy(stream + offset, &instruccion[2], strlen(instruccion[2])+1);
	offset += strlen(instruccion[2])+1;
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = F_TRUNCATE;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_WAIT(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+1;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = WAIT;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_SIGNAL(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+1;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = SIGNAL;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_CREATE_SEGMENT(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+ 2 + strlen(instruccion[2]);
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;
	memcpy(stream + offset, &instruccion[2], strlen(instruccion[2])+1);
	offset += strlen(instruccion[2])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = CREATE_SEGMENT;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_DELETE_SEGMENT(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	buffer->stream_size = strlen(instruccion[1])+1;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);
	memcpy(stream + offset, &instruccion[1], strlen(instruccion[1])+1);
	offset += strlen(instruccion[1])+1;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = DELETE_SEGMENT;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_YIELD(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = YIELD;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	recibir_contexto(fd_kernel, contexto);
}

void ejecutar_EXIT(char** instruccion, t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer* buffer;
	void* stream;
	int offset = 0;
	serializar_contexto(contexto, buffer, stream, offset);

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = EXIT;
	paquete->buffer = buffer;

	send(fd_kernel, stream, buffer->stream_size, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	recibir_contexto(fd_kernel, contexto);
}


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
	else if(strcmp(instruccion_parseada[0], "F_OPEN") == 0){
		ejecutar_F_OPEN(instruccion_parseada, contexto, fd_kernel);
	}
	else if(strcmp(instruccion_parseada[0], "F_CLOSE") == 0){
		ejecutar_F_CLOSE(instruccion_parseada, contexto, fd_kernel);
	}
	else if(strcmp(instruccion_parseada[0], "F_SEEK") == 0){
			ejecutar_F_SEEK(instruccion_parseada, contexto, fd_kernel);
		}
	else if(strcmp(instruccion_parseada[0], "F_READ") == 0){
			ejecutar_F_READ(instruccion_parseada, contexto, fd_kernel, fd_memoria, config);
		}
	else if(strcmp(instruccion_parseada[0], "F_WRITE") == 0){
			ejecutar_F_WRITE(instruccion_parseada, contexto, fd_kernel, fd_memoria, config);
		}
	else if(strcmp(instruccion_parseada[0], "F_TRUNCATE") == 0){
			ejecutar_F_TRUNCATE(instruccion_parseada, contexto, fd_kernel);
		}
	else if(strcmp(instruccion_parseada[0], "WAIT") == 0){
			ejecutar_WAIT(instruccion_parseada, contexto, fd_kernel);
		}
	else if(strcmp(instruccion_parseada[0], "SIGNAL") == 0){
			ejecutar_SIGNAL(instruccion_parseada, contexto, fd_kernel);
		}
	else if(strcmp(instruccion_parseada[0], "CREATE_SEGMENT") == 0){
			ejecutar_CREATE_SEGMENT(instruccion_parseada, contexto, fd_kernel);
		}
	else if(strcmp(instruccion_parseada[0], "DELETE_SEGMENT") == 0){
				ejecutar_DELETE_SEGMENT(instruccion_parseada, contexto, fd_kernel);
			}
	else if(strcmp(instruccion_parseada[0], "YIELD") == 0){
				ejecutar_YIELD(instruccion_parseada, contexto, fd_kernel);
			}
	else if(strcmp(instruccion_parseada[0], "EXIT") == 0){
				ejecutar_EXIT(instruccion_parseada, contexto, fd_kernel);
			}
}
