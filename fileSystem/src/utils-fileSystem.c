#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


void probando_cositas(){
	// Prueba respuesta a petición de creación de archivo (SEGUIR) //
	// --- INICIO --- //
	printf("%i\n",(int)estructura_bitmap->size);

	levantar_fcb_nuevo_archivo("fcbPrueba");
	t_config* configTCB = iniciar_config("../fileSystem/grupoDeBloques/fcbPrueba");
	t_fcb fcb;



	fcb.puntero_directo = config_get_int_value(configTCB,"PUNTERO_DIRECTO");
	fcb.nombre_archivo = config_get_string_value(configTCB,"NOMBRE_ARCHIVO");

	printf("Puntero directo: %d \n",fcb.puntero_directo);
	printf("Nombre archivo:  %s \n",fcb.nombre_archivo);

	if(abrir_archivo("fcbPrueba"))
	{
		printf("Existe el archivo solicitado fcbPrueba \n");
	}
	else
	{
		printf("No existe el archivo solicitado fcbPrueba \n");
	}

	crear_archivo("campeonesF1");

	if(abrir_archivo("campeonesF1"))
	{
		printf("Existe el archivo solicitado campeonesF1 \n");
	}
	else
	{
		printf("No existe el archivo solicitado campeonesF1 \n");
	}


//	truncar_archivo("campeonesF1","125",mapping_archivo_bloques);

	int a = mapping_archivo_bloques;

	printf("%i \n",a);

	obtener_ruta_archivo("ArchivoEjemplo");

	// --- FIN --- //


	// Verificación bloques asignados a archivo creado se indican como ocupados //
	// --- INICIO --- //
	FILE* f;
	f = fopen("../fileSystem/grupoDeBloques/bitmap.bin","r+");


	if(fileno(f) == -1){
		printf("Error al abrir archivo bitmap\n");
	}

		fseek(f,0,0);

		unsigned char b;

		for(int i = 0; i < config_super_bloque_valores->block_count/8; i++){
			fread(&b,1,1,f);
			printf("%u\n",b);
		}

		fclose(f);
	// --- FIN --- //
}



// |								   |
// |   	FUNCIONES MÁS "ALTO NIVEL"     |
// |								   |



// |								   |
// |   	FUNCIONES DE MANEJO DE FS	   |
// |								   |

int abrir_archivo(char* nombre_archivo){
	// busco archivo, si no existe f será nula y fd será -1, devolver fd
	FILE* f;

	int boolExiste = 1;

	char* ruta = obtener_ruta_archivo(nombre_archivo);

	f = fopen(ruta,"r");

	if(f == NULL)
	{
		boolExiste = 0;
	}
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
	free(ruta);

	return 1;
}

void truncar_archivo(char* nombre_archivo,char* nuevo_tamanio)
{
	int nuevo_tamanio_entero = atoi(nuevo_tamanio);

	FILE* f;

	char* ruta = obtener_ruta_archivo(nombre_archivo);

	f = fopen(ruta,"r+");

	if (fileno(f) == -1){
		printf("Error relacionado al archivo %s \n",nombre_archivo);
	}

	t_config* config_fcb_archivo = iniciar_config(ruta);

	int tamanio_fcb = config_get_int_value(config_fcb_archivo,"TAMANIO_ARCHIVO");

	int tamanio_bloque = config_super_bloque_valores->block_size;

	int cantidad_bloques = config_super_bloque_valores->block_count;

	int cantidad_punteros_bloque = config_super_bloque_valores->block_count / sizeof(uint32_t);

	if(nuevo_tamanio_entero > tamanio_fcb)
	{
			ampliar_tamanio(tamanio_fcb,nuevo_tamanio_entero,tamanio_bloque,cantidad_punteros_bloque,cantidad_bloques,config_fcb_archivo);
	}
	else
	{

	}

	msync(mapping_archivo_bloques,tamanio_bloque*cantidad_bloques,MS_SYNC);

	config_set_value(config_fcb_archivo,"TAMANIO_ARCHIVO",nuevo_tamanio);

	config_save(config_fcb_archivo);

	fclose(f);

}

