#include"utils_cpu2.h"
#include <math.h>
#include<shared-2.h>

int convertirAEntero(char *cadena) {
    int resultado = 0;

    for (int i = 0; cadena[i] != '\0'; ++i) {
            resultado = resultado * 10 + (cadena[i] - '0');
    }
    return resultado;
}

void asignar_valor_a_registro(char *valor, char *registro, t_registros *registros) {

	if (strcmp(registro, "AX") == 0) {
		strcpy(registros_cpu.ax, valor);
	}
	else if (strcmp(registro, "BX") == 0) {
		strcpy(registros_cpu.bx, valor);
	}
	else if (strcmp(registro, "CX") == 0) {
		strcpy(registros_cpu.cx, valor);
	}
	else if (strcmp(registro, "DX") == 0) {
		strcpy(registros_cpu.dx, valor);
	}
	else if (strcmp(registro, "EAX") == 0) {
		strcpy(registros_cpu.eax, valor);
	}
	else if (strcmp(registro, "EBX") == 0) {
		strcpy(registros_cpu.ebx, valor);
	}
	else if (strcmp(registro, "ECX") == 0) {
		strcpy(registros_cpu.ecx, valor);
	}
	else if (strcmp(registro, "EDX") == 0) {
		strcpy(registros_cpu.edx, valor);
	}
	else if (strcmp(registro, "RAX") == 0) {
		strcpy(registros_cpu.rax, valor);
	}
	else if (strcmp(registro, "RBX") == 0) {
		strcpy(registros_cpu.rbx, valor);
	}
	else if (strcmp(registro, "RCX") == 0) {
		strcpy(registros_cpu.rcx, valor);
	}
	else if (strcmp(registro, "RDX") == 0) {
		strcpy(registros_cpu.rdx, valor);
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
	agregar_a_paquete(paquete, &valor, strlen(valor)+1); // No le estabamos pasando el valor que queriamos que escriba en la direccion fisica
	enviar_paquete(paquete, fd_memoria);
	eliminar_paquete(paquete);
}

char* mmu_valor_buscado(t_contexto_de_ejecucion* contexto, int direccion_logica, int fd_memoria, t_config* config){
	int direccion_fisica = obtener_direccion_fisica(direccion_logica, fd_memoria, config, contexto);
	char* valor;
	strncpy(valor, leer_de_memoria(direccion_fisica, config, fd_memoria), strlen(leer_de_memoria(direccion_fisica, config, fd_memoria)) + 1);
	return valor;
}
