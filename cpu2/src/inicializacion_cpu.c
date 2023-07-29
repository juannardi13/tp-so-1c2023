#include"utils_cpu2.h"
#include <math.h>
#include<shared-2.h>

// FUNCIONES QUE SON PARA INICIALIZAR EL CPU

t_registros registros_cpu; //Variable global para los registros del CPU, después es lo que se le asigna al proceso cuando se va de CPU. TODO
t_list* tabla_segmentos_prueba;

t_log* iniciar_logger(void) {
	t_log* nuevo_logger = log_create("cpu.log", "cpu.log", 1, LOG_LEVEL_INFO);

	if (nuevo_logger == NULL) {
		printf("No se pudo crear el log\n");
		exit(1);
	}

	return nuevo_logger;
}

t_log* iniciar_logger_principal(void) {
	t_log* nuevo_logger = log_create("cpu_principal.log", "cpu_principal.log", 1, LOG_LEVEL_INFO);

	if (nuevo_logger == NULL) {
		printf("No se pudo crear el log\n");
		exit(1);
	}

	return nuevo_logger;
}

t_config* iniciar_config(char *ruta) {
	t_config* nuevo_config = config_create(ruta);

	if (nuevo_config == NULL) {
		printf("No se pudo leer la config\n");
		exit(2);
	}

	return nuevo_config;
}

void inicializar_registros(void) {
	strcpy(registros_cpu.ax, "0000");
	strcpy(registros_cpu.bx, "0000");
	strcpy(registros_cpu.cx, "0000");
	strcpy(registros_cpu.dx, "0000");
	strcpy(registros_cpu.eax, "00000000");
	strcpy(registros_cpu.ebx, "00000000");
	strcpy(registros_cpu.ecx, "00000000");
	strcpy(registros_cpu.edx, "00000000");
	strcpy(registros_cpu.rax, "0000000000000000");
	strcpy(registros_cpu.rbx, "0000000000000000");
	strcpy(registros_cpu.rcx, "0000000000000000");
	strcpy(registros_cpu.rdx, "0000000000000000");
}

void inicializar_segmentos_prueba(void) {
	tabla_segmentos_prueba = list_create();

	t_segmento* segmento0 = malloc(sizeof(t_segmento));

	segmento0->id = 0;
	segmento0->base = 0;
	segmento0->tamanio = 128;

	list_add(tabla_segmentos_prueba, segmento0);

	t_segmento* segmento1 = malloc(sizeof(t_segmento));

	segmento1->id = 1;
	segmento1->base = 129;
	segmento1->tamanio = 128;

	list_add(tabla_segmentos_prueba, segmento1);

	t_segmento* segmento2 = malloc(sizeof(t_segmento));

	segmento2->id = 2;
	segmento2->base = 300;
	segmento2->tamanio = 128;

	list_add(tabla_segmentos_prueba, segmento2);
}
