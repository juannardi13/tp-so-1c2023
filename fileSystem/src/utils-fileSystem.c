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


t_bitarray* inicializar_archivo_bm(int cantidad_bloques){

	FILE* f;
	f = fopen("../fileSystem/bitmap.bin","w+");

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

	fclose(f);
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
	f = fopen("../fileSystem/bitmap.bin","r+");

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

void liberar_recursos_bitmap(t_bitarray* estructura_bitmap,int cantidad_bloques){
	int cantidad_bytes = cantidad_bloques / 8;
	munmap(estructura_bitmap->bitarray,cantidad_bytes+1);
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
void levantar_archivo_bloques(int cantidad_bloques,int tamanio_bloques){
	FILE* f;
	f = fopen("../fileSystem/archivoBloques","w+");

	int fd = fileno(f);

	if(fd == -1){
		printf("Error al crear archivo de bloques\n");
		fclose(f);
	}
	int tamanio_en_bytes = cantidad_bloques * tamanio_bloques;

	ftruncate(fd,tamanio_en_bytes);

	fclose(f);
}


// tamanio de archivo => al inicializarlo se le asigna mínimo un bloque de para almacenar punteros
// tamanio inicial de archivo es el de 1 bloque
// se asignará como primer bloque y bloque indirecto los primeros bloques disponibles, para lo cual se consulta el bitmap

void concatenar_strings(const char* s1, const char* s2,char* buffer){
	memccpy(memccpy(buffer,s1,'\0',100)-1,s2,'\0',100);
}


void levantar_fcb_nuevo_archivo(const char* nombre_archivo, int tamanio_bloques,t_bitarray* estructura_bitmap){
	FILE* f;
	const char* ruta_incompleta = "../fileSystem/";

	char* ruta = malloc(25*sizeof(int));
	concatenar_strings(ruta_incompleta,nombre_archivo,ruta);

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
