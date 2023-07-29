#include"utils_kernel2.h"
#include<shared-2.h>

// ------------------------------------- ESTADO DE EJECUCIÓN, LO MÁS IMPORTANTE --------------------------------------------------------------------------------------

void estado_ejecutar(void) {

	while (1) {
		sem_wait(&sem_exec);
		pthread_mutex_lock(&mutex_exec);
		t_proceso* proceso_a_ejecutar = list_get(cola_exec, 0); //esto ayuda para cuando se necesita devolver el mismo proceso a cpu, como en las instrucciones de memoria
		// ME CAGÓ LA VIDA DURANTE UN BUEN RATO EL LIST_REMOVE HABÍA PUESTO LIST_GET Y NO ANDABA LA CONCHA DE LA LORA!"!!!!!!!!!"°!"°!"
		// 27/07/23 ME FUI DOMADÍSIMO UNA VEZ MÁS Y TUVO QUE VOLVER EL LIST_GET COMO MVP DE ESTADO_EXEC
		pthread_mutex_unlock(&mutex_exec);

		proceso_a_ejecutar->pcb->estado = EXEC;

	 	if(proceso_a_ejecutar->desalojado == DESALOJADO){
	 		int inicio_cpu = get_time();
	 		proceso_a_ejecutar->principio_ultima_rafaga = inicio_cpu;
	 	}
		
		enviar_pcb(socket_cpu, proceso_a_ejecutar->pcb);
		log_info(logger_kernel, "PID: <%d> enviada a CPU", proceso_a_ejecutar->pcb->pid);

		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));
		recv(socket_cpu, &(paquete->codigo_operacion), sizeof(op_code), MSG_WAITALL);
		recv(socket_cpu, &(paquete->buffer->stream_size), sizeof(int), 0);
		paquete->buffer->stream = malloc(paquete->buffer->stream_size);
		recv(socket_cpu, paquete->buffer->stream, paquete->buffer->stream_size, 0);

		op_code respuesta_cpu = paquete->codigo_operacion;

		void *stream = paquete->buffer->stream;

		//-------------------------------------------------------------RECIBO EL PID
		memcpy(&(proceso_a_ejecutar->pcb->pid), stream, sizeof(int));
		stream += sizeof(int);

		//-------------------------------------------------------------RECIBO EL PROGRAM COUNTER
		memcpy(&(proceso_a_ejecutar->pcb->pc), stream, sizeof(int));
		stream += sizeof(int);

		//-------------------------------------------------------------RECIBO INSTRUCCIONES
		memcpy(&(proceso_a_ejecutar->pcb->tamanio_instrucciones), stream, sizeof(int));
		stream += sizeof(int);

		char* instrucciones = malloc(proceso_a_ejecutar->pcb->tamanio_instrucciones);
		memset(instrucciones, 0, proceso_a_ejecutar->pcb->tamanio_instrucciones);

		memcpy(instrucciones, stream, proceso_a_ejecutar->pcb->tamanio_instrucciones);
		stream += proceso_a_ejecutar->pcb->tamanio_instrucciones;

		proceso_a_ejecutar->pcb->instrucciones = malloc(strlen(instrucciones) + 1);
		strcpy(proceso_a_ejecutar->pcb->instrucciones, instrucciones);

		//-------------------------------------------------------------RECIBO REGISTROS
		int tamanio_registro_chico = strlen(registros_iniciados.ax) + 1;
		int tamanio_registro_mediano = strlen(registros_iniciados.eax) + 1;
		int tamanio_registro_grande = strlen(registros_iniciados.rax) + 1;

		memcpy(&(proceso_a_ejecutar->pcb->registros.ax), stream, tamanio_registro_chico);
		stream += tamanio_registro_chico;
		memcpy(&(proceso_a_ejecutar->pcb->registros.bx), stream, tamanio_registro_chico);
		stream += tamanio_registro_chico;
		memcpy(&(proceso_a_ejecutar->pcb->registros.cx), stream, tamanio_registro_chico);
		stream += tamanio_registro_chico;
		memcpy(&(proceso_a_ejecutar->pcb->registros.dx), stream, tamanio_registro_chico);
		stream += tamanio_registro_chico;

		memcpy(&(proceso_a_ejecutar->pcb->registros.eax), stream, tamanio_registro_mediano);
		stream += tamanio_registro_mediano;
		memcpy(&(proceso_a_ejecutar->pcb->registros.ebx), stream, tamanio_registro_mediano);
		stream += tamanio_registro_mediano;
		memcpy(&(proceso_a_ejecutar->pcb->registros.ecx), stream, tamanio_registro_mediano);
		stream += tamanio_registro_mediano;
		memcpy(&(proceso_a_ejecutar->pcb->registros.edx), stream, tamanio_registro_mediano);
		stream += tamanio_registro_mediano;

		memcpy(&(proceso_a_ejecutar->pcb->registros.rax), stream, tamanio_registro_grande);
		stream += tamanio_registro_grande;
		memcpy(&(proceso_a_ejecutar->pcb->registros.rbx), stream, tamanio_registro_grande);
		stream += tamanio_registro_grande;
		memcpy(&(proceso_a_ejecutar->pcb->registros.rcx), stream, tamanio_registro_grande);
		stream += tamanio_registro_grande;
		memcpy(&(proceso_a_ejecutar->pcb->registros.rdx), stream, tamanio_registro_grande);
		stream += tamanio_registro_grande;

