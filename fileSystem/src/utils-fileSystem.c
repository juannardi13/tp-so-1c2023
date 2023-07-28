#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <commons/string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

int acceso_lectura_bitmap(int nro_bloque)
{
	char* string;

	int ret = bitarray_test_bit(estructura_bitmap,nro_bloque);

	if(ret)
	{
		string = "Ocupado";
	}
	else
	{
		string = "Libre";
	}

	log_info(logger,"Acceso de Lectura a Bitmap - Bloque <%i> - Estado: <%s>",nro_bloque,string);
	return ret;
}

int abrir_archivo(char* nombre_archivo){

	FILE* f;

	int boolExiste = 1;

	char* ruta = obtener_ruta_archivo(nombre_archivo);

	f = fopen(ruta,"r");

	if(f == NULL)
	{
		boolExiste = 0;
	}

	free(ruta);

	return boolExiste;
}


int crear_archivo(char* nombre_archivo)
{
	FILE* f;

	char* ruta = obtener_ruta_archivo(nombre_archivo);

	f = fopen(ruta,"w+");

	int fd = fileno(f);

	if(fd == -1){
		printf("Error al crear archivo de config\n");
		fclose(f);
	}

	fprintf(f,"NOMBRE_ARCHIVO=%s\n",nombre_archivo);
	fprintf(f,"TAMANIO_ARCHIVO=%s\n","0");
	fprintf(f,"PUNTERO_DIRECTO=%s\n","-1");
	fprintf(f,"PUNTERO_INDIRECTO=%s\n","-1");

	fclose(f);

	log_info(logger,"Crear Archivo: <%s>",nombre_archivo);

	free(ruta);

	return 1;
}

void msleep(int tiempo_microsegundos) {
	usleep(tiempo_microsegundos * 1000);
}

void acceso_a_bloque(void* destino, void* origen,int cantidad_bytes,char* nombre_archivo,int nro_bloque,int nro_bloque_inicial_en_archivo_bloques)
{
	msleep(config_valores.retardo_acceso_bloque);

	memcpy(destino,origen,cantidad_bytes);

	log_info(logger,"Acceso Bloque - Archivo: <%s> - Bloque Archivo: <%i> - Bloque FileSystem: <%i>",nombre_archivo,nro_bloque,nro_bloque_inicial_en_archivo_bloques);
}

void truncar_archivo(char* nombre_archivo,int nuevo_tamanio_entero)
{

	FILE* f;

	char* ruta = obtener_ruta_archivo(nombre_archivo);

	f = fopen(ruta,"r+");

	if (fileno(f) == -1){
		printf("Error relacionado al archivo %s \n",nombre_archivo);
	}

	t_config* config_fcb_archivo = iniciar_config(ruta);

	int tamanio_fcb = config_get_int_value(config_fcb_archivo,"TAMANIO_ARCHIVO");

	if(nuevo_tamanio_entero > tamanio_fcb)
	{
			ampliar_tamanio(tamanio_fcb,nuevo_tamanio_entero,config_fcb_archivo,nombre_archivo);
	}
	else
	{
			reducir_tamanio(tamanio_fcb,nuevo_tamanio_entero,config_fcb_archivo,nombre_archivo);
	}

	log_info(logger,"Truncar Archivo: <%s> - Tamaño: <%i>",nombre_archivo,nuevo_tamanio_entero);

	char* nuevo_tamanio = string_itoa(nuevo_tamanio_entero);

	config_set_value(config_fcb_archivo,"TAMANIO_ARCHIVO",nuevo_tamanio);

	config_save(config_fcb_archivo);

	config_destroy(config_fcb_archivo);

	free(nuevo_tamanio);

	free(ruta);

	fclose(f);

}

