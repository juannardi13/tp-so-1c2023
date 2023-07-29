#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int socket_memoria;

void manejar_fopen(int socket_cliente,void* stream)
{
	// ya saque cod op, me falta el size del buffer y el buffer

	int offset = 0;

	int tam_nombre_archivo;
	memcpy(&tam_nombre_archivo,stream + offset,sizeof(int));
	offset += sizeof(int);
	char* string_recibido = malloc(tam_nombre_archivo);
	memset(string_recibido,0,tam_nombre_archivo);
	memcpy(string_recibido,stream + offset,tam_nombre_archivo);
	offset += tam_nombre_archivo;


	int rta_para_kernel = abrir_archivo(string_recibido);

	int rta;

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

	send_op(socket_cliente,rta);

	free(stream);
	free(string_recibido);
}

// devolver cod op finalizado
void manejar_fread(int socket_cliente,void* stream)
{


	// leer_archivo(char* nombre_archivo,int nro_byte_archivo,char* direccion_volcar_lectura,int cantidad_bytes_leer)

	int offset = 0;
	int tamanio_string_nombre_archivo;
	int direccion_fisica;
	int nro_byte_archivo;
	int cantidad_bytes_leer;

	memcpy(&tamanio_string_nombre_archivo,stream + offset,sizeof(int));
	offset += sizeof(int);
	char* string_nombre_archivo = malloc(tamanio_string_nombre_archivo);
	memcpy(string_nombre_archivo,stream + offset,tamanio_string_nombre_archivo);
	offset += tamanio_string_nombre_archivo;
	memcpy(&nro_byte_archivo,stream + offset,sizeof(int));
	offset += sizeof(int);
	memcpy(&direccion_fisica,stream + offset,sizeof(int));
	offset += sizeof(int);
	memcpy(&cantidad_bytes_leer,stream + offset,sizeof(int));

//	int tamanio_string_nombre_archivo = deserializar_int(contenido_buffer,&offset);
//	char* string_nombre_archivo = deserializar_string(tamanio_string_nombre_archivo,contenido_buffer,&offset);
//	int nro_byte_archivo = deserializar_int(contenido_buffer,&offset);
//	int direccion_fisica = deserializar_int(contenido_buffer,&offset);
//	int cantidad_bytes_leer = deserializar_int(contenido_buffer,&offset);

	void* direccion_fisica_contenido  = leer_archivo(string_nombre_archivo,nro_byte_archivo,cantidad_bytes_leer);

	printf("%s\n",direccion_fisica_contenido);


	// primero dir fisica, dps tamanio valor y dps valor

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	paquete->buffer->stream_size = sizeof(int) + sizeof(int) + cantidad_bytes_leer;
	void* stream_mem = malloc(paquete->buffer->stream_size);

	offset = 0;
	memcpy(stream_mem + offset,&direccion_fisica,sizeof(int));
	offset += sizeof(int);
	memcpy(stream_mem + offset,&cantidad_bytes_leer,sizeof(int));
	offset += sizeof(int);
	memcpy(stream_mem + offset,direccion_fisica_contenido,cantidad_bytes_leer);
	offset += cantidad_bytes_leer;

	paquete->codigo_operacion = ESCRIBIR_EN_MEMORIA_FS;
	paquete->buffer->stream = stream_mem;

	int tam_a_enviar = sizeof(int) + sizeof(int) + paquete->buffer->stream_size;
	void* a_enviar = malloc(tam_a_enviar);
	offset = 0;
	agregar_a_stream(a_enviar,&offset,&(paquete->codigo_operacion),sizeof(int));
	agregar_a_stream(a_enviar,&offset,&(paquete->buffer->stream_size),sizeof(int));
	agregar_a_stream(a_enviar,&offset,paquete->buffer->stream,paquete->buffer->stream_size);

	send(socket_memoria,a_enviar,tam_a_enviar,0);

	int rta_memoria = recibir_operacion(socket_memoria);

	if(rta_memoria == OK_VALOR_ESCRITO)
	{
		int cod_op = FINALIZADO;

		send_op(socket_cliente,cod_op);

		log_info(logger,"Leer Archivo: <%s> - Puntero <%i> - Memoria <%i> - Tamaño <%i>",string_nombre_archivo,nro_byte_archivo,direccion_fisica,cantidad_bytes_leer);


	}
	else
	{
		log_warning(logger, "Error al recibir respuesta de memoria");
	}

	eliminar_paquete(paquete);
	free(stream);
	free(direccion_fisica_contenido);
	free(string_nombre_archivo);
}