void ampliar_tamanio(int tamanio_fcb,int nuevo_tamanio_entero,int tamanio_bloque,int cantidad_punteros_bloque,int cantidad_bloques,t_config* config_fcb_archivo)
{
	if(tamanio_fcb == 0)
	{
		if(nuevo_tamanio_entero <= tamanio_bloque)
		{
			int puntero_directo = primer_bloque_disponible(estructura_bitmap);

			bitarray_set_bit(estructura_bitmap,puntero_directo);

			char* puntero_directo_string = string_itoa(puntero_directo);

			config_set_value(config_fcb_archivo,"PUNTERO_DIRECTO",puntero_directo_string);
		}
		else
		{
			int cantidad_bloques_puntero_indirecto = ceil((nuevo_tamanio_entero - tamanio_bloque) / tamanio_bloque);

			int puntero_directo = primer_bloque_disponible(estructura_bitmap);

			bitarray_set_bit(estructura_bitmap,puntero_directo);

			char* puntero_directo_string = string_itoa(puntero_directo);

			config_set_value(config_fcb_archivo,"PUNTERO_DIRECTO",puntero_directo_string);

			int puntero_indirecto = primer_bloque_disponible(estructura_bitmap);

			bitarray_set_bit(estructura_bitmap,puntero_indirecto);

			config_set_value(config_fcb_archivo,"PUNTERO_INDIRECTO",string_itoa(puntero_indirecto));

			for(int i = 0; i <  cantidad_punteros_bloque;i++)
			{

				if(i < cantidad_bloques_puntero_indirecto)
				{

					int bloque_datos = primer_bloque_disponible(estructura_bitmap);

					bitarray_set_bit(estructura_bitmap,bloque_datos);

					// escribir número de bloque asignado en el bloque del puntero indirecto (todavía no hecho)

					memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto),&bloque_datos,sizeof(uint32_t));
				}
				else
				{
					memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto),-1,sizeof(uint32_t));
				}
			}
		}
	}
	else
	{
		if(tamanio_fcb <= tamanio_bloque)
		{
			if(nuevo_tamanio_entero <= tamanio _ bloque)
			{
				// no se hace nada pues ya tiene bloque necesario, solo se reescribe tamanio en fcb
			}
			else
			{
				// resto a nuevo tam el tam actual. Con lo que me queda veo cuantos bloques extra necesito.
				// dps asigno bloque para punteros indirectos y sobre ese bloque escribo la cantidad de punteros
				// correspondiente a la cantidad de bloques extra que se necesitaron
				//				int tamanio_necesario = nuevo_tamanio_entero - tamanio_fcb;
				//
				//				int bloques_nuevos_necesarios = ceil(tamanio_necesario / tamanio_bloque);

				int cantidad_bloques_puntero_indirecto = ceil((nuevo_tamanio_entero - tamanio_bloque) / tamanio_bloque);

				int puntero_indirecto = primer_bloque_disponible(estructura_bitmap);

				bitarray_set_bit(estructura_bitmap,puntero_indirecto);

				config_set_value(config_fcb_archivo,"PUNTERO_INDIRECTO",string_itoa(puntero_indirecto));

				for(int i = 0; i <  cantidad_punteros_bloque;i++)
				{

					if(i < cantidad_bloques_puntero_indirecto)
					{

						int bloque_datos = primer_bloque_disponible(estructura_bitmap);

						bitarray_set_bit(estructura_bitmap,bloque_datos);

						// escribir número de bloque asignado en el bloque del puntero indirecto (todavía no hecho)

						memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto),&bloque_datos,sizeof(uint32_t));
					}
					else
					{
						memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto),-1,sizeof(uint32_t));
					}
				}
			}
		}
		else
		{
			// coincide con número de entrada dentro de bloque de punteros indirectos siguiente a usar.
			// por ejemplo si el tam fcb es de 16390, le resto 1024 del puntero directo
			//  me quedan 15366. Dividido 1024 es 15.005 osea se usaron 16 entradas del bloque, de la 0 a la 15
			// si el tamanio necesario me queda por ejemplo 2056 necesito 3 bloque nuevos. Serán el 16, 17, 18 osea cant_ind_usadas + 2

			int cantidad_indirecciones_usadas = ceil((tamanio_fcb - tamanio_bloque)/tamanio_bloque);

			int tamanio_necesario = nuevo_tamanio_entero - tamanio_fcb;

			int cantidad_punteros_indirectos = ceil(tamanio_necesario / tamanio_bloque);

			for(int i = cantidad_indirecciones_usadas; i < (cantidad_indirecciones_usadas + cantidad_punteros_indirectos) ;i++)
			{
				int bloque_datos = primer_bloque_disponible();

				bitarray_set_bit(estructura_bitmap,bloque_datos);

				// escribir número de bloque asignado en el bloque del puntero indirecto (todavía no hecho)

				memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(i),&bloque_datos,sizeof(uint32_t));
			}

		}
	}


}





void concatenar_strings(const char* s1, const char* s2,char* buffer){
	memccpy(memccpy(buffer,s1,'\0',100)-1,s2,'\0',100);
}


