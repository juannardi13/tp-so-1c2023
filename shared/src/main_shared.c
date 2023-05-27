#include "../include/main_shared.h"


//----------------------------------ENVIO - RECIBO DE  PAQUETE/BUFFER/MENSAJE/OPERACION------------------------------------

// OPERACION

int recibir_operacion(int socket_cliente) {  // TP0
   int cod_op;
    if(recv(socket_cliente, &cod_op, sizeof(uint8_t), MSG_WAITALL) != 0)
        return cod_op;
    else
    {
        close(socket_cliente);
        return -1;
    }
}

int recibir_operacion_nuevo(int socket_cliente) {
   int cod_op = 0;
    if(recv(socket_cliente, &cod_op, sizeof(uint8_t), MSG_WAITALL) != 0)
        return cod_op;
    else
    {
        close(socket_cliente);
        return -1;
    }
}

// MENSAJE

void enviar_mensaje(char *mensaje, int socket_cliente) //TP0
{
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = MENSAJE;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->stream_size = strlen(mensaje) + 1;
    paquete->buffer->stream = malloc(paquete->buffer->stream_size);
    memcpy(paquete->buffer->stream, mensaje, paquete->buffer->stream_size);

    int bytes = paquete->buffer->stream_size + 2 * sizeof(int);

    void *a_enviar = serializar_paquete_con_bytes(paquete,bytes);

    send(socket_cliente, a_enviar, bytes, 0);

    free(a_enviar);
    eliminar_paquete(paquete);
}

void recibir_mensaje(int socket_cliente,t_log* logger) { //TP0
    int size;
    char* buffer = recibir_buffer(&size, socket_cliente);
    log_info(logger, "Me llego el mensaje %s", buffer);
    free(buffer);
}


 // BUFFER

void crear_buffer(t_paquete *paquete)
{
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->stream_size = 0;
    paquete->buffer->stream = NULL;
}

void *recibir_stream(int *size, int socket_cliente)
{
    void *stream;

    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    stream = malloc(*size);
    recv(socket_cliente, stream, *size, MSG_WAITALL);

    return stream;
}


// recibir buffer comun sin tamanio proceso

 void* recibir_buffer(int* size, int socket_cliente) {
     void * buffer;

     recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
     buffer = malloc(*size);
     recv(socket_cliente, buffer, *size, MSG_WAITALL);

     return buffer;
 }



void agregar_a_buffer(t_buffer *buffer, void *src, int size) {
	buffer->stream = realloc(buffer->stream, buffer->stream_size + size);
	memcpy(buffer->stream + buffer->stream_size, src, size);
	buffer->stream_size+=size;
}

t_buffer *inicializar_buffer_con_parametros(uint32_t size, void *stream) {
	t_buffer *buffer = (t_buffer *)malloc(sizeof(t_buffer));
	buffer->stream_size = size;
	buffer->stream = stream;
	return buffer;
}

// PAQUETE


void* serializar_paquete_con_bytes(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->stream_size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->stream_size);
	desplazamiento+= paquete->buffer->stream_size;

	return magic;
}

t_buffer* serializar_paquete(t_paquete* paquete) {
    t_buffer * magic = inicializar_buffer_con_parametros(0, NULL);

    agregar_a_buffer(magic, &(paquete->codigo_operacion), sizeof(uint8_t));
    agregar_a_buffer(magic, &(paquete->buffer->stream_size), sizeof(uint32_t));
    agregar_a_buffer(magic, paquete->buffer->stream, paquete->buffer->stream_size);

    return magic;
}

// paquete con codigo de operacion solo paquete

t_paquete *crear_paquete(void){ // TP0
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = PAQUETE;
    crear_buffer(paquete);
    return paquete;
}

//paquete con cualquier codigo de operacion

t_paquete *crear_paquete_consola(void){
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = PAQUETE_CONSOLA;
    crear_buffer(paquete);
    return paquete;
}


void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio_valor) {
    agregar_a_buffer(paquete->buffer, &tamanio_valor, sizeof(int)); // Para que no se envie el valor del tamanio del stream size
    agregar_a_buffer(paquete->buffer, valor, tamanio_valor);
}


void agregar_entero_a_paquete(t_paquete *paquete, int tamanio_proceso) // Agregar un entero a un paquete (ya creado)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->stream_size + sizeof(int));
	int size_tamanio = sizeof(int);
	memcpy(paquete->buffer->stream , &size_tamanio, sizeof(int));
    memcpy(paquete->buffer->stream , &tamanio_proceso, sizeof(int));
    paquete->buffer->stream_size += tamanio_proceso + sizeof(int);
}


void enviar_paquete(t_paquete *paquete, int socket_cliente) //TP0
{
    t_buffer *a_enviar = serializar_paquete(paquete);
    send(socket_cliente, a_enviar->stream, a_enviar->stream_size, 0);
    free(a_enviar->stream);
    free(a_enviar);
}

// Recibir paquete como lista

t_list *recibir_paquete(int socket_cliente)  // TP0
{
    int size;
    int desplazamiento = 0;
    void *stream;
    t_list *valores = list_create();
    int tamanio;

    stream = recibir_stream(&size, socket_cliente);

    while (desplazamiento < size)
    {
        memcpy(&tamanio, stream + desplazamiento, sizeof(int));
        desplazamiento += sizeof(int);
        char *valor = malloc(tamanio);
        memcpy(valor, stream + desplazamiento, tamanio);
        desplazamiento += tamanio;
        list_add(valores, valor);
    }
    free(stream);
    return valores;
}

// Recibir paquete como paquete

t_paquete* recibe_paquete(int socket){
	int size;
	void *stream;

	op_code codigo=recibir_operacion(socket);
	t_paquete* paquete=crear_paquete_consola();

	stream = recibir_stream(&size, socket);
	memcpy(&paquete->buffer->stream_size, &size, sizeof(int));
	paquete->buffer->stream = malloc(size);
	memcpy(paquete->buffer->stream, stream, size);

	free(stream);
	return paquete;

}

void eliminar_paquete(t_paquete* paquete) {
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}


// DATOS


//Envio y recibo de datos

int enviar_datos(int socket_fd, void *source, uint32_t size) {
	return send(socket_fd, source, size, 0);
}

int recibir_datos(int socket_fd, void *dest, uint32_t size) {
	return recv(socket_fd, dest, size, 0); // cuantos bytes a recibir y a donde los quiero recibir
}

int crear_conexion(char *ip, char *puerto)
{
    struct addrinfo hints;
    struct addrinfo *server_info;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    getaddrinfo(ip, puerto, &hints, &server_info);

    int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

    if (connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
    {
        printf("error");
    }

    //freeaddrinfo(server_info);

    return socket_cliente;
}


void liberar_conexion(int socket_cliente) {
    close(socket_cliente);
}
//SERVIDOR

int iniciar_servidor(char *ip, char *puerto)
{

    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, puerto, &hints, &servinfo);

    // Creamos el socket de escucha del servidor

    int socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    // Asociamos el socket a un puerto

    if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) != 0)
    {
        perror("Fallo el bind");
        printf("FALLO EL BIND LOSER\n\n");
        exit(1);
    }

    // Escuchamos las conexiones entrantes

    listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);
    //log_trace(logger, "Listo para escuchar a cliente");

    return socket_servidor;
}

int esperar_cliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (struct sockaddr *) &dir_cliente, &tam_direccion);
	if(socket_cliente == -1)
		return -1;

	return socket_cliente;
}
