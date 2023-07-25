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
	void* contenido_buffer = recibir_buffer(&size_buffer,socket_cliente);

	// el paquete recibido tiene solo un string, por ende en el stream hay |strlen(stringEnviado) + 1 (por \n)|stringEnviado|
	int offset = 0;
	int tamanio_string_recibido = deserializar_int(contenido_buffer,&offset);
	char* string_recibido = deserializar_string(tamanio_string_recibido,contenido_buffer,&offset);

	int rta_para_kernel = abrir_archivo(string_recibido);

	op_code rta;

	if(rta_para_kernel)
	{
		printf("Existe\n");
		rta = EXISTE;
	}
	else
	{
		printf("No Existe\n");
		rta = NO_EXISTE;
	}

	log_info(logger,"Abrir Archivo: <%s>",string_recibido);

	send(socket_cliente,&rta,sizeof(op_code),0);
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
	int direccion_fisica = deserializar_int(contenido_buffer,&offset);
	int cantidad_bytes_leer = deserializar_int(contenido_buffer,&offset);

	char* direccion_fisica_contenido = malloc(cantidad_bytes_leer);

//	direccion_fisica_contenido = leer_archivo(string_nombre_archivo,nro_byte_archivo,cantidad_bytes_leer);

	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream_size = sizeof(uint32_t) + cantidad_bytes_leer;

	void* stream = malloc(buffer->stream_size);
	offset = 0;

	memcpy(stream + offset,&cantidad_bytes_leer,sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset,direccion_fisica_contenido,cantidad_bytes_leer);

	paquete->codigo_operacion = ESCRIBIR_EN_MEMORIA;
	paquete->buffer = buffer;

	int tam_a_enviar = sizeof(uint8_t) + sizeof(uint32_t) + paquete->buffer->stream_size;
	void* a_enviar = malloc(tam_a_enviar);
	offset = 0;

	memcpy(a_enviar + offset,&(paquete->codigo_operacion),sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(a_enviar + offset,&(paquete->buffer->stream_size),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset,paquete->buffer->stream,paquete->buffer->stream_size);

	int socket_memoria = config_valores.puerto_memoria;

	send(socket_memoria,a_enviar,tam_a_enviar,0);

	op_code respuesta_memoria;

	recv(socket_memoria,&respuesta_memoria,sizeof(op_code),MSG_WAITALL);

	op_code respuesta_a_kernel;

	if(respuesta_memoria == ESCRITURA_OK)
	{
		respuesta_a_kernel = FREAD_OK;
	}
	else
	{
		log_info(logger,"Error en la escritura en la direccion de memoria %i",direccion_fisica);
	}

	send(socket_cliente,&respuesta_a_kernel,sizeof(op_code),0);

	log_info(logger,"Leer Archivo: <%s> - Puntero <%i> - Memoria <%i> - Tamaño <%i>",string_nombre_archivo,nro_byte_archivo,direccion_fisica,cantidad_bytes_leer);

	free(direccion_fisica_contenido);
	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
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
	int direccion_fisica = deserializar_int(contenido_buffer,&offset);
	int cantidad_bytes_escribir = deserializar_int(contenido_buffer,&offset);

	char* direccion_fisica_contenido = malloc(cantidad_bytes_escribir);

	// tengo que mandar a leer de memoria de la direccion fisica

	t_paquete* paquete = malloc(sizeof(t_paquete));
	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->stream_size = sizeof(uint32_t);
	void* stream = malloc(buffer->stream_size);

	memcpy(stream,&direccion_fisica,sizeof(uint32_t));

	paquete->codigo_operacion = LEER_DE_MEMORIA;
	paquete->buffer = buffer;

	int tam_a_enviar = sizeof(uint8_t) + sizeof(uint32_t) + paquete->buffer->stream_size;
	void* a_enviar = malloc(tam_a_enviar);
	offset = 0;

	memcpy(a_enviar,&(paquete->codigo_operacion),sizeof(uint8_t));
	offset += sizeof(uint8_t);
	memcpy(a_enviar,&(paquete->buffer->stream_size),sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar,paquete->buffer->stream,paquete->buffer->stream_size);

	int socket_memoria = config_valores.puerto_memoria;

	send(socket_memoria,a_enviar,tam_a_enviar,0);

	// no aclara si la memoria devuelve un paquete, por ahora supongo que no

	recv(socket_memoria,direccion_fisica_contenido,cantidad_bytes_escribir,MSG_WAITALL);

	escribir_archivo(string_nombre_archivo,nro_byte_archivo,direccion_fisica_contenido,cantidad_bytes_escribir);
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

//	truncar_archivo(string_nombre_archivo,tamanio_nuevo);
}

void manejar_conexion(int socket_cliente){

	while(1){

		int codigo_operacion = recibir_operacion(socket_cliente);

//		printf("%i\n",codigo_operacion);

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

		case -1:
			log_warning(logger,"Se desconectó el Kernel\n");
			return;
		}
	}

}


void atender_clientes_file_system(int fd_fs){
	int socket_kernel = esperar_cliente(fd_fs);


	manejar_conexion(socket_kernel);

}

void inicializar_servidor(){
	char* puerto_escucha = config_valores.puerto_escucha;
	int fd_fs= iniciar_servidor(puerto_escucha);
	log_info(logger, "File System inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_escucha);

	atender_clientes_file_system(fd_fs);

}

