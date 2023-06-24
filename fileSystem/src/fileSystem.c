#include "fileSystem.h"

int main(void) {

	printf("%i \n",sizeof(unsigned char));

	char* puerto_escucha;


	//  Inicialización logger //
	// --- INICIO --- //
	t_log* logger = iniciar_logger();
	// --- FIN --- //

	//  Inicialización config //
	// --- INICIO --- //
	t_config* config = iniciar_config("fileSystem.config");
	// --- FIN --- //


	//  Inicialización super bloque //
	// --- INICIO --- //
	t_config* config_super_bloque = iniciar_config(config_get_string_value(config,"PATH_SUPERBLOQUE"));

	// --- FIN --- //


	// Obtención información superbloque //
	// --- INICIO --- //
	t_super_bloque super;
	super.block_count = config_get_int_value(config_super_bloque,"BLOCK_COUNT");
	super.block_size = config_get_int_value(config_super_bloque,"BLOCK_SIZE");
	// --- FIN --- //

	//  Inicialización bitmap //
	// --- INICIO --- //
	FILE* archivo_bm;
	t_bitarray* estructuraBitmap = inicializar_archivo_bm(archivo_bm,super.block_count,config);
	// --- FIN --- //

	// Inicialización archivo de bloques
	// --- INICIO --- //
	FILE* archivo_bloques;
	char* mapping_archivo_bloques;
	levantar_archivo_bloques(archivo_bloques,super.block_count,super.block_size,config,mapping_archivo_bloques);
	// --- FIN --- //

	// Loggear información fileSystem //
	// --- INICIO --- //
	log_info(logger, "El fileSystem posee una cantidad de %d bloques",super.block_count);
	log_info(logger, "Los bloques del fileSystem tiene un tamaño de %d bytes",super.block_size);
	log_info(logger, "Se inicializa el módulo File System");
    puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	log_info(logger, "PUERTO ESCUCHA: %s", puerto_escucha);
	// --- FIN --- //





	// Prueba respuesta a petición de creación de archivo (SEGUIR) //
	// --- INICIO --- //
	levantar_fcb_nuevo_archivo("fcbPrueba",super.block_size,estructuraBitmap);
	t_config* configTCB = iniciar_config("../fileSystem/grupoDeBloques/fcbPrueba");
	t_fcb fcb;

	fcb.puntero_directo = config_get_int_value(configTCB,"PUNTERO_DIRECTO");
	fcb.nombre_archivo = config_get_string_value(configTCB,"NOMBRE_ARCHIVO");

	printf("Puntero directo: %d \n",fcb.puntero_directo);
	printf("Nombre archivo:  %s \n",fcb.nombre_archivo);

	if(abrir_archivo("fcbPrueba"))
	{
		printf("Existe el archivo solicitado fcbPrueba \n");
	}
	else
	{
		printf("No existe el archivo solicitado fcbPrueba \n");
	}

	crear_archivo("campeonesF1");

	if(abrir_archivo("campeonesF1"))
	{
		printf("Existe el archivo solicitado campeonesF1 \n");
	}
	else
	{
		printf("No existe el archivo solicitado campeonesF1 \n");
	}


	truncar_archivo("campeonesF1","125",config_super_bloque,estructuraBitmap,mapping_archivo_bloques);

	int a = mapping_archivo_bloques;

	printf("%i \n",a);

	obtener_ruta_archivo("ArchivoEjemplo");

	// --- FIN --- //


	// Verificación bloques asignados a archivo creado se indican como ocupados //
	// --- INICIO --- //
	FILE* f;
	f = fopen("../fileSystem/grupoDeBloques/bitmap.bin","r+");


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
	// --- FIN --- //

	// Inicialización del file system como servidor cuyo ciente será el kernel //
	// --- INICIO --- //
	int fd_file_system = iniciar_servidor(puerto_escucha);
	log_info(logger, "File System inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_escucha);

	while(atender_clientes_file_system(fd_file_system,logger,config_super_bloque,estructuraBitmap));

//	int fd_kernel = esperar_cliente(logger, "FILE SYSTEM", fd_file_system);
//
//	while (1) {
//		int cod_op = recibir_operacion(fd_kernel);
//		switch (cod_op) {
//		case MENSAJE:
//			recibir_mensaje(logger, fd_kernel);
//			break;
//		case -1:
//			log_error(logger, "el cliente se desconecto. Terminando servidor");
//			return EXIT_FAILURE;
//		default:
//			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
//			break;
//		}
//	}

	// --- FIN --- //

	// Liberar recursos usados //
	// --- INICIO --- //
	liberar_recursos_bitmap(estructuraBitmap,super.block_count,super.block_size,archivo_bm,archivo_bloques,mapping_archivo_bloques);
	// --- FIN --- //

	return 0;
}
