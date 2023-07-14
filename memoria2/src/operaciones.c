#include "operaciones.h"

void atender_File_System(int *fileSystem_fd) {

	int socket_cliente = *fileSystem_fd;

	while (1) {
		int cod_op = recibir_operacion(socket_cliente);

		switch(cod_op) {
		case -1:
			log_warning(logger, "Se desconectó el File System!");
			return;
		}
	}
}

// AGREGO INICIO DE ATENDER A CPU
void atender_CPU(int *cpu_fd){

	int socket_cliente = *cpu_fd;

	while(1){
		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));
		recv(socket_cliente, &(paquete->buffer->stream_size), sizeof(int), 0);
		paquete->buffer->stream = malloc(paquete->buffer->stream_size);
		recv(socket_cliente, paquete->buffer->stream, paquete->buffer->stream_size, 0);

		int* direccion_fisica_buscada = malloc(sizeof(int));
		char* valor_a_asignar = malloc(sizeof(t_registros));

		switch(paquete->codigo_operacion){
		case LEER_DE_MEMORIA :
			// RECIBO LA DIRECCION FISICA BUSCADA POR CPU, BUSCO SU VALOR ASOCIADO Y LO ENVIO A CPU
			void *stream1 = paquete->buffer->stream;
			memcpy(&(direccion_fisica_buscada), stream1, sizeof(int));
			stream1 += sizeof(int);
			buscar_valor_enviar_CPU(direccion_fisica_buscada, socket_cliente);


		case ESCRIBIR_EN_MEMORIA :
			// RECIBO LA DIRECCION FISICA Y EL VALOR A ESCRIBIR EN ELLA
			void *stream2 = paquete->buffer->stream;
			memcpy(&(direccion_fisica_buscada), stream2, sizeof(int));
			stream2 += sizeof(int);
			memcpy(&(valor_a_asignar), stream2, sizeof(t_registros));
			stream2 += sizeof(t_registros);
			escribir_valor_en_direccion_fisica(direccion_fisica_buscada, valor_a_asignar);
		}
	}
}

// FUNCIONES SIN DESARROLLAR TODAVIA

void buscar_valor_enviar_CPU(int direccion_fisica_buscada, int socket_cliente){
	// Tenemos que ver como buscar una direccion fisica
	// Si encuentra la direccion fisica, debe devolverle a cpu el valor encontrado como un paquete
	if(1==1){ // if encuentra direccion fisica
		char* valor_buscado; // le deberia asignar el valor que encontre
		t_paquete* paquete_valor = crear_paquete(LEIDO);
		agregar_a_paquete(paquete_valor, &valor_buscado, strlen(valor_buscado)+1);
		enviar_paquete(paquete_valor, socket_cliente);
		eliminar_paquete(paquete_valor);
	}
	else{
		t_paquete* paquete_valor = crear_paquete(NO_LEIDO);
		enviar_paquete(paquete_valor, socket_cliente);
		eliminar_paquete(paquete_valor);
	}
}

void escribir_valor_en_direccion_fisica(int direccion_fisica, char* valor){
	// Buscar direccion fisica, le asigna valor que me paso CPU
	// Ver que pasa si no la encuentra !!!
}

