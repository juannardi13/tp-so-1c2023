#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <commons/string.h>
#include <sys/mman.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

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

//	uint32_t lectura2;
//	fseek(archivo_bloques,obtener_posicion_archivo_bloques(6),0);
//	fread(&lectura2,4,1,archivo_bloques);
//	// tendría que sacar 7
//	printf("%ld\n",lectura2);
//
//	 lectura2;
//	fseek(archivo_bloques,obtener_posicion_archivo_bloques(6) + 4,0);
//	fread(&lectura2,4,1,archivo_bloques);
//	// tendría que sacar 8
//	printf("%ld\n",lectura2);

	// probé la hipotésis de acá comentando la lectura del archivo del 99 arriba
	// imprimió 7 y 8, función caso de ftruncate


	// otro caso de ftruncate
	// archivo no estaba vacio, tamanio actual es menor a tamanio de bloque, tamanio nuevo tambien menor a tam bloque

	crear_archivo("campeonesFutbolArgentino");

	truncar_archivo("campeonesFutbolArgentino",string_itoa(54));

	truncar_archivo("campeonesFutbolArgentino",string_itoa(60));

	// funca

	// archivo no estaba vacio, tamanio actual es menor a tamanio de bloque, tamanio nuevo mayor a tam bloque, tiene que asignar puntero indirecto

	crear_archivo("campeonesPremier");

	truncar_archivo("campeonesPremier",string_itoa(54));

	truncar_archivo("campeonesPremier",string_itoa(147));

	// se necesitaron 2 bloques que sean apuntados por punteros de bloque de punteros (bloque numero 11), por ende leyendo en archivo
	// desde pos 11 desplazado 0 y 4 bytes deberían leer los nros de los bloques de datos 12, 13.

//	uint32_t lectura2;
//	fseek(archivo_bloques,obtener_posicion_archivo_bloques(11),0);
//	fread(&lectura2,4,1,archivo_bloques);
//	// tendría que sacar 12
//	printf("%ld\n",lectura2);
//
//	 lectura2;
//	fseek(archivo_bloques,obtener_posicion_archivo_bloques(11) + 4,0);
//	fread(&lectura2,4,1,archivo_bloques);
//	// tendría que sacar 13
//	printf("%ld\n",lectura2);

	// funca, imprimio lo que tenía que escribir en archivo de bloques

	// archivo no estaba vacio, tamanio actual mayor a tamanio de bloque, determinar cuantos nuevos bloques tendrán que ser apuntados por punteros
	// del bloque de punteros si es que esto es necesario. Puntero indirecto es 15. Si en 137 usa 2 punteros del bloque de puntero, tiene que usar 4 en total ahora entonces
	// dos nuevo nro de bloque tendría que ser 18 y 19 (16 y 17 fueron usado para nros de bloques anteriores en primer truncamiento.

	crear_archivo("campeonesLibertadores");

	truncar_archivo("campeonesLibertadores",string_itoa(137));

	truncar_archivo("campeonesLibertadores",string_itoa(250));

		uint32_t lectura2;
		fseek(archivo_bloques,obtener_posicion_archivo_bloques(15),0);
		fread(&lectura2,4,1,archivo_bloques);
		// tendría que sacar 16
		printf("%ld\n",lectura2);

		fseek(archivo_bloques,obtener_posicion_archivo_bloques(15) + 4,0);
		fread(&lectura2,4,1,archivo_bloques);
		// tendría que sacar 17
		printf("%ld\n",lectura2);

		fseek(archivo_bloques,obtener_posicion_archivo_bloques(15) + 8,0);
		fread(&lectura2,4,1,archivo_bloques);
		// tendría que sacar 18
		printf("%ld\n",lectura2);

		fseek(archivo_bloques,obtener_posicion_archivo_bloques(15) + 12,0);
		fread(&lectura2,4,1,archivo_bloques);
		// tendría que sacar 19
		printf("%ld\n",lectura2);

		// funca

		// fin pruebas ampliar
		// archivo de 325 bytes (1 bloque con puntero directo 5 con puntero directo)
		// si nuevo tamanio < 64, por ejemplo 56, libero todos los bloques usados apuntados por punteros de bloque

		crear_archivo("campeonesSudamericana");

		truncar_archivo("campeonesSudamericana",string_itoa(325));

		truncar_archivo("campeonesSudamericana",string_itoa(56));

		crear_archivo("poleSittersHungaroring");

		// necesita 6 bloques, uno apuntado directo y 5 indirecciones
		truncar_archivo("poleSittersHungaroring",string_itoa(325));

		// necesita 3 bloques, uno apuntado directo y 2 indirecciones, tengo que liberar 3 bloques
		truncar_archivo("poleSittersHungaroring",string_itoa(160));

		// funca - fin preubas ftruncate

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

	if(nuevo_tamanio_entero > tamanio_fcb)
	{
			ampliar_tamanio(tamanio_fcb,nuevo_tamanio_entero,config_fcb_archivo);
	}
	else
	{
			reducir_tamanio(tamanio_fcb,nuevo_tamanio_entero,config_fcb_archivo);
	}

	log_info(logger,"Truncar Archivo: <%s> - Tamaño: <%s>",nombre_archivo,nuevo_tamanio);

	config_set_value(config_fcb_archivo,"TAMANIO_ARCHIVO",nuevo_tamanio);

	config_save(config_fcb_archivo);

	config_destroy(config_fcb_archivo);

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



