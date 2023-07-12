#include "utils_cpu2.h"

#define MAX_LEN 256

int main() {
	bool cpu_bloqueada = false;
	char* ip_kernel;
	char* puerto_kernel;
    t_log* logger = iniciar_logger();
    t_config* config = iniciar_config();
    inicializar_registros();

    log_info(logger, "Hola! Se inicializo el modulo cliente CPU.");

    ip_kernel = config_get_string_value(config, "IP");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

    log_info(logger, "La configuracion de la conexion indica el PUERTO %s y la IP %s", ip_kernel, puerto_kernel);

    int fd_cpu = iniciar_servidor(puerto_kernel);
    log_info(logger, "CPU inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_kernel);
    int fd_kernel = esperar_cliente(logger, "CPU", fd_cpu);


    //int ip_memoria = config_get_string_value(config, "IP");
    //int puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    int fd_memoria = 4; //crear_conexion(logger, "CPU", ip_memoria, puerto_memoria);
//    int fd_cpu_memoria = iniciar_servidor(puerto_memoria);
//    int fd_memoria = esperar_cliente(logger, "CPU", fd_memoria);

    while(1) {
    	log_info(logger, "CPU esperando a recibir un nuevo contexto de ejecución");
    	t_paquete* paquete = malloc(sizeof(t_paquete));
    	paquete->buffer = malloc(sizeof(t_buffer));
    	recv(fd_kernel, &(paquete->codigo_operacion), sizeof(op_code), MSG_WAITALL);
    	recv(fd_kernel, &(paquete->buffer->stream_size), sizeof(int), 0);
    	paquete->buffer->stream = malloc(paquete->buffer->stream_size);
    	recv(fd_kernel, paquete->buffer->stream, paquete->buffer->stream_size, 0);

    	t_contexto_de_ejecucion* contexto = malloc(sizeof(t_contexto_de_ejecucion));

		switch (paquete->codigo_operacion) {
		case CONTEXTO_DE_EJECUCION:
			void *stream = paquete->buffer->stream;

			//--------------------------------------------------------------------RECIBO EL PID DEL PROCESO
			memcpy(&(contexto->pid), stream, sizeof(int));
			stream += sizeof(int);

			//--------------------------------------------------------------------RECIBO LAS INSTRUCCIONES
			memcpy(&(contexto->tamanio_instrucciones), stream, sizeof(int));
			stream += sizeof(int);

			char *instrucciones = malloc(contexto->tamanio_instrucciones);
			memset(instrucciones, 0, contexto->tamanio_instrucciones);

			memcpy(instrucciones, stream, contexto->tamanio_instrucciones);
			stream += contexto->tamanio_instrucciones;

			contexto->instrucciones = malloc(strlen(instrucciones) + 1);
			strcpy(contexto->instrucciones, instrucciones);

			//--------------------------------------------------------------------RECIBO EL PROGRAM COUNTER
			memcpy(&(contexto->pc), stream, sizeof(int));
			stream += sizeof(int);

			//--------------------------------------------------------------------DESERIALIZACIÓN DE SEGMENTOS Y DE REGISTROS, FALTA ARREGLARLO
//    				t_registros* registro_actual = contexto->registros_pcb;
//    				for(int i=0; i<(contexto->tamanio_registros); i++){
//    					memcpy(&(registro_actual), stream, sizeof(t_registros));
//    					stream += sizeof(t_registros);
//    					registro_actual++;
//    				}
//
//    				for(int a=0; a<(list_size(contexto->tabla_segmentos)); a++){
//    					t_segmento* segmento_actual = list_get(contexto->tabla_segmentos, a);
//    					deserializar_segmentos(segmento_actual, stream);
//    						}

			log_info(logger, "Recibido Proceso id[%d] desde Kernel", contexto->pid);

			int contexto_sigue_en_cpu = 1; // Es para el while de abajo, si queremos que el contexto vuelva al kernel va a ser 0, rompiendo el ciclo y quedandose el cpu en espera a recibir de nuevo el contexto de ejecución
			char **instrucciones_parseadas = string_split(contexto->instrucciones, "\n");

			while(contexto_sigue_en_cpu) {
				char *instruccion_a_ejecutar = malloc(strlen(instrucciones_parseadas[contexto->pc]) + 1);

				strcpy(instruccion_a_ejecutar, instrucciones_parseadas[contexto->pc]);

				op_code codigo_instruccion = fetch_instruccion(instruccion_a_ejecutar);

				switch(codigo_instruccion) {
				case SET: //TERMINADO
					contexto->pc++;
					int tiempo_de_espera;
					tiempo_de_espera = config_get_int_value(config, "RETARDO_INSTRUCCION");
					usleep(tiempo_de_espera); //Usamos usleep que esta recibe el parámetro en milisegundos, como pedía el enunciado.
					ejecutar_SET(instruccion_a_ejecutar, contexto);
					log_warning(logger, "Se ejecuta la instruccion SET");
					break;
				case MOV_IN:
					contexto->pc++;
//					ejecutar_MOV_IN(instruccion_a_ejecutar, contexto, fd_memoria, config);
					log_warning(logger, "Se ejecuta la instruccion MOV_IN");
					break;
				case MOV_OUT:
					contexto->pc++;
//					ejecutar_MOV_OUT(instruccion_a_ejecutar, contexto, fd_memoria, config);
					log_warning(logger, "Se ejecuta la instruccion MOV_OUT");
					break;
				case IO:
					contexto->pc++;
//					ejecutar_IO(instruccion_a_ejecutar, contexto, fd_kernel, cpu_bloqueada);
					log_warning(logger, "Se ejecuta la instruccion IO");
					contexto_sigue_en_cpu = 0;
					break;
				case F_OPEN:
					contexto->pc++;
//					ejecutar_F_OPEN(instruccion_a_ejecutar, contexto, fd_kernel);
					log_warning(logger, "Se ejecuta la instruccion F_OPEN");
					break;
				case F_CLOSE:
					contexto->pc++;
//					ejecutar_F_CLOSE(instruccion_a_ejecutar, contexto, fd_kernel);
					log_warning(logger, "Se ejecuta la instruccion F_CLOSE");
					break;
				case F_SEEK:
					contexto->pc++;
//					ejecutar_F_SEEK(instruccion_a_ejecutar, contexto, fd_kernel);
					log_warning(logger, "Se ejecuta la instruccion F_SEEK");
					break;
				case F_READ:
					contexto->pc++;
//					ejecutar_F_READ(instruccion_a_ejecutar, contexto, fd_kernel, fd_memoria, config);
					log_warning(logger, "Se ejecuta la instruccion F_READ");
					break;
				case F_WRITE:
					contexto->pc++;
//					ejecutar_F_WRITE(instruccion_a_ejecutar, contexto, fd_kernel, fd_memoria, config);
					log_warning(logger, "Se ejecuta la instruccion F_WRITE");
					break;
				case F_TRUNCATE:
					contexto->pc++;
//					ejecutar_F_TRUNCATE(instruccion_a_ejecutar, contexto, fd_kernel);
					log_warning(logger, "Se ejecuta la instruccion F_TRUNCATE");
					break;
				case WAIT:
					contexto->pc++;
//					ejecutar_WAIT(instruccion_a_ejecutar, contexto, fd_kernel);
					log_warning(logger, "Se ejecuta la instruccion WAIT");
					break;
				case SIGNAL:
					contexto->pc++;
//					ejecutar_SIGNAL(instruccion_a_ejecutar, contexto, fd_kernel);
					log_warning(logger, "Se ejecuta la instruccion SIGNAL");
					break;
				case CREATE_SEGMENT:
					contexto->pc++;
//					ejecutar_CREATE_SEGMENT(instruccion_a_ejecutar, contexto, fd_kernel);
					log_warning(logger, "Se ejecuta la instruccion CREATE_SEGMENT");
					break;
				case DELETE_SEGMENT:
					contexto->pc++;
//					ejecutar_DELETE_SEGMENT(instruccion_a_ejecutar, contexto, fd_kernel);
					log_warning(logger, "Se ejecuta la instruccion DELETE_SEGMENT");
					break;
				case YIELD:
					contexto->pc++;
					ejecutar_YIELD(instruccion_a_ejecutar, contexto, fd_kernel);
					log_warning(logger, "Se ejecuta la instruccion YIELD");
					contexto_sigue_en_cpu = 0;
					break;
				case EXIT:
					contexto->pc++;
					ejecutar_EXIT(instruccion_a_ejecutar, contexto, fd_kernel);
					log_warning(logger, "Se ejecuta la instruccion EXIT");
					break;
				default:
					log_error(logger, "instruccion desconocida");
					break;
				}

			}

			log_warning(logger, "AX: %s", registros_cpu.ax);
			log_warning(logger, "BX: %s", registros_cpu.bx);
			log_warning(logger, "CX: %s", registros_cpu.cx);
			log_warning(logger, "DX: %s", registros_cpu.dx);
			log_warning(logger, "EAX: %s", registros_cpu.eax);
			log_warning(logger, "EBX: %s", registros_cpu.ebx);

		}
	}

	return 0;
}

