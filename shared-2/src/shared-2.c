#include "shared-2.h"

void hola(void) {
	printf("Hola!");
}

int iniciar_servidor(char *puerto)
{

    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, puerto, &hints, &servinfo);

    int socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

    if (bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen) != 0)
    {
        perror("Fallo el bind");
        printf("FALLO EL BIND LOSER\n\n");
        exit(1);
    }

    listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    return socket_servidor;
}

int esperar_cliente(t_log* logger, const char* name, int socket_servidor) {
    struct sockaddr_in dir_cliente;
    socklen_t tam_direccion = sizeof(struct sockaddr_in);

    int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

    log_info(logger, "Cliente conectado (a %s)\n", name);

    return socket_cliente;
}

int esperar_cliente_alt(int socket_servidor) {
//    struct sockaddr_in dir_cliente;
//    socklen_t tam_direccion = sizeof(struct sockaddr_in);

    int socket_cliente = accept(socket_servidor, NULL, NULL);

    return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

char* recibir_mensaje(t_log* logger, int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "%s", buffer);
	return buffer;
}

t_paquete *crear_paquete_consola(void){
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = PAQUETE_CONSOLA;
    crear_buffer(paquete);
    return paquete;
}

void crear_buffer(t_paquete *paquete)
{
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->stream_size = 0;
    paquete->buffer->stream = NULL;
}

void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio_valor) {
    agregar_a_buffer(paquete->buffer, &tamanio_valor, sizeof(int)); // Para que no se envie el valor del tamanio del stream size
    agregar_a_buffer(paquete->buffer, valor, tamanio_valor);
}

void agregar_string_a_paquete(t_paquete* paquete, void* valor, int size) {
    paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->stream_size + size + sizeof(int));

    memcpy(paquete->buffer->stream + paquete->buffer->stream_size, &size, sizeof(int));
    memcpy(paquete->buffer->stream + paquete->buffer->stream_size + sizeof(int), valor, size);

    paquete->buffer->stream_size += size + sizeof(int);
}

void agregar_a_buffer(t_buffer *buffer, void *src, int size) {
	buffer->stream = realloc(buffer->stream, buffer->stream_size + size);
	memcpy(buffer->stream + buffer->stream_size, src, size);
	buffer->stream_size+=size;
}

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

void eliminar_paquete(t_paquete* paquete) {
    free(paquete->buffer->stream);
    free(paquete->buffer);
    free(paquete);
}

void *recibir_stream(int *size, int socket_cliente)
{
    void *stream;

    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    stream = malloc(*size);
    recv(socket_cliente, stream, *size, MSG_WAITALL);

    return stream;
}

t_buffer *inicializar_buffer_con_parametros(uint32_t size, void *stream) {
	t_buffer *buffer = (t_buffer *)malloc(sizeof(t_buffer));
	buffer->stream_size = size;
	buffer->stream = stream;
	return buffer;
}

void* serializar_paquete(t_paquete* paquete, int bytes)
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

t_paquete *crear_paquete(int codigo){
    t_paquete *paquete = malloc(sizeof(t_paquete));

    paquete->codigo_operacion = codigo;
    crear_buffer(paquete);
    return paquete;
}

void agregar_entero_a_paquete(t_paquete *paquete, int tamanio_proceso) // Agregar un entero a un paquete (ya creado)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->stream_size + sizeof(int));
	int size_tamanio = sizeof(int);
	memcpy(paquete->buffer->stream , &size_tamanio, sizeof(int));
    memcpy(paquete->buffer->stream , &tamanio_proceso, sizeof(int));
    paquete->buffer->stream_size += tamanio_proceso + sizeof(int);
}

void agregar_int_a_paquete(t_paquete *paquete, int valor) {

    paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->stream_size + sizeof(int));
    memcpy(paquete->buffer->stream + paquete->buffer->stream_size, &valor, sizeof(int));

    paquete->buffer->stream_size += sizeof(int);
}


void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->stream_size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

t_list *recibir_paquete(int socket_cliente)
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
        free(valor);
    }
    free(stream);
    return valores;
}

// Recibir paquete como paquete

t_paquete* recibe_paquete(int socket){
	int size;
	void *stream;

	//op_code codigo=recibir_operacion(socket);
	t_paquete* paquete=crear_paquete_consola();

	stream = recibir_stream(&size, socket);
	memcpy(&paquete->buffer->stream_size, &size, sizeof(int));
	paquete->buffer->stream = malloc(size);
	memcpy(paquete->buffer->stream, stream, size);

	free(stream);
	return paquete;

}

int enviar_datos(int socket_fd, void *source, uint32_t size) {
	return send(socket_fd, source, size, 0);
}

int recibir_datos(int socket_fd, void *dest, uint32_t size) {
	return recv(socket_fd, dest, size, 0); // cuantos bytes a recibir y a donde los quiero recibir
}

int crear_conexion(t_log* logger, const char* name, char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		perror("\nNo se ha podido conectar");
		exit(1);
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

int crear_conexion_alt(char *ip, char* puerto) {
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		perror("No se ha podido conectar");
		exit(1);
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

void liberar_conexion(int socket_cliente) {
    close(socket_cliente);
}

int bits_to_bytes(int bits) {

	return bits / 8;
}

int deserializar_int(void *buffer, int *offset) {

	int aux;

	memcpy(&aux, buffer + (*offset), sizeof(int));
	(*offset) += sizeof(int);

	return aux;
}

void* deserializar_string(int size, void *buffer, int *offset) {

    void *aux;

    aux = malloc(size);
    memcpy(aux, buffer + (*offset), size);
    (*offset) += size;

    return aux;
}

void msleep(int tiempo_microsegundos) {
	usleep(tiempo_microsegundos * 1000);
}

void agregar_segmento_a_paquete(t_paquete *paquete, t_segmento *seg) {

	agregar_int_a_paquete(paquete, seg->id);
	agregar_int_a_paquete(paquete, seg->base);
	agregar_int_a_paquete(paquete, seg->tamanio);
}

void agregar_tabla_a_paquete(t_paquete *paquete, t_tabla_segmentos *tabla) {

	int tam_lista = list_size(tabla->segmentos);

	agregar_int_a_paquete(paquete, tabla->pid);
	agregar_int_a_paquete(paquete, tam_lista);

	for (int i = 0; i < tam_lista; i++) {
		t_segmento *seg = list_get(tabla->segmentos, i);
		agregar_segmento_a_paquete(paquete, seg);
	}

}

void send_op(int socket_cliente, int cod_op) {

	void *a_enviar = malloc(sizeof(int));
	memcpy(a_enviar, &cod_op, sizeof(int));
	send(socket_cliente, a_enviar, sizeof(int), 0);

	free(a_enviar);
}

t_segmento *crear_segmento(int id, int base, int size) {

	t_segmento *seg = malloc(sizeof(t_segmento));

	seg->id   	  = id;
	seg->base 	  = base;
	seg->tamanio  = size;

	return seg;
}