void reducir_tamanio(int tamanio_fcb,int nuevo_tamanio_entero,t_config* config_fcb_archivo)
{

	int tamanio_bloque = config_super_bloque_valores->block_size;


	if(tamanio_fcb <= tamanio_bloque)
	{

	}
	else
	{

		int nro_bloque_punteros_indirectos = config_get_int_value(config_fcb_archivo,"PUNTERO_INDIRECTO");

		int nro_ultima_entrada_bloque_pind = ceil((double)(tamanio_fcb - tamanio_bloque) / (double)tamanio_bloque) -1;

		if(nuevo_tamanio_entero <= tamanio_bloque)
		{
			reducir_tamanio_cuando_tam_actual_mayor_tam_bloque_y_nuevo_tam_menor_tam_bloque(nro_ultima_entrada_bloque_pind,nro_bloque_punteros_indirectos);
		}
		else
		{
			int cantidad_bloques_apuntados_necesarios = ceil((double)nuevo_tamanio_entero / (double)tamanio_bloque) - 1;

			reducir_tamanio_cuando_tam_actual_mayor_tam_bloque_y_nuevo_tam_mayor_tam_bloque(nro_ultima_entrada_bloque_pind,nro_bloque_punteros_indirectos,cantidad_bloques_apuntados_necesarios);
		}

	}
}


void ampliar_tamanio(int tamanio_fcb,int nuevo_tamanio_entero,t_config* config_fcb_archivo)
{
	int tamanio_bloque = config_super_bloque_valores->block_size;

	if(tamanio_fcb == 0)
	{
		if(nuevo_tamanio_entero <= tamanio_bloque)
		{
			ampliar_con_tam_actual_cero_y_tam_nuevo_menor_igual_tam_bloque(config_fcb_archivo);
		}
		else
		{
			ampliar_con_tam_actual_cero_y_tam_nuevo_mayor_tam_bloque(nuevo_tamanio_entero,config_fcb_archivo);
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
				ampliar_con_tam_actual_menor_tam_bloque_tam_nuevo_mayor_tam_bloque(nuevo_tamanio_entero,config_fcb_archivo);
			}
		}
		else
		{
			ampliar_con_tam_actual_mayor_tam_bloque(tamanio_fcb,nuevo_tamanio_entero,config_fcb_archivo);
		}
	}
}





void concatenar_strings(const char* s1, const char* s2,char* buffer){
	memccpy(memccpy(buffer,s1,'\0',100)-1,s2,'\0',100);
}


t_bitarray* inicializar_archivo_bm(FILE* f){

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

	int nro_primer_bloque = primer_bloque_disponible();

	acceso_escritura_bitmap(nro_primer_bloque,1);

	int nro_bloque_punteros = primer_bloque_disponible();

	acceso_escritura_bitmap(nro_bloque_punteros,1);

	config_set_value(config_fcb_archivo,"PUNTERO_DIRECTO",string_itoa(nro_primer_bloque));
	config_set_value(config_fcb_archivo,"PUNTERO_INDIRECTO",string_itoa(nro_bloque_punteros));

	config_save(config_fcb_archivo);

	printf("Prueba cantidad keys config: %i\n",config_keys_amount(config_fcb_archivo));
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

char* obtener_ruta_archivo(const char* nombre_archivo)
{
	const char* ruta_incompleta = "../fileSystem/grupoDeBloques/";

	char* ruta = malloc(25*sizeof(int));
	concatenar_strings(ruta_incompleta,nombre_archivo,ruta);

	return ruta;
}



