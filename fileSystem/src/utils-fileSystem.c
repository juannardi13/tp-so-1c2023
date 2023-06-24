#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


t_log* iniciar_logger(void) {
	t_log* nuevo_logger = log_create("fileSystem.log", "File System", 1, LOG_LEVEL_INFO);

	if (nuevo_logger == NULL) {
		printf("No se pudo crear el log\n");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(char* ruta) {
	t_config* nuevo_config = config_create(ruta);

	if (nuevo_config == NULL) {
		printf("No se pudo leer la config\n");
		exit(2);
	}

	return nuevo_config;
}

void manejar_conexion(int socket_cliente,t_log* logger,t_config* config,t_bitarray* estructura_bitmap){

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


int atender_clientes_file_system(int socket_fs,t_log* logger_fs,t_config* config,t_bitarray* estructura_bitmap){
	int socket_consola = esperar_cliente(logger_fs, "FILE SYSTEM", socket_fs);

	int ret = 0;

	if(socket_consola != -1)
	{
		manejar_conexion(socket_consola,logger_fs,config,estructura_bitmap);
		ret = 1;
	}
	else
	{
		log_error(logger_fs,"Error al escuchar cliente... Finalizando servidor \n");
	}

	return ret;
}

void concatenar_strings(const char* s1, const char* s2,char* buffer){
	memccpy(memccpy(buffer,s1,'\0',100)-1,s2,'\0',100);
}


int abrir_archivo(char* nombre_archivo){
	// busco archivo, si no existe f será nula y fd será -1, devolver fd
	FILE* f;

	int boolExiste = 1;

	char* ruta = obtener_ruta_archivo(nombre_archivo);

	f = fopen(ruta,"r");

	if(f == NULL)
	{
		boolExiste = 0;
	}
	return boolExiste;
}




t_bitarray* inicializar_archivo_bm(FILE* f,int cantidad_bloques,t_config* config){

	f = fopen(config_get_string_value(config,"PATH_BITMAP"),"w+");

	int fd = fileno(f);

	if(fd == -1){
		printf("Error al crear archivo bitmap\n");
		fclose(f);
	}

	unsigned char byte = 0;

	int byte_adicional = (cantidad_bloques % 8 != 0)?1:0;

	int cantidad_bytes = (cantidad_bloques / 8) + byte_adicional;

	ftruncate(fd,cantidad_bytes);

	for(int i = 0; i < cantidad_bytes; i++){
		fwrite(&byte,1,1,f);
	}

	char* mapping = mmap(NULL,cantidad_bytes+1,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);

	if(mapping == MAP_FAILED){
		printf("Error al mappear memoria\n");
	}

	t_bitarray* estructura_aux = bitarray_create_with_mode(mapping,cantidad_bytes,LSB_FIRST);

	return estructura_aux;
}

bool esta_ocupado(t_bitarray* estructura_bitmap,int nro_bloque){
	return bitarray_test_bit(estructura_bitmap,nro_bloque);
}

int primer_bloque_disponible(t_bitarray* estructura_bitmap)
{
	int i = 0;
	int limite = estructura_bitmap->size * 8;

	while(i < limite && esta_ocupado(estructura_bitmap,i)){
		i++;
	}

	return i;
}

void asignar_bloques_iniciales(t_bitarray* estructura_bitmap,FILE* fcb){
	FILE* f;
	f = fopen("../fileSystem/grupoDeBloques/bitmap.bin","r+");

	if(fileno(f) == -1){
		printf("Error al abrir archivo bitmap\n");
	}

	int nro_primer_bloque = primer_bloque_disponible(estructura_bitmap);

	bitarray_set_bit(estructura_bitmap,nro_primer_bloque);

	fprintf(fcb,"PUNTERO_DIRECTO=%i\n",nro_primer_bloque);

	int nro_bloque_punteros = primer_bloque_disponible(estructura_bitmap);

	bitarray_set_bit(estructura_bitmap,nro_bloque_punteros);

	fprintf(fcb,"PUNTERO_INDIRECTO=%i\n",nro_bloque_punteros);

	fclose(f);
	fclose(fcb);
}

void liberar_recursos_bitmap(t_bitarray* estructura_bitmap,int cantidad_bloques,int tamanio_bloques,FILE* archivo_bm,FILE* archivo_bloques,char* mapping_archivo_bloques){
	int cantidad_bytes = cantidad_bloques / 8;
	munmap(estructura_bitmap->bitarray,cantidad_bytes+1);
	munmap(mapping_archivo_bloques,tamanio_bloques*cantidad_bloques);
	fclose(archivo_bm);
	fclose(archivo_bloques);
	bitarray_destroy(estructura_bitmap);
}


// un archivo fcb por cada archivo (fichero)usado. Tiene puntero "a lista" de bloques, a bloque con punteros a otros bloques y puntero directo a bloque inicial
// PERO, como se recomienda un archivo de bloques y no una lista de bloques, el puntero "sera" la posicion dentro del archivo
// donde se cuenta con el contenido. Cada posición del archivo de bloques es un bloque (64 bytes). Además, para cada archivo, dentro
// de esta lista / archivo de bloques habrá un bloque que contendrá únicamente punteros hacia el resto de bloques del archivo.
// como cada bloque
// luego cada bloque lo leo de a 8 bits (1 byte) -> tamaño estandar de stream (flujo de datos cuyo tipo no conozco) ???? COMO RESUELVO ESTO



// cada registro de archivo será un bloque, por ende cada registro del archivo consiste de 64 bytes (o lo que se indique en el archivo de config que ocupan los bloques)
//
void levantar_archivo_bloques(FILE*f,int cantidad_bloques,int tamanio_bloques,t_config* config,char* mapping){
	f = fopen(config_get_string_value(config,"PATH_BLOQUES"),"w+");

	int fd = fileno(f);

	if(fd == -1){
		printf("Error al crear archivo de bloques\n");
		fclose(f);
	}
	int tamanio_en_bytes = cantidad_bloques * tamanio_bloques;

	ftruncate(fd,tamanio_en_bytes);

	mapping = mmap(NULL,tamanio_en_bytes,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);

	if(mapping == MAP_FAILED){
		printf("Error al mappear memoria\n");
	}


}

int obtener_posicion_archivo_bloques(int indice,int tamanio_bloques)
{
	return indice*tamanio_bloques;
}


// tamanio de archivo => al inicializarlo se le asigna mínimo un bloque de para almacenar punteros
// tamanio inicial de archivo es el de 1 bloque
// se asignará como primer bloque y bloque indirecto los primeros bloques disponibles, para lo cual se consulta el bitmap

char* obtener_ruta_archivo(const char* nombre_archivo)
{
	const char* ruta_incompleta = "../fileSystem/grupoDeBloques/";

	char* ruta = malloc(25*sizeof(int));
	concatenar_strings(ruta_incompleta,nombre_archivo,ruta);

	return ruta;
}

int crear_archivo(char* nombre_archivo)
{
	FILE* f;

	char* ruta = obtener_ruta_archivo(nombre_archivo);

	f = fopen(ruta,"w+");

	int fd = fileno(f);

	if(fd == -1){
		printf("Error al crear archivo de config\n");
		fclose(f);
	}

	fprintf(f,"NOMBRE_ARCHIVO=%s\n",nombre_archivo);
	fprintf(f,"TAMANIO_ARCHIVO=%s\n","0");
	fprintf(f,"PUNTERO_DIRECTO=%s\n","-1");
	fprintf(f,"PUNTERO_INDIRECTO=%s\n","-1");

	fclose(f);
	free(ruta);

	return 1;
}

void truncar_archivo(char* nombre_archivo,char* nuevo_tamanio,t_config* config_super_bloque,t_bitarray* estructura_bitmap,char* mapping_archivo_bloques)
{
	int nuevo_tamanio_entero = atoi(nuevo_tamanio);

	FILE* f;

	char* ruta = obtener_ruta_archivo(nombre_archivo);

	f = fopen(ruta,"r+");

	if (fileno(f) == -1){
		printf("Error relacionado al archivo %s \n",nombre_archivo);
	}

	t_config* config_fcb_archivo = iniciar_config(ruta);

	int tamanio_fcb = config_get_int_value(config_fcb_archivo,"TAMANIO_ARCHIVO");

	int tamanio_bloque = config_get_int_value(config_super_bloque,"BLOCK_SIZE");

	int cantidad_bloques = config_get_int_value(config_super_bloque,"BLOCK_COUNT");

	if(nuevo_tamanio_entero > tamanio_fcb)
	{
		if(tamanio_fcb == 0 && nuevo_tamanio_entero <= tamanio_bloque){
			int puntero_directo = primer_bloque_disponible(estructura_bitmap);

			bitarray_set_bit(estructura_bitmap,puntero_directo);

			char* puntero_directo_string = string_itoa(puntero_directo);

			config_set_value(config_fcb_archivo,"PUNTERO_DIRECTO",puntero_directo_string);
		}
		if(tamanio_fcb == 0 && nuevo_tamanio_entero > tamanio_bloque)
		{
			int cantidad_bloques_puntero_indirecto_parcial = (nuevo_tamanio_entero - tamanio_bloque) / tamanio_bloque;

			int bloque_adicional = (cantidad_bloques_puntero_indirecto_parcial % tamanio_bloque != 0)?1:0;

			int cantidad_bloques_puntero_indirecto_final = cantidad_bloques_puntero_indirecto_parcial + bloque_adicional;

			int puntero_directo = primer_bloque_disponible(estructura_bitmap);

			bitarray_set_bit(estructura_bitmap,puntero_directo);

			char* puntero_directo_string = string_itoa(puntero_directo);

			config_set_value(config_fcb_archivo,"PUNTERO_DIRECTO",puntero_directo_string);

			int puntero_indirecto = primer_bloque_disponible(estructura_bitmap);

			bitarray_set_bit(estructura_bitmap,puntero_indirecto);

			config_set_value(config_fcb_archivo,"PUNTERO_INDIRECTO",string_itoa(puntero_indirecto));

			for(int i = 0; i < cantidad_bloques_puntero_indirecto_final;i++){
				int bloque_datos = primer_bloque_disponible(estructura_bitmap);

				bitarray_set_bit(estructura_bitmap,bloque_datos);

				// escribir número de bloque asignado en el bloque del puntero indirecto (todavía no hecho)

				memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_directo,tamanio_bloque),&bloque_datos,sizeof(uint32_t));

				msync(mapping_archivo_bloques,tamanio_bloque*cantidad_bloques,MS_SYNC);
			}

		}
		else
		{

		}
	}

	config_set_value(config_fcb_archivo,"TAMANIO_ARCHIVO",nuevo_tamanio);

	config_save(config_fcb_archivo);

	fclose(f);

}

void levantar_fcb_nuevo_archivo(const char* nombre_archivo, int tamanio_bloques,t_bitarray* estructura_bitmap){
	FILE* f;

	char* ruta = obtener_ruta_archivo(nombre_archivo);

	f = fopen(ruta,"w+");

	int fd = fileno(f);

	if(fd == -1){
		printf("Error al crear archivo de config\n");
		fclose(f);
	}

	fprintf(f,"NOMBRE_ARCHIVO=%s\n",nombre_archivo);
	fprintf(f,"TAMANIO_ARCHIVO=%i\n",tamanio_bloques*2);

	asignar_bloques_iniciales(estructura_bitmap,f);
	free(ruta);
}
