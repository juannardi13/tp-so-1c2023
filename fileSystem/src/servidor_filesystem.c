#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void manejar_conexion(int socket_cliente){

	// un paquete es un codigo de un operación más un buffer
	// un buffer es un tamaño de buffer seguido de un stream con todos los apartados correspondientes al struct enviado

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	recv(socket_cliente,&(paquete->codigo_operacion),sizeof(int),0);
	recv(socket_cliente,(paquete->buffer->stream_size),sizeof(int),0);

	paquete->buffer->stream = malloc(paquete->buffer->stream_size);

	recv(socket_cliente,paquete->buffer->stream,paquete->buffer->stream_size,0);

	void* stream;

	switch(paquete->codigo_operacion)
	{

	case F_OPEN:
		log_info(logger,"File System recibe fopen");
		break;

	case F_CLOSE:
			log_info(logger,"File System recibe fclose");
			break;

	case F_SEEK:
			log_info(logger,"File System recibe fseek");
			break;

	case F_READ:
			log_info(logger,"File System recibe fread");
			break;

	case F_WRITE:
			log_info(logger,"File System recibe fwrite");
			break;

	case F_TRUNCATE:
			log_info(logger,"File System recibe ftruncate");
			break;

	default:
		log_warning(logger,"Operacion desconocida\n");
	}

}


int atender_clientes_file_system(int socket_fs){
	int socket_consola = esperar_cliente(logger, "FILE SYSTEM", socket_fs);

	int ret = 0;

	if(socket_consola != -1)
	{
		manejar_conexion(socket_consola);
		ret = 1;
	}
	else
	{
		log_error(logger,"Error al escuchar cliente... Finalizando servidor \n");
	}

	return ret;
}

void inicializar_servidor(){
	char* puerto_escucha = config_valores->puerto_escucha;
	int fd_file_system = iniciar_servidor(puerto_escucha);
	log_info(logger, "File System inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_escucha);

	while(atender_clientes_file_system(fd_file_system));



}

