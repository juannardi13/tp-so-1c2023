#include"utils_cpu2.h"
#include <math.h>
#include<shared-2.h>

void serializar_segmentos(t_segmento* segmento_actual, void* stream, int offset){
	memcpy(stream + offset, &segmento_actual->base, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &segmento_actual->id, sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &segmento_actual->tamanio, sizeof(int));
	offset += sizeof(int);
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

/*//TODO comento las funciones para que no molesten los warnings
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

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

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
}*/
