#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void manejar_fopen(int socket_cliente)
{
	// ya saque cod op, me falta el size del buffer y el buffer

	int size_buffer;
	void* contenido_buffer = deserializo_buffer(&size_buffer,socket_cliente);

	// el paquete recibido tiene solo un string, por ende en el stream hay |strlen(stringEnviado) + 1 (por \n)|stringEnviado|
	int offset = 0;
	int tamanio_string_recibido = deserializo_int(contenido_buffer,&offset);
	char* string_recibido = deserializo_string(tamanio_string_recibido,contenido_buffer,&offset);

	abrir_archivo(string_recibido);
}

void manejar_fread(int socket_cliente)
{
	int size_buffer;
	void* contenido_buffer = recibir_buffer(&size_buffer,socket_cliente);

	// leer_archivo(char* nombre_archivo,int nro_byte_archivo,char* direccion_volcar_lectura,int cantidad_bytes_leer)

	int offset = 0;
	int tamanio_string_nombre_archivo = deserializar_int(contenido_buffer,&offset);
	char* string_nombre_archivo = deserializar_string(tamanio_string_nombre_archivo,contenido_buffer,&offset);
	int nro_byte_archivo = deserializar_int(contenido_buffer,&offset);
	int tamanio_string_representa_memoria = deserializar_int(contenido_buffer,&offset);
	char* direccion_fisica = deserializar_string(tamanio_string_representa_memoria,contenido_buffer,&offset);
	int cantidad_bytes_leer = deserializar_int(contenido_buffer,&offset);

	leer_archivo(string_nombre_archivo,nro_byte_archivo,direccion_fisica,cantidad_bytes_leer);
}

void manejar_fwrite(int socket_cliente)
{
	int size_buffer;
	void* contenido_buffer = recibir_buffer(&size_buffer,socket_cliente);

	// leer_archivo(char* nombre_archivo,int nro_byte_archivo,char* direccion_volcar_lectura,int cantidad_bytes_leer)

	int offset = 0;
	int tamanio_string_nombre_archivo = deserializar_int(contenido_buffer,&offset);
	char* string_nombre_archivo = deserializar_string(tamanio_string_nombre_archivo,contenido_buffer,&offset);
	int nro_byte_archivo = deserializar_int(contenido_buffer,&offset);
	int tamanio_string_representa_memoria = deserializar_int(contenido_buffer,&offset);
	char* direccion_fisica = deserializar_string(tamanio_string_representa_memoria,contenido_buffer,&offset);
	int cantidad_bytes_escribir = deserializar_int(contenido_buffer,&offset);

	leer_archivo(string_nombre_archivo,nro_byte_archivo,direccion_fisica,cantidad_bytes_escribir);
}

void manejar_ftruncate(int socket_cliente)
{
	int size_buffer;
	void* contenido_buffer = recibir_buffer(&size_buffer,socket_cliente);

	// leer_archivo(char* nombre_archivo,int nro_byte_archivo,char* direccion_volcar_lectura,int cantidad_bytes_leer)

	int offset = 0;
	int tamanio_string_nombre_archivo = deserializar_int(contenido_buffer,&offset);
	char* string_nombre_archivo = deserializar_string(tamanio_string_nombre_archivo,contenido_buffer,&offset);
	int tamanio_nuevo = deserializar_int(contenido_buffer,&offset);

	truncar_archivo(string_nombre_archivo,tamanio_nuevo);
}

void manejar_conexion(int socket_cliente){

	int codigo_operacion = recibir_operacion(socket_cliente);

	switch(codigo_operacion)
	{

	case F_OPEN:
			manejar_fopen(socket_cliente);
			break;

	case F_READ:
			manejar_fread(socket_cliente);
			break;

	case F_WRITE:
			manejar_fwrite(socket_cliente);
			break;

	case F_TRUNCATE:
			manejar_ftruncate(socket_cliente);
			break;

	default:
		log_warning(logger,"Operacion desconocida\n");
	}

}


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

