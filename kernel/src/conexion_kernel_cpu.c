#include"../include/utils_kernel.h"
#include"../../shared/include/main_shared.h"
/*
typedef enum {
	NEW,
	READY,
	EXEC,
	BLOCK,
	EXIT
}t_estado

typedef struct {
	char* ax;
	char* bx;
	char* cx;
	char* dx;
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

// Serializar y enviar paquete - revisar

t_paquete* serializar_instrucciones(char** lista_instrucciones){
t_paquete* paquete = malloc(sizeof(paquete));
paquete->buffer = malloc(sizeof(t_buffer));

int tamanio_lista = string_length(lista_instrucciones);
int offset = 0;

int tamanio_buffer = tamanio_lista + sizeof(int) + 1;
paquete->buffer->size = tamanio_buffer;
void* stream = malloc(tamanio_buffer);

memcpy(stream + offset, &tamanio_lista, sizeof(int));
offset += sizeof(int);
memcpy(stream + offset, lista_instrucciones, tamanio_lista + 1);

paquete->buffer->stream = stream;

return paquete;
}


t_paquete* serializar_contexto_de_ejecucion(t_contexto_de_ejecucion contexto){
t_paquete* paquete = malloc(sizeof(paquete));
paquete->buffer = malloc(sizeof(t_buffer));

int tamanio_lista = sizeof(contexto.instrucciones);
int offset = 0;
int tamanio_pid = sizeof(contexto.pid);
int tamanio_pc = sizeof(contexto.pc);
int tamnio_estado = sizeof(contexto.estado);
int tamanio_registros = sizeof(contexto.registros_pcb);

int tamanio_buffer = tamanio_lista + tamanio_pid + tamanio_pc + tamanio_estado + tamanio_registros + sizeof(int) + 1;
paquete->buffer->size = tamanio_buffer;
void* stream = malloc(tamanio_buffer);

memcpy(stream + offset, &tamanio_lista, sizeof(int));
offset += sizeof(int);
memcpy(stream + offset, lista_instrucciones, tamanio_lista + 1);

paquete->buffer->stream = stream;

return paquete;
}


// SERIALIZAR CONTEXTO DE EJECUCION v2. Agregue los tamanios directamente en la estructura

t_paquete* paquete_contexto_de_ejecucion(t_contexto_de_ejecucion contexto){

contexto->tamanio_instrucciones = sizeof(contexto->instrucciones); //con esto creo que hay un problema porque tendriamos que usar  list_size()
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

void enviar_instrucciones(char** lista_instrucciones, int fd_cpu){
t_paquete* paquete_a_enviar = serializar_paquete(lista_instrucciones);
enviar_paquete(paquete, fd_cpu);
free(paquete);
eliminar_paquete(paquete);
*/
