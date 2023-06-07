//#include"utils_kernel2.h"
//#include<shared-2.h>
/*
typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXT //Queda como EXT porque si no tiene conflicto con el tipo de la instruccion.
}t_estado

// agrego registros faltantes
typedef struct {
	char* ax;
	char* bx;
	char* cx;
	char* dx;
	char* eax;
	char* ebx;
	char* ecx;
	char* edx;
	char* rax;
	char* rbx;
	char* rcx;
	char* rdx;
}t_registros;

typedef struct {
	int pid;
	t_list instrucciones;
	int pc;
	t_registros registros_pcb;
}t_pcb_cpu;

t_pcb_cpu pcb_en_ejecucion;

typedef struct {
	int pid;
	uint32_t tamanio_pid;
	t_list instrucciones;
	uint32_t tamanio_instrucciones;
	int pc;
	uint32_t tamanio_pc;
	t_registros registros_pcb;
	uint32_t tamanio_registros_pcb;
	t_estado estado;
	uint32_t tamanio_estado
}t_contexto_de_ejecucion

// SERIALIZAR CONTEXTO DE EJECUCION v2. VERSION OK

t_paquete* paquete_contexto_de_ejecucion(t_contexto_de_ejecucion contexto){

	contexto->tamanio_instrucciones = list_size(contexto->instrucciones);
	contexto->tamanio_pid = sizeof(contexto->pid);
	contexto->tamanio_pc = sizeof(contexto->pc);
	contexto->tamnio_estado = sizeof(contexto->estado);
	contexto->tamanio_registros = sizeof(contexto->registros_pcb);

	t_paquete *paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = CONTEXTO_EJECUCION;
	crear_buffer(paquete);
	paquete->buffer->stream_size = contexto->tamanio_instrucciones
								+ contexto->tamanio_pid
								+ contexto->tamanio_pc
								+ contexto->tamnio_estado
								+ contexto->tamanio_registros_pcb;

	void* stream = malloc(paquete->buffer->stream_size);

	int offset = 0;

	memcpy(stream + offset, &(contexto->pid), contexto->tamanio_pid);
	offset += contexto->tamanio_pid
			memcpy(stream + offset, &(contexto->instrucciones), contexto->tamanio_instrucciones);
	offset += contexto->tamanio_intrucciones;
	memcpy(stream + offset, &(contexto->pc), contexto->tamanio_pc);
	offset += contexto->tamanio_pc;
	memcpy(stream + offset, &(contexto->registros_pcb), contexto->tamanio_registros_pcb);
	offset += contexto->tamanio_registros_pcb;
	memcpy(stream + offset, &(contexto->estado), contexto->tamanio_estado);

	paquete->buffer->stream = stream;

	return paquete;
}

 No se usa por ahora
char** deserializar_instrucciones(t_buffer* buffer){
char** lista_instrucciones;
void* stream = buffer->stream;

int tamanio_lista;

memcpy(&tamanio_lista, stream, sizeof(int));
stream += sizeof(int);
lista_instrucciones = malloc(tamanio_lista);
memcpy(lista_instrucciones, stream, tamanio_lista);
return lista_instrucciones;
}
*/
/*
// ENVIA CONTEXTO CPU A KERNEL, ver como implementar en ejecutar_instruccion
void enviar_contexto(t_contexto_de_ejecucion contexto, int fd_kernel){
	t_paquete* paquete_a_enviar = paquete_contexto_de_ejecucion(contexto);
	enviar_paquete(paquete, fd_kernel);
	free(paquete);
	eliminar_paquete(paquete);
}*/

