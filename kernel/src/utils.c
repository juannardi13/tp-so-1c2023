#include"../include/utils.h"

t_log* logger;
t_config* config;

int iniciar_servidor(t_log* logger, const char* name, char* ip, char* puerto) {
    int socket_servidor;
    struct addrinfo hints, *servinfo;

    // Inicializando hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Recibe los addrinfo
    getaddrinfo(ip, puerto, &hints, &servinfo);

    bool conecto = false;

    // Itera por cada addrinfo devuelto
    for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next) {
        socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_servidor == -1) // fallo de crear socket
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            // Si entra aca fallo el bind
            close(socket_servidor);
            continue;
        }
        // Ni bien conecta uno nos vamos del for
        conecto = true;
        break;
    }

    if(!conecto) {
        free(servinfo);
        return 0;
    }

    listen(socket_servidor, SOMAXCONN); // Escuchando (hasta SOMAXCONN conexiones simultaneas)

    // Aviso al logger
    log_info(logger, "Escuchando en %s:%s (%s)\n", ip, puerto, name);

    freeaddrinfo(servinfo);

    return socket_servidor;
}

int esperar_cliente(t_log* logger, const char* name, int socket_servidor) {
    struct sockaddr_in dir_cliente;
    socklen_t tam_direccion = sizeof(struct sockaddr_in);
    log_info(logger, "Entro a la función esperar cliente");

    int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

    log_info(logger, "Cliente conectado (a %s)\n", name);

    return socket_cliente;
}

int recibir_operacion(t_log* logger, int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0){
		return cod_op;
		log_info(logger, "se realizo el handshake");
	}
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

void recibir_mensaje(t_log* logger, int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "%s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);

	return valores;
}



