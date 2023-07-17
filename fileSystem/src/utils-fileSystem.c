#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <commons/string.h>
#include <sys/mman.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// dps cambiar log_info a donde esta el manejo de conexión, están en las funciones de manera provisional

void probando_cositas(){
////	 Prueba respuesta a petición de creación de archivo (SEGUIR) //
////	 --- INICIO --- //
//	printf("%i\n",(int)estructura_bitmap->size);
//
//	acceso_lectura_bitmap(0);
//
////	levantar_fcb_nuevo_archivo("fcbPrueba");
////	t_config* configTCB = iniciar_config("../fileSystem/grupoDeBloques/fcbPrueba");
////	t_fcb fcb;
//
//
////
////	fcb.puntero_directo = config_get_int_value(configTCB,"PUNTERO_DIRECTO");
////	fcb.nombre_archivo = config_get_string_value(configTCB,"NOMBRE_ARCHIVO");
////
////	printf("Puntero directo: %d \n",fcb.puntero_directo);
////	printf("Nombre archivo:  %s \n",fcb.nombre_archivo);
//
//	if(abrir_archivo("fcbPrueba"))
//	{
//		printf("Existe el archivo solicitado fcbPrueba \n");
//	}
//	else
//	{
//		printf("No existe el archivo solicitado fcbPrueba \n");
//	}
//
//	crear_archivo("campeonesF1");
//
//	if(abrir_archivo("campeonesF1"))
//	{
//		printf("Existe el archivo solicitado campeonesF1 \n");
//	}
//	else
//	{
//		printf("No existe el archivo solicitado campeonesF1 \n");
//	}

//	acceso_lectura_bitmap(0);
//
////	truncar_archivo("campeonesF1","125",mapping_archivo_bloques);
//
//	int a = mapping_archivo_bloques;
//
//	printf("%i \n",a);
//
//	obtener_ruta_archivo("ArchivoEjemplo");
//
////	 --- FIN --- //
////
////
////	 Verificación bloques asignados a archivo creado se indican como ocupados //
////	 --- INICIO --- //
//	FILE* f;
//	f = fopen("../fileSystem/grupoDeBloques/bitmap.bin","r+");
//
//
//	if(fileno(f) == -1){
//		printf("Error al abrir archivo bitmap\n");
//	}
//
//		fseek(f,0,0);
//
//		unsigned char b;
//
//
//			fread(&b,1,1,f);
//			printf("%u\n",b);
//
//
//		fclose(f);
	// --- FIN --- //

	crear_archivo("campeonesFormula1");

	// PROVISIONAL -> probando que funcionen cambios sobre FCB (osea entrada de directorio) - dps sacar porque no corresponde a creación de archivo
	// pues no se asignan bloques cuando se crea !

	char* ruta1 = obtener_ruta_archivo("campeonesFormula1");

	t_config* config_fcb_archivo1 = iniciar_config(ruta1);

	asignar_bloques_iniciales(ruta1,config_fcb_archivo1);


	// PROVISIONAL !

//
//	// veo si con asignación de bloques cambié archivo bitmap
//	// 0 y 1 ocupados, osea primer byte es 00000011, osea 3


	unsigned char b;
	fseek(archivo_bm,0,0);
	fread(&b,1,1,archivo_bm);
	printf("%u\n",b);

	// funco :)

// luego si creo otro archivo asigna bloques 2,3
// tendría como primer byte 00001111, considerar que cada unsigned byte son 4 bits se obtendrán dos 3

	crear_archivo("campeonesF1");

	char* ruta2 = obtener_ruta_archivo("campeonesF1");

	t_config* config_fcb_archivo2 = iniciar_config(ruta2);

	asignar_bloques_iniciales(ruta2,config_fcb_archivo2);

	fseek(archivo_bm,0,0);
	fread(&b,1,1,archivo_bm);
	printf("%u\n",b);

// funco

// pruebo si al escribir en direcc memoria mappeada por ejemplo en bloque de puntero indirecto , se efectua sobre archivo
// por ejempl para campeonesF1 el bloque de punteros indirectos es el bloque nro 3.

	int nro_bloque = 99;

	memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(3),&nro_bloque,sizeof(uint32_t));

	uint32_t p = *(mapping_archivo_bloques + obtener_posicion_archivo_bloques(3));

	printf("%ld\n",p);

	// ARRIBA MUESTRO QUE SE ESCRIBIÓ 99 EN MEMORIA

