#include "fileSystem.h"

int main(void) {
	// char* ip_kernel;
	char* puerto_escucha;

	t_log* logger = iniciar_logger();
	t_config* config = iniciar_config("fileSystem.config");

	t_config* config_super_bloque = iniciar_config("fileSystem.superbloque");

	t_super_bloque super;

	super.block_count = config_get_int_value(config_super_bloque,"BLOCK_COUNT");
	super.block_size = config_get_int_value(config_super_bloque,"BLOCK_SIZE");


	log_info(logger, "El fileSystem posee una cantidad de %d bloques",super.block_count);
	log_info(logger, "Los bloques del fileSystem tiene un tamaño de %d bytes",super.block_size);

	log_info(logger, "Se inicializa el módulo File System");

    // ip_kernel = config_get_string_value(config, "IP");
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");

	log_info(logger, "PUERTO ESCUCHA: %s", puerto_escucha);

	t_bitarray* estructuraBitmap = inicializarArchivoBM(super.block_count);


	// lleno 16 bits de 1s, por ende tengo en 8 bits el número 255 y en otros 8 el número 225 (porque 11111111 en binario es 255 en decimal)
	for(int nroBloque = 0; nroBloque < 16; nroBloque++){
		if(!estaOcupado(estructuraBitmap,nroBloque)){
				usarBloque(estructuraBitmap,nroBloque);
			}
	}

	// esta iteración es solo para ver los cambios en el archivo // más allá de pruebas, se puede borrar

	FILE* f;
	f = fopen("../fileSystem/bitmap.bin","r+");


	if(fileno(f) == -1){
		printf("Error al abrir archivo bitmap\n");
	}

		fseek(f,0,0);

		unsigned char b;

		for(int i = 0; i < super.block_count/8; i++){
			fread(&b,1,1,f);
			printf("%u\n",b);
		}

		fclose(f);

	liberarRecursosBitmap(estructuraBitmap,super.block_count);


	int fd_file_system = iniciar_servidor(puerto_escucha);
	log_info(logger, "File System inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_escucha);
	int fd_kernel = esperar_cliente(logger, "FILE SYSTEM", fd_file_system);


	while (1) {
		int cod_op = recibir_operacion(fd_kernel);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(logger, fd_kernel);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

	return 0;
}