// devolver cod op finalizado
void manejar_fwrite(int socket_cliente,void* stream)
{
	int offset = 0;
	int tamanio_string_nombre_archivo;
	int direccion_fisica;
	int nro_byte_archivo;
	int cantidad_bytes_escribir;

	memcpy(&tamanio_string_nombre_archivo,stream + offset,sizeof(int));
	offset += sizeof(int);
	char* string_nombre_archivo = malloc(tamanio_string_nombre_archivo);
	memcpy(string_nombre_archivo,stream + offset,tamanio_string_nombre_archivo);
	offset += tamanio_string_nombre_archivo;
	memcpy(&nro_byte_archivo,stream + offset,sizeof(int));
	offset += sizeof(int);
	memcpy(&direccion_fisica,stream + offset,sizeof(int));
	offset += sizeof(int);
	memcpy(&cantidad_bytes_escribir,stream + offset,sizeof(int));

	// leer de memoria en la dir fisica, eso escribir a archivo

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	paquete->buffer->stream_size = sizeof(int) + sizeof(int);
	void* stream_mem = malloc(paquete->buffer->stream_size);

	offset = 0;
	memcpy(stream_mem + offset,&direccion_fisica,sizeof(int));
	offset += sizeof(int);
	memcpy(stream_mem + offset,&cantidad_bytes_escribir,sizeof(int));
	offset += sizeof(int);


	paquete->codigo_operacion = LEER_DE_MEMORIA_FS;
	paquete->buffer->stream = stream_mem;

	int tam_a_enviar = sizeof(int) + sizeof(int) + paquete->buffer->stream_size;
	void* a_enviar = malloc(tam_a_enviar);
	offset = 0;
	agregar_a_stream(a_enviar,&offset,&(paquete->codigo_operacion),sizeof(int));
	agregar_a_stream(a_enviar,&offset,&(paquete->buffer->stream_size),sizeof(int));
	agregar_a_stream(a_enviar,&offset,paquete->buffer->stream,paquete->buffer->stream_size);

	send(socket_memoria,a_enviar,tam_a_enviar,0);

	t_paquete* paquete_rta = malloc(sizeof(t_paquete));
	paquete_rta->buffer = malloc(sizeof(t_buffer));

	recv(socket_memoria,&(paquete_rta->codigo_operacion),sizeof(int),MSG_WAITALL);
	recv(socket_memoria,&(paquete_rta->buffer->stream_size),sizeof(int),0);

	paquete_rta->buffer->stream = malloc(paquete_rta->buffer->stream_size);
	recv(socket_memoria,paquete_rta->buffer->stream,paquete_rta->buffer->stream_size,0);

	void* stream_rta = paquete_rta->buffer->stream;

//	switch(paquete_rta->codigo_operacion) {
//			case LEIDO :
				log_info(logger, "Se leyó con éxito de memoria");

//				int size_buffer_mem;
//				void* contenido_buffer_mem = recibir_buffer(&size_buffer_mem,socket_cliente);
//
//				int offset_mem = 0;
//				int tamanio_recibir = deserializar_int(contenido_buffer_mem,&offset_mem);
//				void* direccion_fisica_contenido = deserializar_string(tamanio_recibir,contenido_buffer_mem,&offset_mem);

				int tamanio_recibido;
				int offset_rta = 0;

				memcpy(&tamanio_recibido,stream_rta + offset_rta,sizeof(int));
				offset_rta += sizeof(int);

				char* direccion_fisica_contenido = malloc(tamanio_recibido);
				memset(direccion_fisica_contenido, 0, tamanio_recibido);

				memcpy(direccion_fisica_contenido, stream_rta + offset_rta, tamanio_recibido);
				offset_rta += tamanio_recibido;

				log_warning(logger, "LO QUE SE LEYÓ DE MEMORIA: %s", direccion_fisica_contenido);

				escribir_archivo(string_nombre_archivo,nro_byte_archivo,direccion_fisica_contenido,cantidad_bytes_escribir);

				int cod_op = FINALIZADO;

				log_info(logger,"Escribir Archivo: <%s> - Puntero <%i> - Memoria <%i> - Tamaño <%i>",string_nombre_archivo,nro_byte_archivo,direccion_fisica,cantidad_bytes_escribir);

				send_op(socket_cliente,cod_op);

				free(direccion_fisica_contenido);

//				break;
//			case NO_LEIDO :
//				printf("No se pudo leer el valor del registro dado");
//				break;
//			default :
//				printf("Operacion desconocida");
//				break;
//		}


//	void* direccion_fisica_contenido = "Fernando Alonso";

	eliminar_paquete(paquete_rta);
	eliminar_paquete(paquete);
	free(string_nombre_archivo);

}

