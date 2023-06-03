#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>


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

void inicializarArchivoBM(int cantidad_bloques){
	FILE* f;
	f = fopen("bitmap.bin","w+");

	int fd = fileno(f);

	if(fd == 1){
		printf("Error al crear bitmap/n");
	}

	int cantidad_bytes = cantidad_bloques / 8;

	ftruncate(f,cantidad_bytes+1);

	char* mapping = mmap(NULL,cantidad_bytes,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);

	t_bitarray* estructura_aux = bitarray_create_with_mode(mapping,cantidad_bytes+1,LSB_FIRST);

	for(int i = 0; i < cantidad_bloques;i++){
		bitarray_set_bit(estructura_aux,0);
	}

	fputs(estructura_aux->bitarray,f);

	bitarray_destroy(estructura_aux);
	free(mapping);
}

