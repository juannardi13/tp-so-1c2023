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

void msleep(int tiempo_microsegundos) {
	usleep(tiempo_microsegundos * 1000);
}

void asignar_valor_a_registro(char *valor, char *registro) {

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

char* obtener_valor_de_registro(char *registro) {

	char* ret;

	if (strcmp(registro, "AX") == 0) {
		ret = malloc(5);
		strcpy(ret,registros_cpu.ax);
	}
	else if (strcmp(registro, "BX") == 0) {
		ret = malloc(5);
		strcpy(ret,registros_cpu.bx);
	}
	else if (strcmp(registro, "CX") == 0) {
		ret = malloc(5);
		strcpy(ret,registros_cpu.cx);
	}
	else if (strcmp(registro, "DX") == 0) {
		ret = malloc(5);
		strcpy(ret,registros_cpu.dx);
	}
	else if (strcmp(registro, "EAX") == 0) {
		ret = malloc(9);
		strcpy(ret,registros_cpu.eax);
	}
	else if (strcmp(registro, "EBX") == 0) {
		ret = malloc(9);
		strcpy(ret,registros_cpu.ebx);
	}
	else if (strcmp(registro, "ECX") == 0) {
		ret = malloc(9);
		strcpy(ret,registros_cpu.ecx);
	}
	else if (strcmp(registro, "EDX") == 0) {
		ret = malloc(9);
		strcpy(ret,registros_cpu.edx);
	}
	else if (strcmp(registro, "RAX") == 0) {
		ret = malloc(17);
		strcpy(ret,registros_cpu.rax);
	}
	else if (strcmp(registro, "RBX") == 0) {
		ret = malloc(17);
		strcpy(ret,registros_cpu.rbx);
	}
	else if (strcmp(registro, "RCX") == 0) {
		ret = malloc(17);
		strcpy(ret,registros_cpu.rcx);
	}
	else if (strcmp(registro, "RDX") == 0) {
		ret = malloc(17);
		strcpy(ret,registros_cpu.rdx);
	}

	return ret;
}



void activar_segmentation_fault(t_contexto_de_ejecucion* contexto, int fd_kernel){
	t_buffer *buffer = malloc(sizeof(t_buffer));
	t_paquete *paquete = malloc(sizeof(t_paquete));

	int tamanio_instrucciones = contexto->tamanio_instrucciones;
	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;
	int cantidad_segmentos = list_size(contexto->tabla_segmentos);

	buffer->stream_size = sizeof(int) * 3 //PID, PC, TAMANIO_INSTRUCCIONES
	//		+ sizeof(t_registros)
	//		+ tamanio_segmentos
	+ tamanio_instrucciones
	+ tamanio_registro_chico * 4
	+ tamanio_registro_mediano * 4
	+ tamanio_registro_grande * 4
	+ sizeof(int)
	+ (cantidad_segmentos * 3 * sizeof(int));;

	void *stream = malloc(buffer->stream_size);
	int offset = 0;
	//	serializar_contexto(contexto, buffer, stream, offset);

	memcpy(stream + offset, &(contexto->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->pc), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(contexto->tamanio_instrucciones), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, contexto->instrucciones, tamanio_instrucciones);
	offset += tamanio_instrucciones;

	// SERIALIZAR DESPUÉS LOS REGISTROS
	memcpy(stream + offset, &(registros_cpu.ax), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.bx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.cx), tamanio_registro_chico);
	offset += tamanio_registro_chico;
	memcpy(stream + offset, &(registros_cpu.dx), tamanio_registro_chico);
	offset += tamanio_registro_chico;

	memcpy(stream + offset, &(registros_cpu.eax), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ebx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.ecx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;
	memcpy(stream + offset, &(registros_cpu.edx), tamanio_registro_mediano);
	offset += tamanio_registro_mediano;

	memcpy(stream + offset, &(registros_cpu.rax), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rbx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rcx), tamanio_registro_grande);
	offset += tamanio_registro_grande;
	memcpy(stream + offset, &(registros_cpu.rdx), tamanio_registro_grande);
	offset += tamanio_registro_grande;

	//Serialización de segmentos
	memcpy(stream + offset, &(cantidad_segmentos), sizeof(int));
	offset += sizeof(int);

	t_list_iterator* iterador = list_iterator_create(contexto->tabla_segmentos);

	while(list_iterator_has_next(iterador)) {
		t_segmento* aux = (t_segmento*) list_iterator_next(iterador);

		memcpy(stream + offset, &(aux->base), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset,&(aux->id), sizeof(int));
		offset += sizeof(int);
		memcpy(stream + offset, &(aux->tamanio), sizeof(int));
		offset += sizeof(int);
	}

	list_iterator_destroy(iterador);

	buffer->stream = stream;

	paquete->codigo_operacion = SEG_FAULT;
	paquete->buffer = buffer;

	void *a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_kernel, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

bool desplazamiento_supera_tamanio(int desplazamiento, char* valor){
	int tamanio_valor = strlen(valor)+1;
	return desplazamiento > tamanio_valor;
}

int obtener_direccion_fisica(int direccion_logica, int fd_memoria, t_config* config, t_contexto_de_ejecucion* contexto, t_log* logger_principal, int fd_kernel, int tamanio_valor) {
	int tamanio_segmento =  config_get_int_value(config, "TAM_MAX_SEGMENTO");
	//int numero_segmento = floor((float)contexto->segmentos->base / (float)tamanio_segmento);
	int numero_segmento = floor((double) direccion_logica / (double) tamanio_segmento);
	int desplazamiento_segmento = direccion_logica % tamanio_segmento;

//	t_segmento* segmento_buscado = list_get(contexto->tabla_segmentos, numero_segmento);

	bool mismo_id(t_segmento *seg) {
		return seg->id == numero_segmento;
	}

	t_segmento *segmento_buscado = list_find(contexto->tabla_segmentos, (void *) mismo_id);

	int direccion_fisica;


	if (segmento_buscado == NULL) {
		log_info(logger_principal, "PID: <%d> - Error SEG_FAULT- Segmento: <%d> - Offset: <%d> - Tamaño: <%d>", contexto->pid, numero_segmento, desplazamiento_segmento, tamanio_segmento);
		activar_segmentation_fault(contexto, fd_kernel);
		direccion_fisica = -1;
	} else {
		int tamanio_segmento_buscado = segmento_buscado->tamanio;

		if(desplazamiento_segmento + tamanio_valor > tamanio_segmento_buscado) {
			log_info(logger_principal, "PID: <%d> - Error SEG_FAULT- Segmento: <%d> - Offset: <%d> - Tamaño: <%d>", contexto->pid, numero_segmento, desplazamiento_segmento, tamanio_segmento);
			activar_segmentation_fault(contexto, fd_kernel);
			direccion_fisica = -1;
		} else {
			direccion_fisica = segmento_buscado->base + desplazamiento_segmento;
		}
	}

	return direccion_fisica;
}

void escribir_en_memoria(int direccion_fisica, char* valor, int fd_memoria, t_log* logger_principal, t_contexto_de_ejecucion* contexto, int direccion_logica, t_config* config) {
	int tamanio_segmento =  config_get_int_value(config, "TAM_MAX_SEGMENTO");
	//int numero_segmento = floor((float)contexto->segmentos->base / (float)tamanio_segmento);
	int numero_segmento = floor(direccion_logica / tamanio_segmento);

	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));

	int tamanio_valor = strlen(valor) + 1;

	buffer->stream_size = sizeof(int) * 2 //ESTE ES EL TAMAÑO DE DIRECCION_FISICA Y EL DE TAMANIO_VALOR
			+ tamanio_valor;

	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, &direccion_fisica, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &tamanio_valor, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, valor, tamanio_valor);
	offset += tamanio_valor;

	buffer->stream = stream;

	paquete->codigo_operacion = ESCRIBIR_EN_MEMORIA;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_memoria, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);


	free(a_enviar);
	eliminar_paquete(paquete);

	op_code respuesta_memoria;

	recv(fd_memoria, &respuesta_memoria, sizeof(op_code), MSG_WAITALL);

	switch(respuesta_memoria) {
	case OK_VALOR_ESCRITO:
		log_info(logger_principal, "PID: <%d> - Acción: <ESCRIBIR> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", contexto->pid, numero_segmento, direccion_fisica, valor);
		break;
	default:
		log_error(logger_principal, "[ERROR] Arreglate con Memoria loco suerte.");
		break;
	}

}

