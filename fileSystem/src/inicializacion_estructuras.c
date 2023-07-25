#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// |								   |
// |   	FUNCIONES MÁS "ALTO NIVEL"     |
// |								   |

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

void inicializar_estructuras()
{
	//  Inicialización logger //
		// --- INICIO --- //
		logger = iniciar_logger();
		// --- FIN --- //

		//  Inicialización config //
		// --- INICIO --- //
		config = iniciar_config("fileSystem.config");

//		IP_MEMORIA=127.0.0.1
//		PUERTO_MEMORIA=8002
//		PUERTO_ESCUCHA=8003
//		PATH_SUPERBLOQUE=../fileSystem/grupoDeBloques/fileSystem.superbloque
//		PATH_BITMAP=../fileSystem/grupoDeBloques/bitmap.bin
//		PATH_BLOQUES=../fileSystem/grupoDeBloques/archivoBloques
//		PATH_FCB=/home/utnso/fs/fcb
//		RETARDO_ACCESO_BLOQUE=15000

		config_valores.ip = config_get_string_value(config,"IP_MEMORIA");
		config_valores.puerto_memoria = config_get_string_value(config,"PUERTO_MEMORIA");
		config_valores.puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");
		config_valores.retardo_acceso_bloque = config_get_int_value(config,"RETARDO_ACCESO_BLOQUE");
		config_valores.path_superbloque = config_get_string_value(config,"PATH_SUPERBLOQUE");
		config_valores.path_bitmap = config_get_string_value(config,"PATH_BITMAP");
		config_valores.path_bloques = config_get_string_value(config,"PATH_BLOQUES");
		config_valores.path_fcb = config_get_string_value(config,"PATH_FCB");


		// --- FIN --- //


		//  Inicialización super bloque //
		// --- INICIO --- //
		config_super_bloque = iniciar_config(config_get_string_value(config,"PATH_SUPERBLOQUE"));

		config_super_bloque_valores.block_count = config_get_int_value(config_super_bloque,"BLOCK_COUNT");
		config_super_bloque_valores.block_size = config_get_int_value(config_super_bloque,"BLOCK_SIZE");


		// --- FIN --- //


		// Obtención información superbloque //
		// --- INICIO --- //
		t_super_bloque super;
		super.block_count = config_get_int_value(config_super_bloque,"BLOCK_COUNT");
		super.block_size = config_get_int_value(config_super_bloque,"BLOCK_SIZE");
		// --- FIN --- //

		//  Inicialización bitmap //
		// --- INICIO --- //
		estructura_bitmap = inicializar_archivo_bm(archivo_bm);


		// --- FIN --- //

		// Inicialización archivo de bloques
		// --- INICIO --- //
		levantar_archivo_bloques();
		// --- FIN --- //

		// Loggear información fileSystem //
		// --- INICIO --- //

		char* puerto_escucha = config_get_string_value(config,"PUERTO_ESCUCHA");

		log_info(logger, "El fileSystem posee una cantidad de %d bloques",super.block_count);
		log_info(logger, "Los bloques del fileSystem tiene un tamaño de %d bytes",super.block_size);
		log_info(logger, "Se inicializa el módulo File System");
	    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
		log_info(logger, "PUERTO ESCUCHA: %s", puerto_escucha);
		// --- FIN --- //


}