// devolver cod op finalizado
void manejar_ftruncate(int socket_cliente,void* stream)
{
//	int size_buffer;
//	void* contenido_buffer = recibir_buffer(&size_buffer,socket_cliente);

	// leer_archivo(char* nombre_archivo,int nro_byte_archivo,char* direccion_volcar_lectura,int cantidad_bytes_leer)

//	int offset = 0;
//	int tamanio_string_nombre_archivo = deserializar_int(contenido_buffer,&offset);
//	char* string_nombre_archivo = deserializar_string(tamanio_string_nombre_archivo,contenido_buffer,&offset);
//	int tamanio_nuevo = deserializar_int(contenido_buffer,&offset);

	int offset = 0;
	int tamanio_string_nombre_archivo;
	int tamanio_nuevo;


	memcpy(&tamanio_string_nombre_archivo,stream + offset,sizeof(int));
	offset += sizeof(int);
	char* string_nombre_archivo = malloc(tamanio_string_nombre_archivo);
	memcpy(string_nombre_archivo,stream + offset,tamanio_string_nombre_archivo);
	offset += tamanio_string_nombre_archivo;
	memcpy(&tamanio_nuevo,stream + offset,sizeof(int));
	offset += sizeof(int);

	truncar_archivo(string_nombre_archivo,tamanio_nuevo);

	int cod_op = FINALIZADO;

	log_info(logger,"Truncar Archivo: <%s> - Tamaño: <%i>",string_nombre_archivo,tamanio_nuevo);

	send_op(socket_cliente,cod_op);

	free(stream);
	free(string_nombre_archivo);
}

void manejar_fcreate(int socket_cliente,void* stream)
{
	int offset = 0;

	int tam_nombre_archivo;
	memcpy(&tam_nombre_archivo,stream + offset,sizeof(int));
	offset += sizeof(int);
	char* string_recibido = malloc(tam_nombre_archivo);
	memset(string_recibido,0,tam_nombre_archivo);
	memcpy(string_recibido,stream + offset,tam_nombre_archivo);
	offset += tam_nombre_archivo;

	crear_archivo(string_recibido);

	free(string_recibido);
	free(stream);
}

void manejar_conexion(int socket_cliente){

	while(1){

		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));

		recv(socket_cliente,&(paquete->codigo_operacion),sizeof(int),MSG_WAITALL);
		recv(socket_cliente,&(paquete->buffer->stream_size),sizeof(int),0);

		paquete->buffer->stream = malloc(paquete->buffer->stream_size);
		recv(socket_cliente,paquete->buffer->stream,paquete->buffer->stream_size,0);

		void* stream = paquete->buffer->stream;

		switch(paquete->codigo_operacion)
		{

		case F_OPEN:
				manejar_fopen(socket_cliente,stream);
				break;

		case F_READ:
				manejar_fread(socket_cliente,stream);
				break;

		case F_WRITE:
				manejar_fwrite(socket_cliente,stream);
				break;

		case F_TRUNCATE:
				manejar_ftruncate(socket_cliente,stream);
				break;

		case F_CREATE:
				manejar_fcreate(socket_cliente,stream);
				break;

		case -1:
			log_info(logger,"Se desconectó el Kernel\n");
			return;
		default:
			log_warning(logger,"Operación desconocido. Ojo con lo que haces");
			return;
		}
	}

}


void atender_clientes_file_system(int fd_fs){
	int socket_kernel = esperar_cliente_alt(fd_fs);
	log_info(logger, "Se conectó el Kernel");

	manejar_conexion(socket_kernel);

}

void inicializar_servidor(){

	socket_memoria = crear_conexion_alt(config_valores.ip, config_valores.puerto_memoria);

	char* puerto_escucha = config_valores.puerto_escucha;
	int fd_fs= iniciar_servidor(puerto_escucha);
	log_info(logger, "File System inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_escucha);

	atender_clientes_file_system(fd_fs);


}