char* mmu_valor_buscado(t_contexto_de_ejecucion* contexto, int direccion_logica, int tamanio, int fd_memoria, t_config* config, t_log* logger_principal, int fd_kernel, int direccion_fisica) {

	int tamanio_segmento =  config_get_int_value(config, "TAM_MAX_SEGMENTO");
	//int numero_segmento = floor((float)contexto->segmentos->base / (float)tamanio_segmento);
	int numero_segmento = floor(direccion_logica / tamanio_segmento);

	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_paquete* paquete = malloc(sizeof(t_paquete));

	buffer->stream_size = sizeof(int) //tamanio_a_leer
			+ sizeof(int);			  //dirección_física

	void* stream = malloc(buffer->stream_size);
	int offset = 0;

	memcpy(stream + offset, &direccion_fisica, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &tamanio, sizeof(int));
	offset += sizeof(int);

	log_info(logger_principal, "Quiero leer en la dirección <%d> un total de <%d>", direccion_fisica, tamanio);

	buffer->stream = stream;
	paquete->codigo_operacion = LEER_DE_MEMORIA;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_memoria, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);
	eliminar_paquete(paquete);

	t_paquete* paquete_respuesta = malloc(sizeof(t_paquete));
	paquete_respuesta->buffer = malloc(sizeof(t_buffer));

	recv(fd_memoria, &(paquete_respuesta->codigo_operacion), sizeof(int), MSG_WAITALL);
	recv(fd_memoria, &(paquete_respuesta->buffer->stream_size), sizeof(int), 0);
	paquete_respuesta->buffer->stream = malloc(paquete_respuesta->buffer->stream_size);
	recv(fd_memoria, paquete_respuesta->buffer->stream, paquete_respuesta->buffer->stream_size, 0);

	void* stream_respuesta = paquete_respuesta->buffer->stream;
	int tamanio_valor_memoria;

	memcpy(&tamanio_valor_memoria, stream_respuesta, sizeof(int));
	stream_respuesta += sizeof(int);

	char* valor = malloc(tamanio_valor_memoria);
	memset(valor, 0, tamanio_valor_memoria);

	memcpy(valor, stream_respuesta, tamanio_valor_memoria);
	stream_respuesta += tamanio_valor_memoria;


	/*switch(paquete_respuesta->codigo_operacion) {
		case LEIDO :
			void* stream_respuesta = paquete_respuesta->buffer->stream;
			int tamanio_valor_memoria;

			memcpy(&tamanio_valor_memoria, stream_respuesta, sizeof(int));
			stream_respuesta += sizeof(int);

			char* valor = malloc(tamanio_valor_memoria);
			memset(valor, 0, tamanio_valor_memoria);

			memcpy(valor, stream_respuesta, tamanio_valor_memoria);
			stream_respuesta += tamanio_valor_memoria;


			break;
		case NO_LEIDO :
			perror("No se pudo leer el valor del registro dado");
			valor = malloc(6);
			strcpy(valor, "ERROR");
			break;
		default :
			perror("Operacion desconocida");
			valor = malloc(6);
			strcpy(valor, "ERROR");
			break;
	}*/

	eliminar_paquete(paquete_respuesta);

	log_info(logger_principal, "PID: <%d> - Acción: <LEER> - Segmento: <%d> - Dirección Física: <%d> - Valor: <%s>", contexto->pid, numero_segmento, direccion_fisica, valor);

	return valor;
}
//------LA FUNCION LEER_DE_MEMORIA() Y MMU_VALOR_BUSCADO CUMPLÍAN LA MISMA FUNCIÓN, DECIDÍ QUEDARME CON LA DE ARRIBA

