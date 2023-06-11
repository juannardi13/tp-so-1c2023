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

// Ignorar de momento - no probado


t_bitarray* inicializarArchivoBM(int cantidad_bloques){

	// prueba en vez de 65k

	// bitarray - vector de booleanos
	// manejo de bits

	// con map, mapeo archivo, accedo facilmente al archivo con escribi con todos los bits en 0
	// dps con el acceso del map al archivo escrito lo traspaso al arreglo bitarray

	// orden de creacion de manera tal que cuanto tenga que cambiar archivo cambio bitarray, que cambia el string para luego
	// copiar el string modificado en el mmap que corresponde el cambio en el archivo
	// file -> mmap -> bitarray

	FILE* f;
	f = fopen("../fileSystem/bitmap.bin","w+");

	int fd = fileno(f);

	if(fd == -1){
		printf("Error al crear archivo bitmap\n");
		fclose(f);
	}


	// byte => 00000000
	unsigned char byte = 0;

	// no sumo uno porque es numero divisible por 8
	int cantidad_bytes = cantidad_bloques / 8;

	ftruncate(fd,cantidad_bytes);

	for(int i = 0; i < cantidad_bytes; i++){
		fwrite(&byte,1,1,f);
	}


	int s = ftell(f);

	printf("%i\n",s);

	char* mapping = mmap(NULL,cantidad_bytes+1,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);

	if(mapping == MAP_FAILED){
		printf("Error al mappear memoria\n");
	}




	t_bitarray* estructura_aux = bitarray_create_with_mode(mapping,cantidad_bytes,LSB_FIRST);


	// direcciones coinciden puedo liberar las estructuras con una sola referencia
	printf("%i\n",mapping);
	printf("%i\n",estructura_aux->bitarray);

	fclose(f);
	return estructura_aux;
}

bool estaOcupado(t_bitarray* estructura_bitmap,int nro_bloque){
	return bitarray_test_bit(estructura_bitmap,nro_bloque);
}

void usarBloque(t_bitarray* estructura_bitmap,int nro_bloque){
	FILE* f;
	f = fopen("../fileSystem/bitmap.bin","r+");


	if(fileno(f) == -1){
		printf("Error al abrir archivo bitmap\n");
	}

	bitarray_set_bit(estructura_bitmap,nro_bloque);


	fclose(f);
}

void liberarRecursosBitmap(t_bitarray* estructura_bitmap,int cantidad_bloques){
	int cantidad_bytes = cantidad_bloques / 8;
	munmap(estructura_bitmap->bitarray,cantidad_bytes+1);
	bitarray_destroy(estructura_bitmap);
}


void pruebaCreacionArchivoConfig(void){

}
