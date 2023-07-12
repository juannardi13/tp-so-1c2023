#include"utils_cpu2.h"
#include <math.h>
#include<shared-2.h>

void decode_instruccion(char* instruccion, t_contexto_de_ejecucion* contexto, t_config* config, int fd_memoria, int fd_kernel, bool cpu_bloqueada){

	char** instruccion_parseada = string_split(instruccion, " ");

	if(strcmp(instruccion_parseada[0], "SET") == 0){
		int tiempo_de_espera;
		tiempo_de_espera = config_get_int_value(config, "RETARDO_INSTRUCCION");
		sleep(tiempo_de_espera);
		ejecutar_SET(instruccion_parseada, contexto);
	}
	else if(strcmp(instruccion_parseada[0], "MOV_IN") == 0){
		ejecutar_MOV_IN(instruccion_parseada, contexto, fd_memoria, config);
	}
	else if(strcmp(instruccion_parseada[0], "MOV_OUT") == 0){
		ejecutar_MOV_OUT(instruccion_parseada, contexto, fd_memoria, config);
	}
	else if(strcmp(instruccion_parseada[0], "IO") == 0){
		ejecutar_IO(instruccion_parseada, contexto, fd_kernel, cpu_bloqueada);
	}
	else if(strcmp(instruccion_parseada[0], "F_OPEN") == 0){
		ejecutar_F_OPEN(instruccion_parseada, contexto, fd_kernel);
	}
	else if(strcmp(instruccion_parseada[0], "F_CLOSE") == 0){
		ejecutar_F_CLOSE(instruccion_parseada, contexto, fd_kernel);
	}
	else if(strcmp(instruccion_parseada[0], "F_SEEK") == 0){
			ejecutar_F_SEEK(instruccion_parseada, contexto, fd_kernel);
		}
	else if(strcmp(instruccion_parseada[0], "F_READ") == 0){
			ejecutar_F_READ(instruccion_parseada, contexto, fd_kernel, fd_memoria, config);
		}
	else if(strcmp(instruccion_parseada[0], "F_WRITE") == 0){
			ejecutar_F_WRITE(instruccion_parseada, contexto, fd_kernel, fd_memoria, config);
		}
	else if(strcmp(instruccion_parseada[0], "F_TRUNCATE") == 0){
			ejecutar_F_TRUNCATE(instruccion_parseada, contexto, fd_kernel);
		}
	else if(strcmp(instruccion_parseada[0], "WAIT") == 0){
			ejecutar_WAIT(instruccion_parseada, contexto, fd_kernel);
		}
	else if(strcmp(instruccion_parseada[0], "SIGNAL") == 0){
			ejecutar_SIGNAL(instruccion_parseada, contexto, fd_kernel);
		}
	else if(strcmp(instruccion_parseada[0], "CREATE_SEGMENT") == 0){
			ejecutar_CREATE_SEGMENT(instruccion_parseada, contexto, fd_kernel);
		}
	else if(strcmp(instruccion_parseada[0], "DELETE_SEGMENT") == 0){
				ejecutar_DELETE_SEGMENT(instruccion_parseada, contexto, fd_kernel);
			}
	else if(strcmp(instruccion_parseada[0], "YIELD") == 0){
				ejecutar_YIELD(instruccion_parseada, contexto, fd_kernel);
			}
	else if(strcmp(instruccion_parseada[0], "EXIT") == 0){
				ejecutar_EXIT(instruccion_parseada, contexto, fd_kernel);
			}
}

char* fetch_instruccion(t_contexto_de_ejecucion* contexto){
	char** intrucciones_parseadas = string_split(contexto->instrucciones, "\n");
	char* instruccion_a_ejecutar = intrucciones_parseadas[contexto->pc];
	contexto->pc++;
	return instruccion_a_ejecutar;
}