char* leer_de_memoria(int direccion_fisica, t_config* config, int fd_memoria, int numero_segmento){

	t_buffer* buffer = malloc(sizeof(t_buffer));
	t_paquete* paquete = malloc(sizeof(t_paquete));

	buffer->stream_size = sizeof(int);

	void* stream = malloc(sizeof(buffer->stream_size));
	int offset = 0;

	memcpy(stream + offset, &direccion_fisica, sizeof(int));
	offset += sizeof(int);

	buffer->stream = stream;

	paquete->codigo_operacion = LEER_DE_MEMORIA;
	paquete->buffer = buffer;

	void* a_enviar = malloc(buffer->stream_size + sizeof(int) + sizeof(int));
	int desplazamiento = 0;

	agregar_a_stream(a_enviar, &desplazamiento, (&paquete->codigo_operacion), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, (&paquete->buffer->stream_size), sizeof(int));
	agregar_a_stream(a_enviar, &desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);

	send(fd_memoria, a_enviar, buffer->stream_size + sizeof(int) + sizeof(int), 0);

	free(a_enviar);

	op_code respuesta_memoria;
	int tamanio_respuesta;

	recv(fd_memoria, &respuesta_memoria, sizeof(op_code), MSG_WAITALL);
	recv(fd_memoria, &tamanio_respuesta, sizeof(int), 0);
	void* recibido = malloc(tamanio_respuesta);
	recv(fd_memoria, recibido, tamanio_respuesta, 0);

	char* valor;
	switch(respuesta_memoria){
		case LEIDO :
			int tamanio_valor_memoria;
			memcpy(&tamanio_valor_memoria, recibido, sizeof(int));
			recibido += sizeof(int);

			valor = malloc(tamanio_valor_memoria);
			memset(valor, 0, tamanio_valor_memoria);

			memcpy(valor, recibido, tamanio_valor_memoria);
			recibido += tamanio_valor_memoria;

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
int tamanio_registro(char* registro){
	int tam = 0;

	if(strcmp(registro, "AX") == 0 || strcmp(registro, "BX") == 0 || strcmp(registro, "CX") == 0 ||strcmp(registro, "DX") == 0) {
		tam = 5;
	}

	if(strcmp(registro, "EAX") == 0 || strcmp(registro, "EBX") == 0 || strcmp(registro, "ECX") == 0 ||strcmp(registro, "EDX") == 0) {
		tam = 9;
	}

	if(strcmp(registro, "RAX") == 0 || strcmp(registro, "RBX") == 0 || strcmp(registro, "RCX") == 0 ||strcmp(registro, "RDX") == 0) {
		tam = 17;
	}

	return tam;
}
