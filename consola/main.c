#include "./include/main.h"
#include "./include/utils.h"
#define MAX_LEN 256

int main(int argc, char** argv) {
	char* ip_kernel;
	char* puerto_kernel;
	FILE* archivo_pseudocodigo = NULL;

    t_log* logger = iniciar_logger();

    if(argc != 3){
    		log_error(logger, "Uso: consola path tamanio");
    		exit(1);
    }

    log_info(logger, "Hola! Se inicializo el modulo cliente Consola.");

    t_config* config = iniciar_config(argv[2]);

   	ip_kernel = config_get_string_value(config, "IP");
    puerto_kernel = config_get_string_value(config, "PUERTO");

	log_info(logger, "Se ha leido el archivo de config con exito, con los valores PUERTO: %s e IP: %s.", puerto_kernel, ip_kernel);

	archivo_pseudocodigo = abrir_archivo_instrucciones(argv[1], logger);

    int conexion_kernel = crear_conexion(logger, "KERNEL", ip_kernel, puerto_kernel);

    log_info(logger, "Conexion exitosa con el Kernel.");
    enviar_mensaje(puerto_kernel, conexion_kernel);

    levantar_instrucciones(archivo_pseudocodigo, logger, conexion_kernel);
    // AGREGO COMENTARIO EN CONSOLA PARA VER SI FUNCIONA EL COMMIT
 //   ⢀⡴⠑⡄⠀⠀⠀⠀⠀⠀⠀⣀⣀⣤⣤⣤⣀⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
  //  ⠸⡇⠀⠿⡀⠀⠀⠀⣀⡴⢿⣿⣿⣿⣿⣿⣿⣿⣷⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀
 //   ⠀⠀⠀⠀⠑⢄⣠⠾⠁⣀⣄⡈⠙⣿⣿⣿⣿⣿⣿⣿⣿⣆⠀⠀⠀⠀⠀⠀⠀⠀
   // ⠀⠀⠀⠀⢀⡀⠁⠀⠀⠈⠙⠛⠂⠈⣿⣿⣿⣿⣿⠿⡿⢿⣆⠀⠀⠀⠀⠀⠀⠀
//    ⠀⠀⠀⢀⡾⣁⣀⠀⠴⠂⠙⣗⡀⠀⢻⣿⣿⠭⢤⣴⣦⣤⣹⠀⠀⠀⢀⢴⣶⣆
//    ⠀⠀⢀⣾⣿⣿⣿⣷⣮⣽⣾⣿⣥⣴⣿⣿⡿⢂⠔⢚⡿⢿⣿⣦⣴⣾⠁⠸⣼⡿
//    ⠀⢀⡞⠁⠙⠻⠿⠟⠉⠀⠛⢹⣿⣿⣿⣿⣿⣌⢤⣼⣿⣾⣿⡟⠉⠀⠀⠀⠀⠀
 //   ⠀⣾⣷⣶⠇⠀⠀⣤⣄⣀⡀⠈⠻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀
 //   ⠀⠉⠈⠉⠀⠀⢦⡈⢻⣿⣿⣿⣶⣶⣶⣶⣤⣽⡹⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀
    //⠀⠀⠀⠀⠀⠀⠀⠉⠲⣽⡻⢿⣿⣿⣿⣿⣿⣿⣷⣜⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀
   // ⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣷⣶⣮⣭⣽⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀
    ⠀//⠀⠀⠀⠀⠀⣀⣀⣈⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀⠀⠀⠀⠀⠀⠀
    ⠀//⠀⠀⠀⠀⠀⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀//⠀⠀⠀⠀⠹⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀//⠀⠀⠀⠀⠉⠛⠻⠿⠿⠿⠿⠛⠉
//
    return EXIT_SUCCESS;
}


void paquete_pseudocodigo(int conexion)
{
	t_paquete* paquete = crear_paquete();

	 char buffer[MAX_LEN];
	 FILE* file = fopen("./cfg/pseudocodigo.txt", "r");
	 if (file == NULL) {
	     printf("Error al abrir el archivo.\n");
	     exit(3);
	 }

	 char** lista_lineas = malloc(sizeof(char*) * 10);
	 int num_lineas = 0;

	 while (fgets(buffer, MAX_LEN, file) != NULL) {
	     char* linea = malloc(sizeof(char) * MAX_LEN);
	     strcpy(linea, buffer);
	     lista_lineas[num_lineas] = linea;
	     num_lineas++;

	     if (num_lineas % 10 == 0) {
	         lista_lineas = realloc(lista_lineas, sizeof(char*) * (num_lineas + 10));
	     }
	 }

	fclose(file);

	for (int i = 0; i < num_lineas; i++) {
		agregar_a_paquete(paquete, lista_lineas[i], 256);
		printf("%s\n", lista_lineas[i]);
	    free(lista_lineas[i]);
    }

	free(lista_lineas);


	enviar_paquete(paquete, conexion);
	free(paquete);
}
