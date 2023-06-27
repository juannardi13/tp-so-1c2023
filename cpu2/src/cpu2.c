#include "utils_cpu2.h"

#define MAX_LEN 256

int main() {
	bool cpu_bloqueada = false;
	char* ip_kernel;
	char* puerto_kernel;
    t_log* logger = iniciar_logger();
    t_config* config = iniciar_config();

    log_info(logger, "Hola! Se inicializo el modulo cliente CPU.");

    ip_kernel = config_get_string_value(config, "IP");
    puerto_kernel = config_get_string_value(config, "PUERTO_KERNEL");

    log_info(logger, "La configuracion de la conexion indica el PUERTO %s y la IP %s", ip_kernel, puerto_kernel);

    int fd_cpu = iniciar_servidor(puerto_kernel);
    log_info(logger, "CPU inicializado, esperando a recibir al Kernel en el PUERTO %s.", puerto_kernel);
    int fd_kernel = esperar_cliente(logger, "CPU", fd_cpu);


    int ip_memoria = config_get_string_value(config, "IP");
    int puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    int fd_cpu_memoria = iniciar_servidor(puerto_memoria);
    int fd_memoria = esperar_cliente(logger, "CPU", fd_memoria);

    while(1) {
    	t_paquete* paquete = malloc(sizeof(t_paquete));
    	paquete->buffer = malloc(sizeof(t_buffer));
    	recv(fd_kernel, &(paquete->codigo_operacion), sizeof(op_code), MSG_WAITALL);
    	recv(fd_kernel, &(paquete->buffer->stream_size), sizeof(int), 0);
    	paquete->buffer->stream = malloc(paquete->buffer->stream_size);
    	recv(fd_kernel, paquete->buffer->stream, paquete->buffer->stream_size, 0);

    	t_contexto_de_ejecucion* contexto;

    	switch(paquete->codigo_operacion){
    	case CONTEXTO_DE_EJECUCION :
    		void* stream = paquete->buffer->stream;
    				memcpy(&(contexto->pid), stream, sizeof(int));
    				stream += sizeof(int);
    				memcpy(&(contexto->tamanio_instrucciones), stream, sizeof(int));
    				stream += sizeof(int);
    				memcpy(&(contexto->instrucciones), stream, contexto->tamanio_instrucciones);
    				stream += contexto->tamanio_instrucciones;
    				memcpy(&(contexto->pc), stream, sizeof(int));
    				stream += sizeof(int);

    				t_registros* registro_actual = contexto->registros_pcb;
    				for(int i=0; i<(contexto->tamanio_registros); i++){
    					memcpy(&(registro_actual), stream, sizeof(t_registros));
    					stream += sizeof(t_registros);
    					registro_actual++;
    				}

    				for(int a=0; a<(list_size(contexto->tabla_segmentos)); a++){
    					t_segmento* segmento_actual = list_get(contexto->tabla_segmentos, a);
    					deserializar_segmentos(segmento_actual, stream);
    						}
    				int i = 0;
    				    		char** intrucciones_parseadas = string_split(contexto->instrucciones, "\n");
    				    		int cantidad_intrucciones = sizeof(intrucciones_parseadas);
    				    		while(i < cantidad_intrucciones){
    				    			char* instruccion_a_ejecutar;
    				    			strncpy(instruccion_a_ejecutar, fetch_instruccion(contexto), strlen(fetch_instruccion(contexto))+1);
    				    			decode_instruccion(instruccion_a_ejecutar, contexto, config, fd_memoria, fd_kernel, cpu_bloqueada);
    				    			i++;
    				    		}

    				    	    break;
    			default:
    	    		log_warning(logger,"Operacion desconocida. No quieras meter la pata");
    	    		break;
    					}

    	}

    return 0;
    }