//		log_info(logger_kernel, "Los registros que trajo PID: <%d> de la CPU son: ", proceso_a_ejecutar->pcb->pid);
//		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.ax);
//		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.bx);
//		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.cx);
//		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.dx);
//		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.eax);
//		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.ebx);
//		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.ecx);
//		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.edx);
//		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.rax);
//		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.rbx);
//		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.rcx);
//		log_info(logger_kernel, "%s", proceso_a_ejecutar->pcb->registros.rdx);

		//--- FALTA DESERIALIZAR SEGMENTOS JAJSDJASJDAJSD SISI YA LOS DESERIALIZO
		// 24/07/23 ME FUI DOMADISIMO Y TENGO QUE SERIALIZAR SEGMENTOS AL FINAL

		int cant_segmentos;
		memcpy(&cant_segmentos, stream, sizeof(int));
		stream += sizeof(int);

		list_clean_and_destroy_elements(proceso_a_ejecutar->pcb->tabla_segmentos.segmentos, free);

		for(int m = 0; m < cant_segmentos; m++) {
			t_segmento* aux = malloc(sizeof(t_segmento));

			memcpy(&(aux->base), stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&(aux->id), stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&(aux->tamanio), stream, sizeof(int));
			stream += sizeof(int);

//			log_info(logger_kernel, "Segmento <%d>:", m);
//			log_info(logger_kernel, "Base <%d>", aux->base);
//			log_info(logger_kernel, "ID <%d>", aux->id);
//			log_info(logger_kernel, "Tamaño <%d>", aux->tamanio);

			list_add(proceso_a_ejecutar->pcb->tabla_segmentos.segmentos, aux);
		}

		switch (respuesta_cpu) {
		case IO: //TERMINADO
			proceso_a_ejecutar->desalojado = DESALOJADO;
			list_remove(cola_exec, 0);
			int finalizacion_cpu = get_time();
			proceso_a_ejecutar->final_ultima_rafaga = finalizacion_cpu;
			proceso_a_ejecutar->ultima_rafaga = proceso_a_ejecutar->final_ultima_rafaga - proceso_a_ejecutar->principio_ultima_rafaga;
			calcular_estimacion(proceso_a_ejecutar);

			proceso_a_ejecutar->ultima_instruccion = IO;

			int inicio_block = get_time();
			proceso_a_ejecutar->inicio_bloqueo = inicio_block;
			
			memcpy(&(proceso_a_ejecutar->tiempo_bloqueo), stream, sizeof(int));
			stream += sizeof(int);

			pthread_mutex_lock(&mutex_block_io);
			proceso_a_ejecutar->pcb->estado = BLOCK;
			list_add(cola_block_io, proceso_a_ejecutar);
			pthread_mutex_unlock(&mutex_block_io);

			log_info(kernel_principal, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCK>", proceso_a_ejecutar->pcb->pid);
			log_info(kernel_principal, "PID: <%d> - Bloqueado por: <IO>", proceso_a_ejecutar->pcb->pid);

			sem_post(&sem_ready);
			sem_post(&sem_block_io);
			break;
		case CREATE_SEGMENT:
			proceso_a_ejecutar->ultima_instruccion = CREATE_SEGMENT;
			proceso_a_ejecutar->desalojado = NO_DESALOJADO;

			int id_segmento;
			int tamanio_segmento;

			memcpy(&id_segmento, stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&tamanio_segmento, stream, sizeof(int));
			stream += sizeof(int);

			log_info(kernel_principal, "PID: <%d> - Crear Segmento - Id: <%d> - Tamaño: <%d>", proceso_a_ejecutar->pcb->pid, id_segmento, tamanio_segmento);

			pthread_mutex_lock(&mutex_operacion_memoria);
			enviar_parametros_a_memoria(proceso_a_ejecutar->pcb->pid, id_segmento, tamanio_segmento, CREATE_SEGMENT);

			recibir_respuesta_create(proceso_a_ejecutar, id_segmento, tamanio_segmento);
			pthread_mutex_unlock(&mutex_operacion_memoria);

//			pthread_mutex_lock(&mutex_exec);
//			list_add(cola_exec, proceso_a_ejecutar);
//			pthread_mutex_unlock(&mutex_exec);

//			sem_post(&sem_exec);
			break;
		case DELETE_SEGMENT:
			proceso_a_ejecutar->ultima_instruccion = DELETE_SEGMENT;
			proceso_a_ejecutar->desalojado = NO_DESALOJADO;

			int id_segmento_a_borrar;

			memcpy(&id_segmento_a_borrar, stream, sizeof(int));
			stream += sizeof(int);

			log_info(kernel_principal, "PID: <%d> - Eliminar Segmento - Id Segmento: <%d>", proceso_a_ejecutar->pcb->pid, id_segmento_a_borrar);

			pthread_mutex_lock(&mutex_operacion_memoria);
			enviar_parametros_a_memoria(proceso_a_ejecutar->pcb->pid, id_segmento_a_borrar, 0, DELETE_SEGMENT); //pongo un 0 en el tercer parámetro porque si es DELETE_SEGMENT no interfiere para nada

			recibir_tablas_delete(proceso_a_ejecutar->pcb);
			pthread_mutex_unlock(&mutex_operacion_memoria);

//			pthread_mutex_lock(&mutex_exec);
//			list_add(cola_exec, proceso_a_ejecutar);
//			pthread_mutex_unlock(&mutex_exec);

			sem_post(&sem_exec);
			break;
		case F_OPEN:
			//Abrir o crear el archivo pasado por parámetro
			//TODO Calculo para el HRRN, si se bloquea, se desaloja y se termina la rafaga del cpu

			proceso_a_ejecutar->ultima_instruccion = F_OPEN;
			
			int tamanio_archivo_a_abrir;

			memcpy(&tamanio_archivo_a_abrir, stream, sizeof(int));
			stream += sizeof(int);

			char* nombre_archivo_a_abrir = malloc(tamanio_archivo_a_abrir);
			memset(nombre_archivo_a_abrir, 0, tamanio_archivo_a_abrir);

			memcpy(nombre_archivo_a_abrir, stream, tamanio_archivo_a_abrir);
			stream += tamanio_archivo_a_abrir;

			log_warning(kernel_principal, "PID: <%d> - Abrir Archivo: <%s>", proceso_a_ejecutar->pcb->pid, nombre_archivo_a_abrir);

			if(dictionary_has_key(tabla_global_archivos, nombre_archivo_a_abrir)) {
				t_archivo_proceso* entrada_archivo = malloc(sizeof(t_archivo_proceso));
				entrada_archivo->nombre = nombre_archivo_a_abrir;
				entrada_archivo->puntero = 0;
				entrada_archivo->tamanio = 0; //Creo que para la entrada no es necesario el tamaño pero lo dejo por si las moscas.

				dictionary_put(proceso_a_ejecutar->pcb->tabla_archivos, nombre_archivo_a_abrir, entrada_archivo);

				t_archivo* archivo_open = dictionary_get(tabla_global_archivos, nombre_archivo_a_abrir);

				proceso_a_ejecutar->desalojado = DESALOJADO;
				list_remove(cola_exec, 0);
				proceso_a_ejecutar->final_ultima_rafaga = get_time();
				proceso_a_ejecutar->ultima_rafaga = proceso_a_ejecutar->final_ultima_rafaga - proceso_a_ejecutar->principio_ultima_rafaga;
				calcular_estimacion(proceso_a_ejecutar);

				log_info(kernel_principal, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <BLOCK>", proceso_a_ejecutar->pcb->pid);
				log_info(kernel_principal, "PID: <%d> - Bloqueado por: <%s>", proceso_a_ejecutar->pcb->pid, nombre_archivo_a_abrir);

				queue_push(archivo_open->cola_bloqueados, proceso_a_ejecutar);
				sem_post(&sem_ready);
			} else {
				pthread_mutex_lock(&mutex_operacion_file_system);

				op_code respuesta_fs_open = consultar_existencia_archivo_a_fs(nombre_archivo_a_abrir);

				t_archivo_proceso* entrada_archivo_open = malloc(sizeof(t_archivo_proceso));
				entrada_archivo_open->nombre = nombre_archivo_a_abrir;
				entrada_archivo_open->puntero = 0;
				entrada_archivo_open->tamanio = 0; //Creo que para la entrada no es necesario el tamaño pero lo dejo por si las moscas.

				switch(respuesta_fs_open) {
				case EXISTE:
					agregar_archivo_a_tabla_global_archivos(nombre_archivo_a_abrir);

					break;
				case NO_EXISTE:
					crear_archivo_en_fs(nombre_archivo_a_abrir);
					agregar_archivo_a_tabla_global_archivos(nombre_archivo_a_abrir);

					break;
				default:
					log_error(logger_kernel, "[ERROR] Fallo al recibir la respuesta de FileSystem, arreglate hermano.");

					break;
				}

				pthread_mutex_unlock(&mutex_operacion_file_system);

				proceso_a_ejecutar->desalojado = NO_DESALOJADO;

				dictionary_put(proceso_a_ejecutar->pcb->tabla_archivos, nombre_archivo_a_abrir, entrada_archivo_open);

//				pthread_mutex_lock(&mutex_exec);
//				list_add(cola_exec, proceso_a_ejecutar);
//				pthread_mutex_unlock(&mutex_exec);

				sem_post(&sem_exec);
			}

			break;
		case F_CLOSE:
			//Cerrar el archivo pasado por parámetro
			//TODO Calculo para el HRRN, si se bloquea, se desaloja y se termina la rafaga del cpu

			proceso_a_ejecutar->ultima_instruccion = F_CLOSE;
			proceso_a_ejecutar->desalojado = NO_DESALOJADO;
			
			int tamanio_archivo_a_cerrar;

			memcpy(&tamanio_archivo_a_cerrar, stream, sizeof(int));
			stream += sizeof(int);

			char* nombre_archivo_a_cerrar = malloc(tamanio_archivo_a_cerrar);
			memset(nombre_archivo_a_cerrar, 0, tamanio_archivo_a_cerrar);

			memcpy(nombre_archivo_a_cerrar, stream, tamanio_archivo_a_cerrar);
			stream += tamanio_archivo_a_cerrar;

			log_warning(kernel_principal, "PID: <%d> - Cerrar Archivo: <%s>", proceso_a_ejecutar->pcb->pid, nombre_archivo_a_cerrar);

			quitar_entrada_archivo_del_proceso(nombre_archivo_a_cerrar, proceso_a_ejecutar);

			t_archivo* archivo_close = dictionary_get(tabla_global_archivos, nombre_archivo_a_cerrar);

			if(queue_is_empty(archivo_close->cola_bloqueados)) {
				quitar_entrada_archivo_de_tabla_global_archivos(nombre_archivo_a_cerrar);
			} else {
				liberar_entrada_archivo(archivo_close); //esta funcion es como el liberar instancia del recurso
			}

//			pthread_mutex_lock(&mutex_exec);
//			list_add(cola_exec, proceso_a_ejecutar);
//			pthread_mutex_unlock(&mutex_exec);

			sem_post(&sem_exec);
			break;
		case F_SEEK:
			//Actualizar el puntero del archivo a la posición pasada por parámetro
			//TODO Calculo para el HRRN, si se bloquea, se desaloja y se termina la rafaga del cpu
			proceso_a_ejecutar->ultima_instruccion = F_SEEK;
			proceso_a_ejecutar->desalojado = NO_DESALOJADO;
			
			int tamanio_archivo_seek;
			int tamanio_en_bytes;

			memcpy(&tamanio_archivo_seek, stream, sizeof(int));
			stream += sizeof(int);

			char* nombre_archivo_seek = malloc(tamanio_archivo_seek);
			memset(nombre_archivo_seek, 0, tamanio_archivo_seek);

			memcpy(nombre_archivo_seek, stream, tamanio_archivo_seek);
			stream += tamanio_archivo_seek;

			memcpy(&tamanio_en_bytes, stream, sizeof(int));
			stream += sizeof(int);

			actualizar_puntero_archivo_proceso(proceso_a_ejecutar, nombre_archivo_seek, tamanio_en_bytes);

			log_warning(kernel_principal, "PID: <%d> - Actualizar puntero Archivo: <%s> - Puntero <%d>", proceso_a_ejecutar->pcb->pid, nombre_archivo_seek, tamanio_en_bytes);

//			pthread_mutex_lock(&mutex_exec);
//			list_add(cola_exec, proceso_a_ejecutar);
//			pthread_mutex_unlock(&mutex_exec);

			sem_post(&sem_exec);
			break;
		case F_READ:
			//Leer del archivo indicado la cantidad de bytes pasados por parámetro y escribirlo en la dirección física de memoria
			//TODO Calculo para el HRRN, si se bloquea, se desaloja y se termina la rafaga del cpu

			proceso_a_ejecutar->ultima_instruccion = F_READ;
			proceso_a_ejecutar->desalojado = DESALOJADO;
			proceso_a_ejecutar->final_ultima_rafaga = get_time();
			proceso_a_ejecutar->ultima_rafaga = proceso_a_ejecutar->final_ultima_rafaga - proceso_a_ejecutar->principio_ultima_rafaga;
			calcular_estimacion(proceso_a_ejecutar);

			int tamanio_archivo_read;
			int direccion_fisica_read;
			int cantidad_bytes_read;

			memcpy(&tamanio_archivo_read, stream, sizeof(int));
			stream += sizeof(int);

			char* nombre_archivo_read = malloc(tamanio_archivo_read);
			memset(nombre_archivo_read, 0, tamanio_archivo_read);

			memcpy(nombre_archivo_read, stream, tamanio_archivo_read);
			stream += tamanio_archivo_read;

			memcpy(&direccion_fisica_read, stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&cantidad_bytes_read, stream, sizeof(int));
			stream += sizeof(int);

			t_peticion* peticion_read = malloc(sizeof(t_peticion));
			t_archivo_proceso* archivo_read = dictionary_get(proceso_a_ejecutar->pcb->tabla_archivos, nombre_archivo_read);

			peticion_read->proceso = proceso_a_ejecutar;
			peticion_read->cantidad_bytes = cantidad_bytes_read;
			peticion_read->codigo_operacion = F_READ;
			peticion_read->direccion_fisica = direccion_fisica_read;
			peticion_read->nombre_archivo = nombre_archivo_read;
			peticion_read->puntero = archivo_read->puntero;

			log_warning(kernel_principal, "PID: <%d> - Leer Archivo: <%s> - Puntero <%d> - Dirección Memoria <%d> - Tamaño <%d>", proceso_a_ejecutar->pcb->pid, nombre_archivo_read, archivo_read->puntero, direccion_fisica_read, cantidad_bytes_read);

			pthread_mutex_lock(&mutex_peticiones_fs);
			queue_push(cola_peticiones_file_system, peticion_read);
			pthread_mutex_unlock(&mutex_peticiones_fs);

			pthread_mutex_lock(&mutex_exec);
			list_remove(cola_exec,0);
			pthread_mutex_unlock(&mutex_exec);

//			pthread_mutex_lock(&mutex_exec);
//			list_add(cola_exec, proceso_a_ejecutar);
//			pthread_mutex_unlock(&mutex_exec);

//			sem_post(&sem_exec);

			sem_post(&sem_peticiones_file_system);
			sem_post(&sem_ready);
			break;
		case F_WRITE:
			//Esta instrucción solicita al Kernel que se escriba en el archivo indicado la cantidad de bytes pasados por parámetro cuya información es obtenida a partir de la dirección física de Memoria.
			//TODO Calculo para el HRRN, si se bloquea, se desaloja y se termina la rafaga del cpu

			proceso_a_ejecutar->ultima_instruccion = F_WRITE;
			proceso_a_ejecutar->desalojado = DESALOJADO;
			proceso_a_ejecutar->final_ultima_rafaga = get_time();
			proceso_a_ejecutar->ultima_rafaga = proceso_a_ejecutar->final_ultima_rafaga - proceso_a_ejecutar->principio_ultima_rafaga;
			calcular_estimacion(proceso_a_ejecutar);

			int tamanio_archivo_write;
			int direccion_fisica_write;
			int cantidad_bytes_write;

			memcpy(&tamanio_archivo_write, stream, sizeof(int));
			stream += sizeof(int);

			char* nombre_archivo_write = malloc(tamanio_archivo_write);
			memset(nombre_archivo_write, 0, tamanio_archivo_write);

			memcpy(nombre_archivo_write, stream, tamanio_archivo_write);
			stream += tamanio_archivo_write;

			memcpy(&direccion_fisica_write, stream, sizeof(int));
			stream += sizeof(int);
			memcpy(&cantidad_bytes_write, stream, sizeof(int));
			stream += sizeof(int);

//			pthread_mutex_lock(&mutex_exec);
//			list_add(cola_exec, proceso_a_ejecutar);
//			pthread_mutex_unlock(&mutex_exec);
//
//			sem_post(&sem_exec);
			t_peticion* peticion_write = malloc(sizeof(t_peticion));
			t_archivo_proceso* archivo_write = dictionary_get(proceso_a_ejecutar->pcb->tabla_archivos, nombre_archivo_write);

			peticion_write->proceso = proceso_a_ejecutar;
			peticion_write->cantidad_bytes = cantidad_bytes_write;
			peticion_write->codigo_operacion = F_WRITE;
			peticion_write->direccion_fisica = direccion_fisica_write;
			peticion_write->nombre_archivo = nombre_archivo_write;
			peticion_write->puntero = archivo_write->puntero;

			log_warning(kernel_principal, "PID: <%d> - Escribir Archivo: <%s> - Puntero <%d> - Dirección Memoria <%d> - Tamaño <%d>", proceso_a_ejecutar->pcb->pid, nombre_archivo_write, archivo_write->puntero, direccion_fisica_write, cantidad_bytes_write);

			pthread_mutex_lock(&mutex_peticiones_fs);
			queue_push(cola_peticiones_file_system, peticion_write);
			pthread_mutex_unlock(&mutex_peticiones_fs);

			pthread_mutex_lock(&mutex_exec);
			list_remove(cola_exec,0);
			pthread_mutex_unlock(&mutex_exec);

			sem_post(&sem_peticiones_file_system);
			sem_post(&sem_ready);
			break;
		case F_TRUNCATE:
			//Modificar el tamaño del archivo al indicado por parámetro.
			//TODO Calculo para el HRRN, si se bloquea, se desaloja y se termina la rafaga del cpu
			proceso_a_ejecutar->ultima_instruccion = F_TRUNCATE;
			proceso_a_ejecutar->desalojado = DESALOJADO;
			proceso_a_ejecutar->final_ultima_rafaga = get_time();
			proceso_a_ejecutar->ultima_rafaga = proceso_a_ejecutar->final_ultima_rafaga - proceso_a_ejecutar->principio_ultima_rafaga;
			calcular_estimacion(proceso_a_ejecutar);

			int tamanio_archivo_a_truncar;
			int nuevo_tamanio;

			memcpy(&tamanio_archivo_a_truncar, stream, sizeof(int));
			stream += sizeof(int);

			char* nombre_archivo_a_truncar = malloc(tamanio_archivo_a_truncar);
			memset(nombre_archivo_a_truncar, 0, tamanio_archivo_a_truncar);

			memcpy(nombre_archivo_a_truncar, stream, tamanio_archivo_a_truncar);
			stream += tamanio_archivo_a_truncar;

			memcpy(&nuevo_tamanio, stream, sizeof(int));
			stream += sizeof(int);

			
			log_warning(kernel_principal, "PID: <%d> - Archivo: <%s> - Tamaño: <%d>", proceso_a_ejecutar->pcb->pid, nombre_archivo_a_truncar, nuevo_tamanio);
//			pthread_mutex_lock(&mutex_exec);
//			list_add(cola_exec, proceso_a_ejecutar);
//			pthread_mutex_unlock(&mutex_exec);
//
//			sem_post(&sem_exec);

			t_peticion* peticion_truncate = malloc(sizeof(t_peticion));

			peticion_truncate->proceso = proceso_a_ejecutar;
			peticion_truncate->codigo_operacion = F_TRUNCATE;
			peticion_truncate->nombre_archivo = nombre_archivo_a_truncar;
			peticion_truncate->nuevo_tamanio_truncate = nuevo_tamanio;

			//peticion_read->puntero = puntero_read;

			pthread_mutex_lock(&mutex_peticiones_fs);
			queue_push(cola_peticiones_file_system, peticion_truncate);
			pthread_mutex_unlock(&mutex_peticiones_fs);

			pthread_mutex_lock(&mutex_exec);
			list_remove(cola_exec,0);
			pthread_mutex_unlock(&mutex_exec);

			sem_post(&sem_peticiones_file_system);
			sem_post(&sem_ready);
			break;
		case WAIT:
			//Se asigne una instancia del recurso indicado por parámetro
			//TODO Calculo para el HRRN, si se bloquea, se desaloja y se termina la rafaga del cpu

			proceso_a_ejecutar->ultima_instruccion = WAIT;
			
			int tamanio_parametro_wait = 0;

			memcpy(&tamanio_parametro_wait, stream, sizeof(int));
			stream += sizeof(int);

			char* recurso_wait = malloc(tamanio_parametro_wait);
			memset(recurso_wait, 0, tamanio_parametro_wait);

			memcpy(recurso_wait, stream, tamanio_parametro_wait);
			stream += tamanio_parametro_wait;

			log_warning(logger_kernel, "PID: <%d> solicita una instancia del Recurso: <%s>", proceso_a_ejecutar->pcb->pid, recurso_wait);

			if (dictionary_has_key(recursos, recurso_wait)) {
				//Lógica para WAIT TODO Creo que es por acá
				t_recurso* recurso_a_pedir = dictionary_get(recursos, recurso_wait);

				if(asignar_recurso(recurso_a_pedir, proceso_a_ejecutar)) {
					proceso_a_ejecutar->desalojado = NO_DESALOJADO;

//					pthread_mutex_lock(&mutex_exec);
//					list_add(cola_exec, proceso_a_ejecutar);
//					pthread_mutex_unlock(&mutex_exec);

					log_info(kernel_principal, "PID: <%d> - Wait: <%s> - Instancias: <%d>", proceso_a_ejecutar->pcb->pid, recurso_a_pedir->nombre, (recurso_a_pedir->instancias_totales - recurso_a_pedir->instancias_usadas));

					sem_post(&sem_exec);
				} else {
					proceso_a_ejecutar->desalojado = DESALOJADO;
					proceso_a_ejecutar->final_ultima_rafaga = get_time();
					proceso_a_ejecutar->ultima_rafaga = proceso_a_ejecutar->final_ultima_rafaga - proceso_a_ejecutar->principio_ultima_rafaga;
					calcular_estimacion(proceso_a_ejecutar);


					pthread_mutex_lock(&mutex_exec);
					list_remove(cola_exec, 0);
					pthread_mutex_unlock(&mutex_exec);

					log_info(kernel_principal, "PID: <%d> - Bloqueado por <%s>", proceso_a_ejecutar->pcb->pid, recurso_a_pedir->nombre);

					queue_push(recurso_a_pedir->cola_espera, proceso_a_ejecutar);
					sem_post(&sem_ready);
				}
			} else {
				pthread_mutex_lock(&mutex_exit);
				proceso_a_ejecutar->pcb->estado = EXT;
				list_add(cola_exit, proceso_a_ejecutar);
				pthread_mutex_unlock(&mutex_exit);

				pthread_mutex_lock(&mutex_exec);
				list_remove(cola_exec, 0);
				pthread_mutex_unlock(&mutex_exec);

				log_info(kernel_principal, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <EXIT>", proceso_a_ejecutar->pcb->pid);
				log_info(kernel_principal, "Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>", proceso_a_ejecutar->pcb->pid);
				sem_post(&sem_exit); //despierta el proceso que saca a los procesos del sistema
				sem_post(&sem_ready);
			}

			break;
		case SIGNAL:
			//Libera una instancia del recurso indicado por parámetro.

			proceso_a_ejecutar->ultima_instruccion = SIGNAL;
			proceso_a_ejecutar->desalojado = NO_DESALOJADO;
			
			int tamanio_parametro_signal = 0;

			memcpy(&tamanio_parametro_signal, stream, sizeof(int));
			stream += sizeof(int);

			char *recurso_signal = malloc(tamanio_parametro_signal);
			memset(recurso_signal, 0, tamanio_parametro_signal);

			memcpy(recurso_signal, stream, tamanio_parametro_signal);
			stream += tamanio_parametro_signal;

			log_warning(logger_kernel, "PID: <%d> solicita liberar una instancia del Recurso <%s>", proceso_a_ejecutar->pcb->pid, recurso_signal);

			if(dictionary_has_key(recursos, recurso_signal)) {
				//Lógica para SIGNAL TODO CREO QUE ES POR ACA

				t_recurso* recurso_a_liberar = dictionary_get(recursos, recurso_signal);

				liberar_instancia_recurso(recurso_a_liberar);
				//quitar_recurso(proceso_a_ejecutar, recurso_a_liberar);

				log_info(kernel_principal, "PID: <%d> - Signal: <%s> - Instancias: <%d>", proceso_a_ejecutar->pcb->pid, recurso_a_liberar->nombre, (recurso_a_liberar->instancias_totales - recurso_a_liberar->instancias_usadas));

//				pthread_mutex_lock(&mutex_exec);
//				list_add(cola_exec, proceso_a_ejecutar);
//				pthread_mutex_unlock(&mutex_exec);

				sem_post(&sem_exec);
			} else {
				pthread_mutex_lock(&mutex_exit);
				proceso_a_ejecutar->pcb->estado = EXT;
				list_add(cola_exit, proceso_a_ejecutar);
				pthread_mutex_unlock(&mutex_exit);

				pthread_mutex_lock(&mutex_exec);
				list_remove(cola_exec, 0);
				pthread_mutex_unlock(&mutex_exec);

				log_info(kernel_principal, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <EXIT>", proceso_a_ejecutar->pcb->pid);
				log_info(kernel_principal, "Finaliza el proceso <%d> - Motivo: <INVALID_RESOURCE>", proceso_a_ejecutar->pcb->pid);
				sem_post(&sem_exit); //despierta el proceso que saca a los procesos del sistema
			}

			break;
		case YIELD: //TERMINADO
			proceso_a_ejecutar->ultima_instruccion = YIELD;
			proceso_a_ejecutar->desalojado = DESALOJADO;

			int fin_cpu = get_time();
			proceso_a_ejecutar->final_ultima_rafaga = fin_cpu;
			proceso_a_ejecutar->ultima_rafaga = proceso_a_ejecutar->final_ultima_rafaga - proceso_a_ejecutar->principio_ultima_rafaga;
			calcular_estimacion(proceso_a_ejecutar);
			
			pthread_mutex_lock(&mutex_exec);
			list_remove(cola_exec, 0);
			pthread_mutex_unlock(&mutex_exec);

			log_warning(logger_kernel, "La última instrucción ejecutada fue YIELD");
			pthread_mutex_lock(&mutex_ready);
			proceso_a_ejecutar->pcb->estado = READY;
			list_add(cola_ready, proceso_a_ejecutar);
			proceso_a_ejecutar->llegada_ready = get_time();
			pthread_mutex_unlock(&mutex_ready);

			sem_post(&sem_ready);
			break;
		case EXIT: //TERMINADO
			pthread_mutex_lock(&mutex_exit);
			proceso_a_ejecutar->pcb->estado = EXT;
			list_add(cola_exit, proceso_a_ejecutar);
			pthread_mutex_unlock(&mutex_exit);

			pthread_mutex_lock(&mutex_exec);
			list_remove(cola_exec, 0);
			pthread_mutex_unlock(&mutex_exec);

			log_info(kernel_principal, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <EXIT>", proceso_a_ejecutar->pcb->pid);
			log_info(kernel_principal, "Finaliza el proceso <%d> - Motivo: <SUCCESS>", proceso_a_ejecutar->pcb->pid);
			sem_post(&sem_exit); //despierta el proceso que saca a los procesos del sistema
			sem_post(&sem_ready);
			break;
		case SEG_FAULT:
			pthread_mutex_lock(&mutex_exit);
			proceso_a_ejecutar->pcb->estado = EXT;
			list_add(cola_exit, proceso_a_ejecutar);
			pthread_mutex_unlock(&mutex_exit);

			pthread_mutex_lock(&mutex_exec);
			list_remove(cola_exec, 0);
			pthread_mutex_unlock(&mutex_exec);

			log_info(kernel_principal, "PID: <%d> - Estado Anterior: <EXEC> - Estado Actual: <EXIT>", proceso_a_ejecutar->pcb->pid);
			log_info(kernel_principal, "Finaliza el proceso <%d> - Motivo: <SEG_FAULT>", proceso_a_ejecutar->pcb->pid);
			sem_post(&sem_exit); //despierta el proceso que saca a los procesos del sistema
			sem_post(&sem_ready);
			break;
		default:
			log_error(logger_kernel, "[ERROR] Feneció el CPU, chau loco suerte arreglate.");
			break;
		}

//		free(stream);
		eliminar_paquete(paquete);
	}

}

//----------------------------------ESTADO EXIT---------------------------------------------------------------
// Este estado se encarga de avisar a la consola que se cierre y de eliminar las estructuras del PCB para liberar memoria

void estado_exit(void) {

	while(1) {
		sem_wait(&sem_exit);
		t_proceso* proceso;

		pthread_mutex_lock(&mutex_exit);
		proceso = list_remove(cola_exit, 0);
		pthread_mutex_unlock(&mutex_exit);

		liberar_recursos_asignados(proceso->pcb->recursos_asignados);

		//Liberar las estructuras del proceso en memoria

		pthread_mutex_lock(&mutex_operacion_memoria);
		enviar_pid_memoria(proceso->pcb->pid, LIBERAR_ESTRUCTURAS);
		log_info(logger_kernel, "PID: <%d> liberando estructuras en memoria", proceso->pcb->pid);
		op_code respuesta_memoria;

		recv(socket_memoria, &respuesta_memoria, sizeof(op_code), MSG_WAITALL);
		pthread_mutex_unlock(&mutex_operacion_memoria);

		if(respuesta_memoria != ESTRUCTURAS_LIBERADAS) {
			log_error(logger_kernel, "No se pudo eliminar de memoria a PCB id[%d]", proceso->pcb->pid);
		}

		pthread_mutex_lock(&mutex_procesos_en_el_sistema);
		dictionary_remove(procesos_en_el_sistema, proceso->pcb->pid_string);
		pthread_mutex_unlock(&mutex_procesos_en_el_sistema);

		avisar_a_modulo(proceso->socket, FINALIZAR_CONSOLA);

		log_info(logger_kernel, "Ordenando a Consola la finalización del PID: <%d>", proceso->pcb->pid);

		eliminar_pcb(proceso->pcb);
		free(proceso);
		sem_post(&sem_grado_multiprogramacion);
	}
}

void eliminar_pcb(t_pcb* pcb) {
	//list_destroy_and_destroy_elements(pcb->tabla_segmentos->segmentos, free);
	//list_destroy_and_destroy_elements(pcb->recursos_asignados, free);
	//TODO seguir destruyendo las eventuales listas de la pcb.
}

//---------------------------------------------------ESTADO READY----------------------------------------------------
//Este estado lo que hace es obtener el proceso necesario en base al algoritmo de la config para agregarlo a la lista EXEC

void estado_ready(void) {

	while(1) {
		sem_wait(&sem_ready);

		pthread_mutex_lock(&mutex_ready);
		if(list_is_empty(cola_ready)) { //Si la lista de ready está vacía vuelve y vuelve a entrar al while hasta que haya procesos.
			pthread_mutex_unlock(&mutex_ready);
			continue;
		}
		pthread_mutex_unlock(&mutex_ready);

		pthread_mutex_lock(&mutex_exec);
		if(list_size(cola_exec) == 0) {       //Esto es para que no haya más de un proceso en cola_exec
			pthread_mutex_unlock(&mutex_exec);

			t_proceso* siguiente_proceso = obtener_proceso_cola_ready();

			pthread_mutex_lock(&mutex_exec);
			list_add(cola_exec, siguiente_proceso);
			pthread_mutex_unlock(&mutex_exec);

			//log_info(logger_kernel, "PID: <%d> ingresa a EXECUTE", siguiente_proceso->pcb->pid);
			log_info(kernel_principal, "PID: <%d> - Estado Anterior: <READY> - Estado Actual: <EXEC>", siguiente_proceso->pcb->pid);
			sem_post(&sem_exec);
		} else {
			pthread_mutex_unlock(&mutex_exec);
		}
	}

}

t_proceso* obtener_proceso_cola_ready(void) {

	t_proceso* proceso = malloc(sizeof(t_proceso));

	switch(config_kernel.algoritmo_planificacion){
	case FIFO:
		proceso = list_remove(cola_ready, 0);
		log_info(logger_kernel, "PID: <%d> saliendo de Cola de Ready por FIFO para ejecutar.", proceso->pcb->pid);
		break;
	case HRRN:
		aplicar_hrrn();
		proceso = list_remove(cola_ready, 0);
		log_info(logger_kernel, "PID: <%d> saliendo de Cola de Ready por HRRN para ejecutar.", proceso->pcb->pid);
		break;
	default:
		log_error(logger_kernel, "Error al elegir un proceso de la cola de Ready para ejecutar");
		break;
	}

	return proceso;
}

void aplicar_hrrn(void) {

	list_iterate(cola_ready, (void*) calcular_response_ratio);
	list_iterate(cola_ready, (void*) mostrar_response_ratio);

	list_sort(cola_ready, (void*) comparador_response_ratio);
}

void calcular_response_ratio(t_proceso* proceso) {
	//ATENCIÓN CON LO QUE VIENE ES UNA MAGIC JOHNSON, DESPUÉS INDAGAR BIEN TODO SOBRE FUNCIONES DE CLOCK
//	struct timespec end;
//	clock_gettime(CLOCK_REALTIME, &end);
//
//	long seconds = end.tv_sec - proceso->pcb->llegada_ready.tv_sec;
//	long nanoseconds = end.tv_nsec - proceso->pcb->llegada_ready.tv_sec;
//	double elapsed = seconds + nanoseconds * 1e-9;

	int fin_espera = get_time();
	int elapsed = fin_espera - proceso->llegada_ready;

	proceso->tiempo_espera = elapsed; //TODO poner tiempo de llegada a ready en el hilo correspondiente estado_ready y estado_block
	proceso->response_ratio = (double) 1 + ((double) proceso->tiempo_espera / (double) proceso->rafaga_estimada);
}

bool comparador_response_ratio(t_proceso* un_proceso, t_proceso* otro_proceso) {
	return un_proceso->response_ratio >= otro_proceso->response_ratio;
}

void mostrar_response_ratio(t_proceso* un_proceso) {
	log_info(logger_kernel, "PID: <%d> tiene un RR de: %f", un_proceso->pcb->pid, un_proceso->response_ratio);
}

void calcular_estimacion(t_proceso* un_proceso) {
	double nueva_rafaga = (config_kernel.alfa_hrrn * un_proceso->rafaga_estimada) + ((un_proceso->ultima_rafaga) * (1 - config_kernel.alfa_hrrn));
	un_proceso->rafaga_estimada = nueva_rafaga;
}

//-----------------------------------------ESTADO BLOCK POR IO--------------------------------------------------------------------------------------
//Este estado es el encargado de gestionar la cola de bloqueados por IO, haciendo que ejecuten la interrupción y se bloqueen por el tiempo indicado.

void estado_block_io(void) {

	while (1) {
		sem_wait(&sem_block_io);

		pthread_mutex_lock(&mutex_block_io);
		t_proceso *proceso = list_remove(cola_block_io, 0);
		pthread_mutex_unlock(&mutex_block_io);

		log_info(kernel_principal, "PID: <%d> - Ejecuta IO: <%d>", proceso->pcb->pid, proceso->tiempo_bloqueo);
		sleep(proceso->tiempo_bloqueo);

		pthread_mutex_lock(&mutex_ready);
		list_add(cola_ready, proceso);

		switch (config_kernel.algoritmo_planificacion) {
		case FIFO:
			log_info(kernel_principal, "Cola Ready <FIFO>:");
			mostrar_cola(cola_ready);
			break;
		case HRRN:
			log_info(kernel_principal, "Cola Ready <HRRN>:");
			mostrar_cola(cola_ready);
			break;
		default:
			break;
		}
			pthread_mutex_unlock(&mutex_ready);

			proceso->llegada_ready = get_time();

			sem_post(&sem_ready);

	}
}
