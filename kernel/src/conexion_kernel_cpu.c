#include"../include/utils_kernel.h"
#include"../../shared/include/main_shared.h"
#include"../../shared/src/main_shared.c"

// Serializar y enviar paquete - revisar
typedef struct {
uint32_t size;
void* stream;
} t_buffer;

typedef {
uint8_t codigo_operacion
t_buffer* buffer;
} t_paquete;


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