void reducir_tamanio(int tamanio_fcb,int nuevo_tamanio_entero,t_config* config_fcb_archivo,char* nombre_archivo)
{

	int tamanio_bloque = config_super_bloque_valores.block_size;


	if(tamanio_fcb <= tamanio_bloque)
	{

	}
	else
	{

		int nro_bloque_punteros_indirectos = config_get_int_value(config_fcb_archivo,"PUNTERO_INDIRECTO");

		int nro_ultima_entrada_bloque_pind = ceil((double)(tamanio_fcb - tamanio_bloque) / (double)tamanio_bloque) -1;

		if(nuevo_tamanio_entero <= tamanio_bloque)
		{
			reducir_tamanio_cuando_tam_actual_mayor_tam_bloque_y_nuevo_tam_menor_tam_bloque(nro_ultima_entrada_bloque_pind,nro_bloque_punteros_indirectos,nombre_archivo);
		}
		else
		{
			int cantidad_bloques_apuntados_necesarios = ceil((double)nuevo_tamanio_entero / (double)tamanio_bloque) - 1;

			reducir_tamanio_cuando_tam_actual_mayor_tam_bloque_y_nuevo_tam_mayor_tam_bloque(nro_ultima_entrada_bloque_pind,nro_bloque_punteros_indirectos,cantidad_bloques_apuntados_necesarios,nombre_archivo);
		}

	}
}


void ampliar_tamanio(int tamanio_fcb,int nuevo_tamanio_entero,t_config* config_fcb_archivo,char* nombre_archivo)
{
	int tamanio_bloque = config_super_bloque_valores.block_size;

	if(tamanio_fcb == 0)
	{
		if(nuevo_tamanio_entero <= tamanio_bloque)
		{
			ampliar_con_tam_actual_cero_y_tam_nuevo_menor_igual_tam_bloque(config_fcb_archivo);
		}
		else
		{
			ampliar_con_tam_actual_cero_y_tam_nuevo_mayor_tam_bloque(nuevo_tamanio_entero,config_fcb_archivo,nombre_archivo);
		}
	}
	else
	{
		if(tamanio_fcb <= tamanio_bloque)
		{
			if(nuevo_tamanio_entero <= tamanio_bloque)
			{

			}
			else
			{
				ampliar_con_tam_actual_menor_tam_bloque_tam_nuevo_mayor_tam_bloque(nuevo_tamanio_entero,config_fcb_archivo,nombre_archivo);
			}
		}
		else
		{
			ampliar_con_tam_actual_mayor_tam_bloque(tamanio_fcb,nuevo_tamanio_entero,config_fcb_archivo,nombre_archivo);
		}
	}
}


// SET RAX SonicTheHedgehog guarda en registro rax SonicTheHedgehog
// MOV_OUT 0 RAX escribo en la dirección de memoria física  obtenida a partir de la dirección lógica SonicTheHedgehog
// luego F_WRITE 0 12 bytes. SE busca en dir log 0 lo que hay (sonic hedgehog) y se escribe segun el puntero indicado por kernel

void escribir_archivo(char* nombre_archivo,int nro_byte_archivo,void* direccion_fisica,int cantidad_bytes_escribir)
{
	if(!abrir_archivo(nombre_archivo))
	{
		log_info(logger,"El archivo <%s> no existe",nombre_archivo);
	}
	else
	{

		// lo jodido va a ser escribir cuando tenga que saltar de bloque

		char* ruta = obtener_ruta_archivo(nombre_archivo);

		t_config* config_fcb_archivo = iniciar_config(ruta);

		int tamanio_bloque = config_super_bloque_valores.block_size;

		int nro_byte_final = nro_byte_archivo + cantidad_bytes_escribir - 1;

//		printf("Número de byte final de archivo a escribir: %i\n",nro_byte_final);


		int nro_bloque_inicial = floor((double) nro_byte_archivo / (double) tamanio_bloque);

		int nro_bloque_final = floor((double) nro_byte_final / (double) tamanio_bloque);
//
//		printf("Número de bloque inicial donde se escribe: %i\n",nro_bloque_inicial);
//
//		printf("Número de bloque final donde se escribe %i\n",nro_bloque_final);

		if(nro_bloque_inicial == nro_bloque_final)
		{
//			printf("Llegué\n");
			escribir_bytes_mismo_bloque(nro_byte_archivo,nro_bloque_inicial,direccion_fisica,cantidad_bytes_escribir,config_fcb_archivo,nombre_archivo);
		}
		else
		{
			escribir_bytes_diferentes_bloques(nro_bloque_inicial,nro_byte_archivo,direccion_fisica,cantidad_bytes_escribir,config_fcb_archivo,nombre_archivo);
		}

		log_info(logger,"Escribir Archivo: <%s> - Puntero <%i> - Tamaño <%i>",nombre_archivo,nro_byte_archivo,cantidad_bytes_escribir);

		config_destroy(config_fcb_archivo);

		free(ruta);
	}


}


