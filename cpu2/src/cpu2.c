#include "utils_cpu2.h"

#define MAX_LEN 256

int main(int argc, char **argv) {

	if (argc < 2) {
		printf("ALTO BOT AMIGO\n");
	}

	char* ip_kernel;
	char* puerto_kernel;
    t_log* logger = iniciar_logger();
    t_log* logger_principal = iniciar_logger_principal();
    t_config* config = iniciar_config(argv[1]);
    inicializar_registros();
    inicializar_segmentos_prueba();

	int tamanio_registro_chico = strlen(registros_cpu.ax) + 1;
	int tamanio_registro_mediano = strlen(registros_cpu.eax) + 1;
	int tamanio_registro_grande = strlen(registros_cpu.rax) + 1;

    log_info(logger, "Hola! Se inicializo el modulo cliente CPU.");

    ip_kernel = config_get_string_value(config, "IP_MEMORIA");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

    log_info(logger, "La configuracion de la conexion indica el PUERTO %s y la IP %s", ip_kernel, puerto_kernel);

    char* ip_memoria = config_get_string_value(config, "IP_MEMORIA"); //TODO comentado para probar serialización de segmentos en kernel.
    char* puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    int fd_memoria = crear_conexion(logger, "CPU", ip_memoria, puerto_memoria);

    int fd_cpu = iniciar_servidor(puerto_kernel);
    log_info(logger, "CPU inicializado, esperando a recibir al Kernel en el PUERTO %s.\n", puerto_kernel);
    int fd_kernel = esperar_cliente(logger, "CPU", fd_cpu);


//    int fd_cpu_memoria = iniciar_servidor(puerto_memoria);
//    int fd_memoria = esperar_cliente(logger, "CPU", fd_memoria);

    while(1) {
    	log_info(logger, "CPU esperando a recibir un nuevo proceso para ejecutar.");
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

			//------------------------------------------------------------------------RECIBO EL PID DEL PROCESO
			memcpy(&(contexto->pid), stream, sizeof(int));
			stream += sizeof(int);

			//------------------------------------------------------------------------RECIBO LAS INSTRUCCIONES
			memcpy(&(contexto->tamanio_instrucciones), stream, sizeof(int));
			stream += sizeof(int);

			char *instrucciones = malloc(contexto->tamanio_instrucciones);
			memset(instrucciones, 0, contexto->tamanio_instrucciones);

			memcpy(instrucciones, stream, contexto->tamanio_instrucciones);
			stream += contexto->tamanio_instrucciones;

			contexto->instrucciones = malloc(strlen(instrucciones) + 1);
			strcpy(contexto->instrucciones, instrucciones);

			//------------------------------------------------------------------------RECIBO EL PROGRAM COUNTER
			memcpy(&(contexto->pc), stream, sizeof(int));
			stream += sizeof(int);

			//------------------------------------------------------------------------DESERIALIZACIÓN DE REGISTROS
			memcpy(&(contexto->registros_pcb.ax), stream, tamanio_registro_chico);
			stream += tamanio_registro_chico;
			memcpy(&(contexto->registros_pcb.bx), stream, tamanio_registro_chico);
			stream += tamanio_registro_chico;
			memcpy(&(contexto->registros_pcb.cx), stream, tamanio_registro_chico);
			stream += tamanio_registro_chico;
			memcpy(&(contexto->registros_pcb.dx), stream, tamanio_registro_chico);
			stream += tamanio_registro_chico;

			memcpy(&(contexto->registros_pcb.eax), stream, tamanio_registro_mediano);
			stream += tamanio_registro_mediano;
			memcpy(&(contexto->registros_pcb.ebx), stream, tamanio_registro_mediano);
			stream += tamanio_registro_mediano;
			memcpy(&(contexto->registros_pcb.ecx), stream, tamanio_registro_mediano);
			stream += tamanio_registro_mediano;
			memcpy(&(contexto->registros_pcb.edx), stream, tamanio_registro_mediano);
			stream += tamanio_registro_mediano;

			memcpy(&(contexto->registros_pcb.rax), stream, tamanio_registro_grande);
			stream += tamanio_registro_grande;
			memcpy(&(contexto->registros_pcb.rbx), stream, tamanio_registro_grande);
			stream += tamanio_registro_grande;
			memcpy(&(contexto->registros_pcb.rcx), stream, tamanio_registro_grande);
			stream += tamanio_registro_grande;
			memcpy(&(contexto->registros_pcb.rdx), stream, tamanio_registro_grande);
			stream += tamanio_registro_grande;

//			log_info(logger, "Los registros que trajo PID: <%d> son: ", contexto->pid);
//			log_info(logger, "%s", contexto->registros_pcb.ax);
//			log_info(logger, "%s", contexto->registros_pcb.bx);
//			log_info(logger, "%s", contexto->registros_pcb.cx);
//			log_info(logger, "%s", contexto->registros_pcb.dx);
//			log_info(logger, "%s", contexto->registros_pcb.eax);
//			log_info(logger, "%s", contexto->registros_pcb.ebx);
//			log_info(logger, "%s", contexto->registros_pcb.ecx);
//			log_info(logger, "%s", contexto->registros_pcb.edx);
//			log_info(logger, "%s", contexto->registros_pcb.rax);
//			log_info(logger, "%s", contexto->registros_pcb.rbx);
//			log_info(logger, "%s", contexto->registros_pcb.rcx);
//			log_info(logger, "%s", contexto->registros_pcb.rdx);

			log_info(logger, "PID: <%d> recibido desde el Kernel para ejecutar.", contexto->pid);

			int cant_segmentos;
			memcpy(&cant_segmentos, stream, sizeof(int));
			stream += sizeof(int);

			contexto->tabla_segmentos = list_create();

			for(int m = 0; m < cant_segmentos; m++) {
				t_segmento* aux = malloc(sizeof(t_segmento));

				memcpy(&(aux->base), stream, sizeof(int));
				stream += sizeof(int);
				memcpy(&(aux->id), stream, sizeof(int));
				stream += sizeof(int);
				memcpy(&(aux->tamanio), stream, sizeof(int));
				stream += sizeof(int);

				//log_info(logger, "Segmento <%d>:", m);
				//log_info(logger, "Base <%d>", aux->base);
				//log_info(logger, "ID <%d>", aux->id);
				//log_info(logger, "Tamaño <%d>", aux->tamanio);

				list_add(contexto->tabla_segmentos, aux);
			}

			//contexto->tabla_segmentos = tabla_segmentos_prueba;

			int contexto_sigue_en_cpu = 1; // Es para el while de abajo, si queremos que el contexto vuelva al kernel va a ser 0, rompiendo el ciclo y quedandose el cpu en espera a recibir de nuevo el contexto de ejecución
			char **instrucciones_parseadas = string_split(contexto->instrucciones, "\n");

			while(contexto_sigue_en_cpu) {
				char *instruccion_a_ejecutar = malloc(strlen(instrucciones_parseadas[contexto->pc]) + 1);

				strcpy(instruccion_a_ejecutar, instrucciones_parseadas[contexto->pc]);

				op_code codigo_instruccion = fetch_instruccion(instruccion_a_ejecutar);

				switch(codigo_instruccion) {
				case SET: //TERMINADO - ERA FINITA - FINISHED - WAR VORBEI
					contexto->pc++;
					int tiempo_de_espera;
					tiempo_de_espera = config_get_int_value(config, "RETARDO_INSTRUCCION");
					msleep(tiempo_de_espera); //Definicion en utils_cpu.c permite usar el retardo en milisegundos y no en microsegundos como usleep
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_SET(instruccion_a_ejecutar, contexto);
					break;
				case MOV_IN: //TERMINADA, FALTA PROBARLA DEBUGEANDO Y TODO ESO
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					contexto_sigue_en_cpu = ejecutar_MOV_IN(instruccion_a_ejecutar, contexto, fd_memoria, config, logger_principal, fd_kernel);
					break;
				case MOV_OUT: //TERMINADA, FALTA PROBARLA DEBUGEANDO Y TODO ESO
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					contexto_sigue_en_cpu = ejecutar_MOV_OUT(instruccion_a_ejecutar, contexto, fd_memoria, config, logger_principal, fd_kernel);
					break;
				case IO: //TERMINADO - ERA FINITA - FINISHED - WAR VORBEI
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_IO(instruccion_a_ejecutar, contexto, fd_kernel);
					contexto_sigue_en_cpu = 0;
					break;
				case F_OPEN: //TERMINADA EN CPU - FALTA TERMINARLA EN KERNEL TODO
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_F_OPEN(instruccion_a_ejecutar, contexto, fd_kernel);
					contexto_sigue_en_cpu = 0;
					break;
				case F_CLOSE: //TERMINADA EN CPU - FALTA TERMINARLA EN KERNEL TODO
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_F_CLOSE(instruccion_a_ejecutar, contexto, fd_kernel);
					contexto_sigue_en_cpu = 0;
					break;
				case F_SEEK: //TERMINADA EN CPU - FALTA TERMINARLA EN KERNEL TODO
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_F_SEEK(instruccion_a_ejecutar, contexto, fd_kernel);
					contexto_sigue_en_cpu = 0;
					break;
				case F_READ:
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_F_READ(instruccion_a_ejecutar, contexto, fd_kernel, fd_memoria, config, logger_principal);
					contexto_sigue_en_cpu = 0;
					break;
				case F_WRITE:
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_F_WRITE(instruccion_a_ejecutar, contexto, fd_kernel, fd_memoria, config, logger_principal);
					contexto_sigue_en_cpu = 0;
					break;
				case F_TRUNCATE: //TERMINADA EN CPU - FALTA TERMINARLA EN KERNEL TODO
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_F_TRUNCATE(instruccion_a_ejecutar, contexto, fd_kernel);
					contexto_sigue_en_cpu = 0;
					break;
				case WAIT: //TERMINADA EN CPU - FALTA TERMINARLA EN KERNEL TODO
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_WAIT(instruccion_a_ejecutar, contexto, fd_kernel);
					contexto_sigue_en_cpu = 0;
					break;
				case SIGNAL: //TERMINADA EN CPU - FALTA TERMINARLA EN KERNEL TODO
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_SIGNAL(instruccion_a_ejecutar, contexto, fd_kernel);
					contexto_sigue_en_cpu = 0;
					break;
				case CREATE_SEGMENT:
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_CREATE_SEGMENT(instruccion_a_ejecutar, contexto, fd_kernel);
					contexto_sigue_en_cpu = 0;
					break;
				case DELETE_SEGMENT:
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_DELETE_SEGMENT(instruccion_a_ejecutar, contexto, fd_kernel);
					contexto_sigue_en_cpu = 0;
					break;
				case YIELD: //TERMINADO - ERA FINITA - FINISHED - WAR VORBEI
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_YIELD(instruccion_a_ejecutar, contexto, fd_kernel);
					contexto_sigue_en_cpu = 0;
					break;
				case EXIT: //TERMINADO - ERA FINITA - FINISHED - WAR VORBEI
					contexto->pc++;
					log_info(logger_principal, "PID: <%d> - Ejecutando: <%s>", contexto->pid, instruccion_a_ejecutar);
					ejecutar_EXIT(instruccion_a_ejecutar, contexto, fd_kernel);
					contexto_sigue_en_cpu = 0;
					break;
				default:
					log_error(logger, "[ERROR] Instruccion desconocida, finalizando el módulo CPU");
					return 1;
					break;
				}

				free(instruccion_a_ejecutar);
			}

//			log_warning(logger, "AX: %s", registros_cpu.ax);
//			log_warning(logger, "BX: %s", registros_cpu.bx);
//			log_warning(logger, "CX: %s", registros_cpu.cx);
//			log_warning(logger, "DX: %s", registros_cpu.dx);
//			log_warning(logger, "EAX: %s", registros_cpu.eax);
//			log_warning(logger, "EBX: %s", registros_cpu.ebx);
//			log_warning(logger, "ECX: %s", registros_cpu.ecx);
//			log_warning(logger, "EDX: %s", registros_cpu.edx);
//			log_warning(logger, "RAX: %s", registros_cpu.rax);
//			log_warning(logger, "RBX: %s", registros_cpu.rbx);
//			log_warning(logger, "RCX: %s", registros_cpu.rcx);
//			log_warning(logger, "RDX: %s", registros_cpu.rdx);
			break;
		default:
			log_error(logger, "[ERROR] Se desconectó Kernel, chau loco suerte.");
			return 2;
		}

		eliminar_paquete(paquete);
		free(contexto);

	}

	return 0;
}