//	uint32_t lectura;
//	fseek(archivo_bloques,obtener_posicion_archivo_bloques(3),0);
//	fread(&lectura,4,1,archivo_bloques);
//	// tendría que sacar 99
//	printf("%ld\n",lectura);

	// ARRIBA MUESTRO QUE SE ESCRIBIÓ 99 EN EL ARCHIVO


	// pruebo caso truncar archivo vacio y tamaño nuevo menor o igual a bloque -> cambia tamaño en fcb y puntero directo
	crear_archivo("escuderiasF1");

	truncar_archivo("escuderiasF1",string_itoa(56));

	// funca

	// pruebo caso truncar archivo vacio y tamaño mayor a bloque -> cambia tamaño en fcb, puntero directo y puntero indirecto y escribe sobre bloque
	// de puntero los primeros punteros
	crear_archivo("plantelArgentinaCampeona");

	truncar_archivo("plantelArgentinaCampeona",string_itoa(137));

	// puntero indirecto asignado es el 6
	// debería poder leer el nro de bloque asignado al primer bloque de datos accedido por indirección para el archivo, que sería 7 y 8
	// NOTA: PROBANDO PONIENDO ACÁ ABAJO EL MEMCPY SOBRE LA DIRECCIÓN DE MEMORIA VINCULADA CON EL ARCHIVO DE BLOQUES ME DI CUENTA
	// QUE NUNCA LO SINCRONIZABA AL BLOQUE. SUpongo que una vez accedido el archivo para lectura como hice arriba para mostrar
	// el 99 se rompe el vínculo del mmap. Primero hacer todas las escrituras y después si quiero probar, leer el archivo sabiendo que no lo voy
	// a poder volver a escribir por modificar la direcc del mmap.

	uint32_t lectura2;
	fseek(archivo_bloques,obtener_posicion_archivo_bloques(6),0);
	fread(&lectura2,4,1,archivo_bloques);
	// tendría que sacar 7
	printf("%ld\n",lectura2);

	 lectura2;
	fseek(archivo_bloques,obtener_posicion_archivo_bloques(6) + 4,0);
	fread(&lectura2,4,1,archivo_bloques);
	// tendría que sacar 7
	printf("%ld\n",lectura2);

	// probé la hipotésis de acá comentando la lectura del archivo del 99 arriba
	// imprimió 7 y 8, función caso de ftruncate

	// otro caso de ftruncate



}


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

	log_info(logger,"Abrir Archivo: <%s>",nombre_archivo);
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

	int cantidad_punteros_bloque = config_super_bloque_valores->block_size / sizeof(uint32_t);

	if(nuevo_tamanio_entero > tamanio_fcb)
	{
			ampliar_tamanio(tamanio_fcb,nuevo_tamanio_entero,tamanio_bloque,cantidad_punteros_bloque,cantidad_bloques,config_fcb_archivo);
	}
	else
	{
			reducir_tamanio(tamanio_fcb,nuevo_tamanio_entero,tamanio_bloque,cantidad_punteros_bloque,config_fcb_archivo);
	}

	log_info(logger,"Truncar Archivo: <%s> - Tamaño: <%s>",nombre_archivo,nuevo_tamanio);

//	msync(mapping_archivo_bloques,tamanio_bloque*cantidad_bloques,MS_SYNC);

	config_set_value(config_fcb_archivo,"TAMANIO_ARCHIVO",nuevo_tamanio);

	config_save(config_fcb_archivo);

	fclose(f);

}

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