// un ejemplo F_WRITE Consoles 32 80
// osea le pasa nombre archivo dir lógica y cantidad bytes
// esto recibe cpu imagino y le pasa a kernel. Después kernel pasa a filesystem el nombre del archivo, el puntero actual donde se encuentra según la tabla de archivos
// abiertos por proceso, y la cantidad de bytes a leer

void* leer_archivo(char* nombre_archivo,int nro_byte_archivo,int cantidad_bytes_leer)
{
	// 10
	void* leido;

	if(!abrir_archivo(nombre_archivo))
	{
		log_info(logger,"El archivo no existe");
	}
	else
	{

		// 64
		int tamanio_bloque = config_super_bloque_valores.block_size;

		// 0
		int nro_bloque_inicial = floor((double) nro_byte_archivo / (double) tamanio_bloque);

		// 69
		int nro_byte_final = nro_byte_archivo + cantidad_bytes_leer - 1;

		// 1
		int nro_bloque_final = floor((double) nro_byte_final / (double) tamanio_bloque);

		char* ruta = obtener_ruta_archivo(nombre_archivo);

		t_config* config_fcb_archivo = iniciar_config(ruta);

		if(nro_bloque_inicial == nro_bloque_final)
		{
			leido = leer_bytes_mismo_bloque(nro_bloque_inicial,nro_byte_archivo,cantidad_bytes_leer,config_fcb_archivo,nombre_archivo);
		}
		else
		{
			leido = leer_bytes_en_bloques_distintos( nro_bloque_inicial, nro_byte_archivo, cantidad_bytes_leer, config_fcb_archivo,nombre_archivo);
		}

		printf("%s\n",leido);

		free(ruta);

		config_destroy(config_fcb_archivo);
	}

	return leido;
}


void concatenar_strings(const char* s1, const char* s2,char* buffer){
	memccpy(memccpy(buffer,s1,'\0',100)-1,s2,'\0',100);
}

t_bitarray* inicializar_archivo_bm_prueba(){

	archivo_bm = fopen("../fileSystem/grupoDeBloques/bitmap.bin","w+");

	int fd = fileno(archivo_bm);

	if(fd == -1){
		printf("Error al crear archivo bitmap\n");
		fclose(archivo_bm);
	}

	unsigned char byte = 0;

	int cantidad_bloques = config_super_bloque_valores.block_count;

	int byte_adicional = (cantidad_bloques % 8 != 0)?1:0;

	int cantidad_bytes = (cantidad_bloques / 8) + byte_adicional;

	ftruncate(fd,cantidad_bytes);

	for(int i = 0; i < cantidad_bytes; i++){
		fwrite(&byte,1,1,archivo_bm);
	}

	char* mapping = mmap(NULL,cantidad_bytes+1,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);

	if(mapping == MAP_FAILED){
		printf("Error al mappear memoria\n");
	}

	t_bitarray* estructura_aux = bitarray_create_with_mode(mapping,cantidad_bytes,LSB_FIRST);

	return estructura_aux;
}

t_bitarray* inicializar_archivo_bm(){

	archivo_bm = fopen("../fileSystem/grupoDeBloques/bitmap.bin","a+");

	int fd = fileno(archivo_bm);

	if(fd == -1){
		printf("Error al crear archivo bitmap\n");
		fclose(archivo_bm);
	}

	fseek(archivo_bm,0,SEEK_END);

	long size = ftell(archivo_bm);

	int cantidad_bloques = config_super_bloque_valores.block_count;

	int byte_adicional = (cantidad_bloques % 8 != 0)?1:0;

	int cantidad_bytes = (cantidad_bloques / 8) + byte_adicional;

	if(size == 0)
	{
		unsigned char byte = 0;

		ftruncate(fd,cantidad_bytes);

		for(int i = 0; i < cantidad_bytes; i++){
			fwrite(&byte,1,1,archivo_bm);
		}
	}


	char* mapping = mmap(NULL,cantidad_bytes+1,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);

	if(mapping == MAP_FAILED){
		printf("Error al mappear memoria\n");
	}

	t_bitarray* estructura_aux = bitarray_create_with_mode(mapping,cantidad_bytes,LSB_FIRST);

	return estructura_aux;
}

bool esta_ocupado(int nro_bloque){
	return bitarray_test_bit(estructura_bitmap,nro_bloque);
}

int primer_bloque_disponible()
{
	int i = 0;

	int limite = estructura_bitmap->size * 8;

	while(i < limite && esta_ocupado(i)){
		i++;
	}

	return i;
}

