#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int socket_memoria;

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

	free(contenido_buffer);
	free(string_recibido);
}

// devolver cod op finalizado
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

	void* direccion_fisica_contenido  = leer_archivo(string_nombre_archivo,nro_byte_archivo,cantidad_bytes_leer);

	printf("%s\n",direccion_fisica_contenido);

	t_paquete* paquete = crear_paquete(ESCRIBIR_EN_MEMORIA_FS);

	// primero dir fisica, dps tamanio valor y dps valor

	agregar_int_a_paquete(paquete,direccion_fisica);
	agregar_int_a_paquete(paquete,cantidad_bytes_leer);
	agregar_string_a_paquete(paquete,direccion_fisica_contenido,cantidad_bytes_leer);

	enviar_paquete(paquete,socket_memoria);

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
	free(direccion_fisica_contenido);
	free(contenido_buffer);
	free(string_nombre_archivo);
}


// devolver cod op finalizado
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

	t_paquete* paquete = crear_paquete(LEER_DE_MEMORIA_FS);

	agregar_int_a_paquete(paquete,direccion_fisica);
	agregar_int_a_paquete(paquete,cantidad_bytes_escribir);

	enviar_paquete(paquete,socket_memoria);


	int rta_memoria = recibir_operacion(socket_memoria);


	switch(rta_memoria) {
			case LEIDO :
				log_info(logger, "Se escribió con éxito en memoria");

				int size_buffer_mem;
				void* contenido_buffer_mem = recibir_buffer(&size_buffer_mem,socket_cliente);

				int offset_mem = 0;
				int tamanio_recibir = deserializar_int(contenido_buffer_mem,&offset_mem);
				void* direccion_fisica_contenido = deserializar_string(tamanio_recibir,contenido_buffer_mem,&offset_mem);

				escribir_archivo(string_nombre_archivo,nro_byte_archivo,direccion_fisica_contenido,cantidad_bytes_escribir);

				int cod_op = FINALIZADO;

				log_info(logger,"Escribir Archivo: <%s> - Puntero <%i> - Memoria <%i> - Tamaño <%i>",string_nombre_archivo,nro_byte_archivo,direccion_fisica,cantidad_bytes_escribir);

				send_op(socket_cliente,cod_op);

				free(direccion_fisica_contenido);
				free(contenido_buffer_mem);

				break;
			case NO_LEIDO :
				printf("No se pudo leer el valor del registro dado");
				break;
			default :
				printf("Operacion desconocida");
				break;
		}


//	void* direccion_fisica_contenido = "Fernando Alonso";

	free(contenido_buffer);
	free(string_nombre_archivo);

}

// devolver cod op finalizado
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

	int cod_op = FINALIZADO;

	log_info(logger,"Truncar Archivo: <%s> - Tamaño: <%i>",string_nombre_archivo,tamanio_nuevo);

	send_op(socket_cliente,cod_op);

	free(contenido_buffer);
	free(string_nombre_archivo);
}

void manejar_fcreate(int socket_cliente)
{
	int size_buffer;
	void* contenido_buffer = recibir_buffer(&size_buffer,socket_cliente);

	int offset = 0;
	int tamanio_string_nombre_archivo = deserializar_int(contenido_buffer,&offset);
	char* string_nombre_archivo = deserializar_string(tamanio_string_nombre_archivo,contenido_buffer,&offset);

	crear_archivo(string_nombre_archivo);

	free(contenido_buffer);
	free(string_nombre_archivo);
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

		case F_CREATE:
				manejar_fcreate(socket_cliente);
				break;

		case -1:
			log_info(logger,"Se desconectó el Kernel\n");
			return;
		default:
			log_warning(logger,"Operación desconocido. Ojo con lo que haces");
			break;
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