void reducir_tamanio(int tamanio_fcb,int nuevo_tamanio_entero,int tamanio_bloque,t_config* config_fcb_archivo)
{
if(tamanio_fcb <= tamanio_bloque)
		{
			// no hace nada, solo cambio numero indicado en fcb
		}
		else
		{
			// tamanio nuevo menor a tamanio fcb y tamanio fcb mayor a tamanio bloque
			// ver con tamanio nuevo cuantos bloques voy a necesitar

			int nro_bloque_punteros_indirectos = config_get_int_value(config_fcb_archivo,"PUNTERO_INDIRECTO");

			int nro_ultima_entrada_bloque_pind = ceil((tamanio_fcb - tamanio_bloque) / tamanio_bloque) -1;

			if(nuevo_tamanio_entero <= tamanio_bloque)
			{
				// liberar a todos bloques apuntados por punteros indirectos

				for(int i = 0; i <= nro_ultima_entrada_bloque_pind; i++)
				{
					int off = sizeof(uint32_t) * i;

					int nro_bloque;

					memcpy(&nro_bloque,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_punteros_indirectos) + off,sizeof(uint32_t));

					bitarray_clean_bit(estructura_bitmap,nro_bloque);
				}

			}
			else
			{
				int cantidad_bloques_apuntados_necesarios = ceil(nuevo_tamanio_entero / tamanio_bloque) - 1;

				for(int i = nro_ultima_entrada_bloque_pind; i >= cantidad_bloques_apuntados_necesarios;i--)
				{
					int off = sizeof(uint32_t) * i;

					int nro_bloque;

					memcpy(&nro_bloque,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_punteros_indirectos) + off,sizeof(uint32_t));

					bitarray_clean_bit(estructura_bitmap,nro_bloque);
				}

			}

		}
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

			int cantidad_bloques_puntero_indirecto = ceil((((double)(nuevo_tamanio_entero - tamanio_bloque)) / ((double)tamanio_bloque)));

			int puntero_directo = primer_bloque_disponible(estructura_bitmap);

			bitarray_set_bit(estructura_bitmap,puntero_directo);

			char* puntero_directo_string = string_itoa(puntero_directo);

			config_set_value(config_fcb_archivo,"PUNTERO_DIRECTO",puntero_directo_string);

			int puntero_indirecto = primer_bloque_disponible(estructura_bitmap);

			bitarray_set_bit(estructura_bitmap,puntero_indirecto);

			config_set_value(config_fcb_archivo,"PUNTERO_INDIRECTO",string_itoa(puntero_indirecto));

			for(int i = 0; i <  cantidad_punteros_bloque;i++)
			{
				int off = sizeof(uint32_t) * i;

				if(i < cantidad_bloques_puntero_indirecto)
				{

					int bloque_datos = primer_bloque_disponible(estructura_bitmap);

					bitarray_set_bit(estructura_bitmap,bloque_datos);

					// esto funcionaba afuera, porque no adentro de la función?

//					int nro_bloque2 = 7;
//
//					memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(6),&nro_bloque2,sizeof(uint32_t));
//
//					uint32_t h = *(mapping_archivo_bloques + obtener_posicion_archivo_bloques(6));
//
//					printf("%ld\n",h);

					memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto) + off,&bloque_datos,sizeof(uint32_t));

				}
				else
				{
					int menos_uno = -1;
					memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto)+ off,&menos_uno,sizeof(uint32_t));
				}
			}
		}
	}
	else
	{
		if(tamanio_fcb <= tamanio_bloque)
		{
			if(nuevo_tamanio_entero <= tamanio_bloque)
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

					int off = sizeof(uint32_t) * i;

					if(i < cantidad_bloques_puntero_indirecto)
					{

						int bloque_datos = primer_bloque_disponible(estructura_bitmap);

						bitarray_set_bit(estructura_bitmap,bloque_datos);

						// escribir número de bloque asignado en el bloque del puntero indirecto (todavía no hecho)

						memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto) + off,&bloque_datos,sizeof(uint32_t));
					}
					else
					{
						memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto) + off,-1,sizeof(uint32_t));
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

			int nro_bloque_punteros_indirectos = config_get_int_value(config_fcb_archivo,"PUNTERO_INDIRECTO");

			int j = 0;

			for(int i = cantidad_indirecciones_usadas; i < (cantidad_indirecciones_usadas + cantidad_punteros_indirectos) ;i++)
			{
				int off = sizeof(uint32_t) * j;

				int bloque_datos = primer_bloque_disponible();

				bitarray_set_bit(estructura_bitmap,bloque_datos);

				// escribir número de bloque asignado en el bloque del puntero indirecto (todavía no hecho)

				memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_punteros_indirectos) + off,&bloque_datos,sizeof(uint32_t));

				j++;
			}

		}
	}


}