void acceso_escritura_bitmap(int nro_bloque,int estado_escribir)
{
	if(estado_escribir)
	{
		bitarray_set_bit(estructura_bitmap,nro_bloque);
		log_info(logger,"Acceso de Escritua a Bitmap - Bloque <%i> - Se cambia estado a: <1>",nro_bloque);
	}
	else
	{
		bitarray_clean_bit(estructura_bitmap,nro_bloque);
		log_info(logger,"Acceso de Escritua a Bitmap - Bloque <%i> - Se cambia estado a: <0>",nro_bloque);
	}
}

// en desuso
void asignar_bloques_iniciales(char* ruta,t_config* config_fcb_archivo){

	int nro_primer_bloque = primer_bloque_disponible();

	acceso_escritura_bitmap(nro_primer_bloque,1);

	int nro_bloque_punteros = primer_bloque_disponible();

	acceso_escritura_bitmap(nro_bloque_punteros,1);

	char* s1 = string_itoa(nro_primer_bloque);
	char* s2 = string_itoa(nro_bloque_punteros);

	config_set_value(config_fcb_archivo,"PUNTERO_DIRECTO",s1);
	config_set_value(config_fcb_archivo,"PUNTERO_INDIRECTO",s2);

	free(s1);
	free(s2);

	config_save(config_fcb_archivo);

//	printf("Prueba cantidad keys config: %i\n",config_keys_amount(config_fcb_archivo));
}

void liberar_recursos_bitmap(){

	int cantidad_bloques = config_super_bloque_valores.block_count;
	int tamanio_bloques = config_super_bloque_valores.block_size;

	int cantidad_bytes = cantidad_bloques / 8;
	munmap(estructura_bitmap->bitarray,cantidad_bytes+1);
	munmap(mapping_archivo_bloques,tamanio_bloques*cantidad_bloques);
	fclose(archivo_bm);
	fclose(archivo_bloques);
	bitarray_destroy(estructura_bitmap);
	log_destroy(logger);
	config_destroy(config_super_bloque);
	config_destroy(config);
}

void levantar_archivo_bloques_prueba(){

	int cantidad_bloques = config_super_bloque_valores.block_count;
	int tamanio_bloques = config_super_bloque_valores.block_size;
	// - //

	archivo_bloques = fopen(config_valores.path_bloques,"w+");

	int fd = fileno(archivo_bloques);

	if(fd == -1){
		printf("Error al crear archivo de bloques\n");
		fclose(archivo_bloques);
	}
	int tamanio_en_bytes = cantidad_bloques * tamanio_bloques;

	ftruncate(fd,tamanio_en_bytes);

	mapping_archivo_bloques = mmap(NULL,tamanio_en_bytes,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);

	if(mapping_archivo_bloques == MAP_FAILED){
		printf("Error al mappear memoria\n");
	}

}

void levantar_archivo_bloques(){

	int cantidad_bloques = config_super_bloque_valores.block_count;
	int tamanio_bloques = config_super_bloque_valores.block_size;
	// - //

	archivo_bloques = fopen(config_valores.path_bloques,"a+");

	int fd = fileno(archivo_bloques);

	fseek(archivo_bloques,0,SEEK_END);

	long size = ftell(archivo_bloques);

	if(fd == -1){
		printf("Error al crear archivo de bloques\n");
		fclose(archivo_bloques);
	}

	int tamanio_en_bytes = cantidad_bloques * tamanio_bloques;

	if(size == 0)
	{
		ftruncate(fd,tamanio_en_bytes);

	}

	mapping_archivo_bloques = mmap(NULL,tamanio_en_bytes,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);

	if(mapping_archivo_bloques == MAP_FAILED){
		printf("Error al mappear memoria\n");
	}

}

int obtener_posicion_archivo_bloques(int indice)
{
	int tamanio_bloques = config_super_bloque_valores.block_size;
	return indice  * tamanio_bloques;
}

char* obtener_ruta_archivo(const char* nombre_archivo)
{
	const char* ruta_incompleta = "../fileSystem/grupoDeBloques/";

	char* ruta = malloc(25*sizeof(int));
	concatenar_strings(ruta_incompleta,nombre_archivo,ruta);

	return ruta;
}

int agregar_a_stream(void *stream, int* offset, void *src, int size) {
	memcpy(stream + *offset, src, size);
	*offset += size;
}