t_bitarray* inicializar_archivo_bm(FILE* f){

//	f = fopen(config_get_string_value(config,"PATH_BITMAP"),"w+");

	f = fopen("../fileSystem/grupoDeBloques/bitmap.bin","w+");

	int fd = fileno(f);

	if(fd == -1){
		printf("Error al crear archivo bitmap\n");
		fclose(f);
	}

	unsigned char byte = 0;

	int cantidad_bloques = config_super_bloque_valores->block_count;

	int byte_adicional = (cantidad_bloques % 8 != 0)?1:0;

	int cantidad_bytes = (cantidad_bloques / 8) + byte_adicional;

	ftruncate(fd,cantidad_bytes);

	for(int i = 0; i < cantidad_bytes; i++){
		fwrite(&byte,1,1,f);
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

void asignar_bloques_iniciales(FILE* fcb){
	FILE* f;
	f = fopen("../fileSystem/grupoDeBloques/bitmap.bin","r+");

	if(fileno(f) == -1){
		printf("Error al abrir archivo bitmap\n");
	}

	int nro_primer_bloque = primer_bloque_disponible(estructura_bitmap);

	bitarray_set_bit(estructura_bitmap,nro_primer_bloque);

	fprintf(fcb,"PUNTERO_DIRECTO=%i\n",nro_primer_bloque);

	int nro_bloque_punteros = primer_bloque_disponible(estructura_bitmap);

	bitarray_set_bit(estructura_bitmap,nro_bloque_punteros);

	fprintf(fcb,"PUNTERO_INDIRECTO=%i\n",nro_bloque_punteros);

	fclose(f);
	fclose(fcb);
}

void liberar_recursos_bitmap(FILE* archivo_bm,FILE* archivo_bloques,char* mapping_archivo_bloques){

	int cantidad_bloques = config_super_bloque_valores->block_count;
	int tamanio_bloques = config_super_bloque_valores->block_size;

	int cantidad_bytes = cantidad_bloques / 8;
	munmap(estructura_bitmap->bitarray,cantidad_bytes+1);
	munmap(mapping_archivo_bloques,tamanio_bloques*cantidad_bloques);
	fclose(archivo_bm);
	fclose(archivo_bloques);
	bitarray_destroy(estructura_bitmap);
}


// un archivo fcb por cada archivo (fichero)usado. Tiene puntero "a lista" de bloques, a bloque con punteros a otros bloques y puntero directo a bloque inicial
// PERO, como se recomienda un archivo de bloques y no una lista de bloques, el puntero "sera" la posicion dentro del archivo
// donde se cuenta con el contenido. Cada posición del archivo de bloques es un bloque (64 bytes). Además, para cada archivo, dentro
// de esta lista / archivo de bloques habrá un bloque que contendrá únicamente punteros hacia el resto de bloques del archivo.
// como cada bloque
// luego cada bloque lo leo de a 8 bits (1 byte) -> tamaño estandar de stream (flujo de datos cuyo tipo no conozco) ???? COMO RESUELVO ESTO



// cada registro de archivo será un bloque, por ende cada registro del archivo consiste de 64 bytes (o lo que se indique en el archivo de config que ocupan los bloques)
//
void levantar_archivo_bloques(FILE*f,char* mapping){

	int cantidad_bloques = config_super_bloque_valores->block_count;
	int tamanio_bloques = config_super_bloque_valores->block_size;
	// - //

	f = fopen(config_valores->path_bloques,"w+");

	int fd = fileno(f);

	if(fd == -1){
		printf("Error al crear archivo de bloques\n");
		fclose(f);
	}
	int tamanio_en_bytes = cantidad_bloques * tamanio_bloques;

	ftruncate(fd,tamanio_en_bytes);

	mapping = mmap(NULL,tamanio_en_bytes,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);

	if(mapping == MAP_FAILED){
		printf("Error al mappear memoria\n");
	}


}

int obtener_posicion_archivo_bloques(int indice)
{
	int tamanio_bloques = config_super_bloque_valores->block_size;
	return indice  * tamanio_bloques;
}


// tamanio de archivo => al inicializarlo se le asigna mínimo un bloque de para almacenar punteros
// tamanio inicial de archivo es el de 1 bloque
// se asignará como primer bloque y bloque indirecto los primeros bloques disponibles, para lo cual se consulta el bitmap

char* obtener_ruta_archivo(const char* nombre_archivo)
{
	const char* ruta_incompleta = "../fileSystem/grupoDeBloques/";

	char* ruta = malloc(25*sizeof(int));
	concatenar_strings(ruta_incompleta,nombre_archivo,ruta);

	return ruta;
}


void levantar_fcb_nuevo_archivo(const char* nombre_archivo){
	FILE* f;

	int tamanio_bloques = config_super_bloque_valores->block_size;

	char* ruta = obtener_ruta_archivo(nombre_archivo);

	f = fopen(ruta,"w+");

	int fd = fileno(f);

	if(fd == -1){
		printf("Error al crear archivo de config\n");
		fclose(f);
	}

	fprintf(f,"NOMBRE_ARCHIVO=%s\n",nombre_archivo);
	fprintf(f,"TAMANIO_ARCHIVO=%i\n",tamanio_bloques*2);

	asignar_bloques_iniciales(f);
	free(ruta);
}