void concatenar_strings(const char* s1, const char* s2,char* buffer){
	memccpy(memccpy(buffer,s1,'\0',100)-1,s2,'\0',100);
}


t_bitarray* inicializar_archivo_bm(FILE* f){

//	f = fopen(config_get_string_value(config,"PATH_BITMAP"),"w+");

	archivo_bm = fopen("../fileSystem/grupoDeBloques/bitmap.bin","w+");

	int fd = fileno(archivo_bm);

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
		fwrite(&byte,1,1,archivo_bm);
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


void asignar_bloques_iniciales(char* ruta,t_config* config_fcb_archivo){
//	FILE* f;
//	f = fopen("../fileSystem/grupoDeBloques/bitmap.bin","r+");
//
//	if(fileno(f) == -1){
//		printf("Error al abrir archivo bitmap\n");
//	}

	int nro_primer_bloque = primer_bloque_disponible(estructura_bitmap);

//	bitarray_set_bit(estructura_bitmap,nro_primer_bloque);

	acceso_escritura_bitmap(nro_primer_bloque,1);
//
//	t_config* config_fcb_archivo_copia = config_fcb_archivo;
//
	int nro_bloque_punteros = primer_bloque_disponible(estructura_bitmap);

//	bitarray_set_bit(estructura_bitmap,nro_bloque_punteros);

	acceso_escritura_bitmap(nro_bloque_punteros,1);

	char* nombre = config_get_string_value(config_fcb_archivo,"NOMBRE_ARCHIVO");

	config_set_value(config_fcb_archivo,"PUNTERO_DIRECTO",string_itoa(nro_primer_bloque));
	config_set_value(config_fcb_archivo,"PUNTERO_INDIRECTO",string_itoa(nro_bloque_punteros));

	config_save(config_fcb_archivo);

	printf("Prueba cantidad keys config: %i\n",config_keys_amount(config_fcb_archivo));
	// da dos, por algun extraño motivo set value borra las llaves anteriores


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
void levantar_archivo_bloques(){

	int cantidad_bloques = config_super_bloque_valores->block_count;
	int tamanio_bloques = config_super_bloque_valores->block_size;
	// - //

	archivo_bloques = fopen(config_valores->path_bloques,"w+");

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


//void levantar_fcb_nuevo_archivo(const char* nombre_archivo){
//	FILE* f;
//
//	int tamanio_bloques = config_super_bloque_valores->block_size;
//
//	char* ruta = obtener_ruta_archivo(nombre_archivo);
//
//	f = fopen(ruta,"w+");
//
//	int fd = fileno(f);
//
//	if(fd == -1){
//		printf("Error al crear archivo de config\n");
//		fclose(f);
//	}
//
//	fprintf(f,"NOMBRE_ARCHIVO=%s\n",nombre_archivo);
//	fprintf(f,"TAMANIO_ARCHIVO=%i\n",tamanio_bloques*2);
//
//	asignar_bloques_iniciales(f);
//	free(ruta);
//}
