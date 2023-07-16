#include "fileSystem.h"

int main(void) {

	inicializar_estructuras();

	probando_cositas();

	inicializar_servidor();

	liberar_recursos_bitmap(archivo_bloques,archivo_bloques,mapping_archivo_bloques);

	return 0;
}


