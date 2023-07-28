#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <commons/string.h>
#include <sys/mman.h>
#include <stdint.h>
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

	config_destroy(config_fcb_archivo1);

	free(ruta1);

	// PROVISIONAL !

//
//	// veo si con asignación de bloques cambié archivo bitmap
//	// 0 y 1 ocupados, osea primer byte es 00000011, osea 3


	unsigned char b;
	fseek(archivo_bm,0,0);
	fread(&b,1,1,archivo_bm);
//	printf("%u\n",b);

	// funco :)

// luego si creo otro archivo asigna bloques 2,3
// tendría como primer byte 00001111, considerar que cada unsigned byte son 4 bits se obtendrán dos 3

	crear_archivo("campeonesF1");

	char* ruta2 = obtener_ruta_archivo("campeonesF1");

	t_config* config_fcb_archivo2 = iniciar_config(ruta2);

	asignar_bloques_iniciales(ruta2,config_fcb_archivo2);

	config_destroy(config_fcb_archivo2);

	free(ruta2);

	fseek(archivo_bm,0,0);
	fread(&b,1,1,archivo_bm);
//	printf("%u\n",b);

// funco

// pruebo si al escribir en direcc memoria mappeada por ejemplo en bloque de puntero indirecto , se efectua sobre archivo
// por ejempl para campeonesF1 el bloque de punteros indirectos es el bloque nro 3.

	int nro_bloque = 99;

	memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(3),&nro_bloque,sizeof(uint32_t));

	uint32_t p = *(mapping_archivo_bloques + obtener_posicion_archivo_bloques(3));

//	printf("%ld\n",p);

	// ARRIBA MUESTRO QUE SE ESCRIBIÓ 99 EN MEMORIA

//	uint32_t lectura;
//	fseek(archivo_bloques,obtener_posicion_archivo_bloques(3),0);
//	fread(&lectura,4,1,archivo_bloques);
//	// tendría que sacar 99
//	printf("%ld\n",lectura);

	// ARRIBA MUESTRO QUE SE ESCRIBIÓ 99 EN EL ARCHIVO


	// pruebo caso truncar archivo vacio y tamaño nuevo menor o igual a bloque -> cambia tamaño en fcb y puntero directo
	crear_archivo("escuderiasF1");

//	char* string_itoa_11 = string_itoa(56);

	truncar_archivo("escuderiasF1",56);

//	free(string_itoa_11);

	// funca

	// pruebo caso truncar archivo vacio y tamaño mayor a bloque -> cambia tamaño en fcb, puntero directo y puntero indirecto y escribe sobre bloque
	// de puntero los primeros punteros
	crear_archivo("plantelArgentinaCampeona");

//	char* string_itoa_12 = string_itoa(137);

	truncar_archivo("plantelArgentinaCampeona",137);

//	free(string_itoa_12);

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

//	char* string_itoa_1 = string_itoa(54);

	truncar_archivo("campeonesFutbolArgentino",54);

//	free(string_itoa_1);

//	char* string_itoa_2 = string_itoa(60);

	truncar_archivo("campeonesFutbolArgentino",60);

//	free(string_itoa_2);

	// funca

	// archivo no estaba vacio, tamanio actual es menor a tamanio de bloque, tamanio nuevo mayor a tam bloque, tiene que asignar puntero indirecto

	crear_archivo("campeonesPremier");

//	char* string_itoa_3 = string_itoa(54);

	truncar_archivo("campeonesPremier",54);

//	free(string_itoa_3);

//	char* string_itoa_4 = string_itoa(147);

	truncar_archivo("campeonesPremier",147);

//	free(string_itoa_4);

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

//	char* string_itoa_5 = string_itoa(137);

	truncar_archivo("campeonesLibertadores",137);

//	free(string_itoa_5);

//	char* string_itoa_6 = string_itoa(250);

	truncar_archivo("campeonesLibertadores",250);

//	free(string_itoa_6);

		uint32_t lectura2;
		fseek(archivo_bloques,obtener_posicion_archivo_bloques(15),0);
		fread(&lectura2,4,1,archivo_bloques);
		// tendría que sacar 16
//		printf("%ld\n",lectura2);

		fseek(archivo_bloques,obtener_posicion_archivo_bloques(15) + 4,0);
		fread(&lectura2,4,1,archivo_bloques);
		// tendría que sacar 17
//		printf("%ld\n",lectura2);

		fseek(archivo_bloques,obtener_posicion_archivo_bloques(15) + 8,0);
		fread(&lectura2,4,1,archivo_bloques);
		// tendría que sacar 18
//		printf("%ld\n",lectura2);

		fseek(archivo_bloques,obtener_posicion_archivo_bloques(15) + 12,0);
		fread(&lectura2,4,1,archivo_bloques);
		// tendría que sacar 19
//		printf("%ld\n",lectura2);

		// funca

		// fin pruebas ampliar
		// archivo de 325 bytes (1 bloque con puntero directo 5 con puntero directo)
		// si nuevo tamanio < 64, por ejemplo 56, libero todos los bloques usados apuntados por punteros de bloque

		crear_archivo("campeonesSudamericana");

//		char* string_itoa_7 = string_itoa(325);

		truncar_archivo("campeonesSudamericana",325);

//		free(string_itoa_7);

//		char* string_itoa_8 = string_itoa(56);

		truncar_archivo("campeonesSudamericana",56);

//		free(string_itoa_8);

		crear_archivo("poleSittersHungaroring");

//		char* string_itoa_9 = string_itoa(325);

		// necesita 6 bloques, uno apuntado directo y 5 indirecciones
		truncar_archivo("poleSittersHungaroring",325);

//		free(string_itoa_9);

//		char* string_itoa_10 = string_itoa(160);

		// necesita 3 bloques, uno apuntado directo y 2 indirecciones, tengo que liberar 3 bloques
		truncar_archivo("poleSittersHungaroring",160);

//		free(string_itoa_10);

		// funca - fin preubas ftruncate

		// probando cosas de escritua y lectura de memoria
		// 1 byte por caracter + 1 adicional, osea acá necesito 16 bytes
		// voy a pedir 32 igual para probar de escribirlos separados como sucedería cuando está en bloques distintos

		char* unString = "Fernando Alonso";

		char* unaDireccionCualq = malloc(32);

		// por ejemplo escribo 4 bytes desde 0, osea del byte 0 al 3
		// despúes escribo 12 bytes desde 6 hasta 17
		// el tema es que por cada división del string no escribo n caracteres sino n-1 porque por cada uno me cuenta el \n.
		// entonces por cada división de string es un byte adicional que necesito
		// por eso para cada string a leer se recomienda mandar en la instrucción más que la cantidad de bytes del string
		// ya que si me paso no pasa nada. Es decir, por ejemplo si me dijeron que tenia que leer 20 bytes y leo los primeros 5
		// "Fern\n" luego cuando haga el memcpy con los restantes (15) de igual manera me trae los 12 del string faltante "ando Alonso\n"


//		memcpy(unaDireccionCualq,string_substring(unString,0,4),5);
//
//		memcpy(unaDireccionCualq + 6,string_substring(unString,4,4),5);
//
//		memcpy(unaDireccionCualq + 14,string_substring(unString,8,7),8);
//
//		char* s1 = string_new();
//		char* s2 = string_new();
//		char* s4 = string_new();
//
//		memcpy(s1,unaDireccionCualq,5);
//
//		memcpy(s2,unaDireccionCualq + 6,5);
//
//		memcpy(s4,unaDireccionCualq + 14,8);
//
//		char* s3 = string_new();
//
//		string_append(&s3,s1);
//		string_append(&s3,s2);
//		string_append(&s3,s4);
//
//		printf("%s\n",s1);
//		printf("%s\n",s2);
//		printf("%s\n",s4);
//		printf("%s\n",s3);

		char* stringPrueba = "River Plate";
		char* unaDirecc = malloc(12);
		memcpy(unaDirecc,stringPrueba,12);

		escribir_archivo("campeonesLibertadores",30,unaDirecc,12);

		// se que el primer bloque del archivo es el 14 (me fijé en fcb). Si me voy a este primer bloque y me desplazo 30 y leo tendría que obtener el string

//		char* otroString = malloc(12);
//
//		memcpy(otroString,mapping_archivo_bloques + obtener_posicion_archivo_bloques(14) + 30,12);
//
//		printf("Leido: %s\n",otroString);

		void* leo_archivo_1;
		leo_archivo_1 = leer_archivo("campeonesLibertadores",30,12);

//		char* l1aux = leo_archivo_1;
//
		printf("Leido con fread: %s\n:",leo_archivo_1);

		free(leo_archivo_1);

		// funca

		char* stringPrueba2 = "Boca Juniors";
		char* unaDirecc2 = malloc(13);
		memcpy(unaDirecc2,stringPrueba2,13);

		escribir_archivo("campeonesLibertadores",70,unaDirecc2,13);

		// se que el primer bloque del archivo es el 14 (me fijé en fcb). Si me voy a este primer bloque y me desplazo 30 y leo tendría que obtener el string

		char* otroString2 = malloc(13);

		memcpy(otroString2,mapping_archivo_bloques + obtener_posicion_archivo_bloques(16) + 6,13);

		printf("Leido: %s\n",otroString2);

		void* leo_archivo_2 = leer_archivo("campeonesLibertadores",70,13);

		char* l2aux = leo_archivo_2;

		printf("Leido con fread: %s\n:",l2aux);

		free(leo_archivo_2);

		char* stringPrueba3 = "Racing";
		char* unaDirecc3 = malloc(10);
		memcpy(unaDirecc3,stringPrueba3,10);

		escribir_archivo("campeonesLibertadores",60,unaDirecc3,10);

//		char* otroString3 = malloc(4);
//		char* otroString4 = malloc(6);
//		char* otroString5 = malloc(10);

		// necesito para leer offset dentro de bloque y cantidad de bytes a escribir en bloque (ver en fwrite)

//		memcpy(otroString3,mapping_archivo_bloques + obtener_posicion_archivo_bloques(14) + 60,4);
//		memcpy(otroString4,mapping_archivo_bloques + obtener_posicion_archivo_bloques(16),6);

//		string_append(&otroString5,otroString3);
//		string_append(&otroString5,otroString4);
//
//		printf("Leido: %s\n",otroString5);

		void* leo_archivo_3;
		leo_archivo_3 = leer_archivo("campeonesLibertadores",60,10);

		char* l3aux = leo_archivo_3;

		printf("Leido con fread: %s\n:",l3aux);

		free(leo_archivo_3);

		char* stringPrueba4 = "Independiente";
		char* unaDirecc4 = malloc(16);

		memcpy(unaDirecc4,stringPrueba4,16);

		escribir_archivo("campeonesLibertadores",185,unaDirecc4,16);

//		char* otroString6 = string_new();
//		char* otroString7 = string_new();
//		char* otroString8 = string_new();
//
//		memcpy(otroString6,mapping_archivo_bloques + obtener_posicion_archivo_bloques(17) + 57,7);
//		memcpy(otroString7,mapping_archivo_bloques + obtener_posicion_archivo_bloques(18),9);
//
////		string_append(&otroString8,otroString6);
////		string_append(&otroString8,otroString7);
////
////		printf("Leido: %s\n",otroString8);

		char* leo_archivo_4 = leer_archivo("campeonesLibertadores",185,16);
		printf("Leido con fread: %s\n",leo_archivo_4);

		int rta = abrir_archivo("hey");

		printf("%i\n",rta);


		char* stringPrueba5 = "SonyPlaystation1SonyPlaystation2SonyPlaystation3SonyPlaystation4SonyPlaystation5";
		char* unaDirecc5 = malloc(80);
		memset(unaDirecc5,0,80);

		memcpy(unaDirecc5,stringPrueba5,80);

		escribir_archivo("Consoles",128,unaDirecc5,80);

		char* leo_archivo_5 = leer_archivo("Consoles",128,80);
		printf("Leido con fread: %s\n",leo_archivo_5);

//		free(otroString8);
//		free(otroString7);
//		free(otroString6);
//		free(otroString5);
//		free(otroString4);
//		free(otroString3);
		free(otroString2);
//		free(otroString);
		free(unaDirecc4);
		free(unaDireccionCualq);
//		free(s1);
//		free(s2);
//		free(s3);
//		free(s4);
		free(unaDirecc3);
		free(unaDirecc2);
		free(unaDirecc);
}

void probando_cositas_2(){
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

//	crear_archivo("campeonesFormula1");

	// PROVISIONAL -> probando que funcionen cambios sobre FCB (osea entrada de directorio) - dps sacar porque no corresponde a creación de archivo
	// pues no se asignan bloques cuando se crea !

//	char* ruta1 = obtener_ruta_archivo("campeonesFormula1");
//
//	t_config* config_fcb_archivo1 = iniciar_config(ruta1);
//
//	asignar_bloques_iniciales(ruta1,config_fcb_archivo1);
//
//	config_destroy(config_fcb_archivo1);
//
//	free(ruta1);

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
//
//	crear_archivo("campeonesF1");
//
//	char* ruta2 = obtener_ruta_archivo("campeonesF1");
//
//	t_config* config_fcb_archivo2 = iniciar_config(ruta2);
//
//	asignar_bloques_iniciales(ruta2,config_fcb_archivo2);
//
//	config_destroy(config_fcb_archivo2);
//
//	free(ruta2);

	fseek(archivo_bm,0,0);
	fread(&b,1,1,archivo_bm);
	printf("%u\n",b);

// funco

// pruebo si al escribir en direcc memoria mappeada por ejemplo en bloque de puntero indirecto , se efectua sobre archivo
// por ejempl para campeonesF1 el bloque de punteros indirectos es el bloque nro 3.

//	int nro_bloque = 99;
//
//	memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(3),&nro_bloque,sizeof(uint32_t));
//
//	uint32_t p = *(mapping_archivo_bloques + obtener_posicion_archivo_bloques(3));

//	printf("%ld\n",p);

	// ARRIBA MUESTRO QUE SE ESCRIBIÓ 99 EN MEMORIA

//	uint32_t lectura;
//	fseek(archivo_bloques,obtener_posicion_archivo_bloques(3),0);
//	fread(&lectura,4,1,archivo_bloques);
//	// tendría que sacar 99
//	printf("%ld\n",lectura);

	// ARRIBA MUESTRO QUE SE ESCRIBIÓ 99 EN EL ARCHIVO


	// pruebo caso truncar archivo vacio y tamaño nuevo menor o igual a bloque -> cambia tamaño en fcb y puntero directo
//	crear_archivo("escuderiasF1");

//	char* string_itoa_11 = string_itoa(56);

//	truncar_archivo("escuderiasF1",56);

//	free(string_itoa_11);

	// funca

	// pruebo caso truncar archivo vacio y tamaño mayor a bloque -> cambia tamaño en fcb, puntero directo y puntero indirecto y escribe sobre bloque
	// de puntero los primeros punteros
//	crear_archivo("plantelArgentinaCampeona");

//	char* string_itoa_12 = string_itoa(137);

//	truncar_archivo("plantelArgentinaCampeona",137);

//	free(string_itoa_12);

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

//	crear_archivo("campeonesFutbolArgentino");

//	char* string_itoa_1 = string_itoa(54);

//	truncar_archivo("campeonesFutbolArgentino",54);

//	free(string_itoa_1);

//	char* string_itoa_2 = string_itoa(60);

//	truncar_archivo("campeonesFutbolArgentino",60);

//	free(string_itoa_2);

	// funca

	// archivo no estaba vacio, tamanio actual es menor a tamanio de bloque, tamanio nuevo mayor a tam bloque, tiene que asignar puntero indirecto

//	crear_archivo("campeonesPremier");

//	char* string_itoa_3 = string_itoa(54);

//	truncar_archivo("campeonesPremier",54);

//	free(string_itoa_3);

//	char* string_itoa_4 = string_itoa(147);

//	truncar_archivo("campeonesPremier",147);

//	free(string_itoa_4);

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

//	crear_archivo("campeonesLibertadores");

//	char* string_itoa_5 = string_itoa(137);

//	truncar_archivo("campeonesLibertadores",137);

//	free(string_itoa_5);

//	char* string_itoa_6 = string_itoa(250);

//	truncar_archivo("campeonesLibertadores",250);

//	free(string_itoa_6);

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

//		crear_archivo("campeonesSudamericana");

//		char* string_itoa_7 = string_itoa(325);

//		truncar_archivo("campeonesSudamericana",325);

//		free(string_itoa_7);

//		char* string_itoa_8 = string_itoa(56);

//		truncar_archivo("campeonesSudamericana",56);

//		free(string_itoa_8);

//		crear_archivo("poleSittersHungaroring");

//		char* string_itoa_9 = string_itoa(325);

		// necesita 6 bloques, uno apuntado directo y 5 indirecciones
//		truncar_archivo("poleSittersHungaroring",325);

//		free(string_itoa_9);

//		char* string_itoa_10 = string_itoa(160);

		// necesita 3 bloques, uno apuntado directo y 2 indirecciones, tengo que liberar 3 bloques
//		truncar_archivo("poleSittersHungaroring",160);

//		free(string_itoa_10);

		// funca - fin preubas ftruncate

		// probando cosas de escritua y lectura de memoria
		// 1 byte por caracter + 1 adicional, osea acá necesito 16 bytes
		// voy a pedir 32 igual para probar de escribirlos separados como sucedería cuando está en bloques distintos

		char* unString = "Fernando Alonso";

		char* unaDireccionCualq = malloc(32);

		// por ejemplo escribo 4 bytes desde 0, osea del byte 0 al 3
		// despúes escribo 12 bytes desde 6 hasta 17
		// el tema es que por cada división del string no escribo n caracteres sino n-1 porque por cada uno me cuenta el \n.
		// entonces por cada división de string es un byte adicional que necesito
		// por eso para cada string a leer se recomienda mandar en la instrucción más que la cantidad de bytes del string
		// ya que si me paso no pasa nada. Es decir, por ejemplo si me dijeron que tenia que leer 20 bytes y leo los primeros 5
		// "Fern\n" luego cuando haga el memcpy con los restantes (15) de igual manera me trae los 12 del string faltante "ando Alonso\n"


//		memcpy(unaDireccionCualq,string_substring(unString,0,4),5);
//
//		memcpy(unaDireccionCualq + 6,string_substring(unString,4,4),5);
//
//		memcpy(unaDireccionCualq + 14,string_substring(unString,8,7),8);
//
//		char* s1 = string_new();
//		char* s2 = string_new();
//		char* s4 = string_new();
//
//		memcpy(s1,unaDireccionCualq,5);
//
//		memcpy(s2,unaDireccionCualq + 6,5);
//
//		memcpy(s4,unaDireccionCualq + 14,8);
//
//		char* s3 = string_new();
//
//		string_append(&s3,s1);
//		string_append(&s3,s2);
//		string_append(&s3,s4);
//
//		printf("%s\n",s1);
//		printf("%s\n",s2);
//		printf("%s\n",s4);
//		printf("%s\n",s3);

//		char* stringPrueba = "River Plate";
//		char* unaDirecc = malloc(12);
//		memcpy(unaDirecc,stringPrueba,12);
//
//		escribir_archivo("campeonesLibertadores",30,unaDirecc,12);

		// se que el primer bloque del archivo es el 14 (me fijé en fcb). Si me voy a este primer bloque y me desplazo 30 y leo tendría que obtener el string

//		char* otroString = malloc(12);
//
//		memcpy(otroString,mapping_archivo_bloques + obtener_posicion_archivo_bloques(14) + 30,12);
//
//		printf("Leido: %s\n",otroString);

		void* leo_archivo_1;
		leo_archivo_1 = leer_archivo("campeonesLibertadores",30,12);

//		char* l1aux = leo_archivo_1;
//
		printf("Leido con fread: %s\n:",leo_archivo_1);

		free(leo_archivo_1);

		// funca

//		char* stringPrueba2 = "Boca Juniors";
//		char* unaDirecc2 = malloc(13);
//		memcpy(unaDirecc2,stringPrueba2,13);
//
//		escribir_archivo("campeonesLibertadores",70,unaDirecc2,13);

		// se que el primer bloque del archivo es el 14 (me fijé en fcb). Si me voy a este primer bloque y me desplazo 30 y leo tendría que obtener el string

//		char* otroString2 = malloc(13);
//
//		memcpy(otroString2,mapping_archivo_bloques + obtener_posicion_archivo_bloques(16) + 6,13);
//
//		printf("Leido: %s\n",otroString2);

		void* leo_archivo_2 = leer_archivo("campeonesLibertadores",70,13);

		char* l2aux = leo_archivo_2;

		printf("Leido con fread: %s\n:",l2aux);

		free(leo_archivo_2);

//		char* stringPrueba3 = "Racing";
//		char* unaDirecc3 = malloc(10);
//		memcpy(unaDirecc3,stringPrueba3,10);
//
//		escribir_archivo("campeonesLibertadores",60,unaDirecc3,10);

//		char* otroString3 = malloc(4);
//		char* otroString4 = malloc(6);
//		char* otroString5 = malloc(10);

		// necesito para leer offset dentro de bloque y cantidad de bytes a escribir en bloque (ver en fwrite)

//		memcpy(otroString3,mapping_archivo_bloques + obtener_posicion_archivo_bloques(14) + 60,4);
//		memcpy(otroString4,mapping_archivo_bloques + obtener_posicion_archivo_bloques(16),6);

//		string_append(&otroString5,otroString3);
//		string_append(&otroString5,otroString4);
//
//		printf("Leido: %s\n",otroString5);

		void* leo_archivo_3;
		leo_archivo_3 = leer_archivo("campeonesLibertadores",60,10);

		char* l3aux = leo_archivo_3;

		printf("Leido con fread: %s\n:",l3aux);

		free(leo_archivo_3);

//		char* stringPrueba4 = "Independiente";
//		char* unaDirecc4 = malloc(16);
//
//		memcpy(unaDirecc4,stringPrueba4,16);
//
//		escribir_archivo("campeonesLibertadores",185,unaDirecc4,15);

//		char* otroString6 = string_new();
//		char* otroString7 = string_new();
//		char* otroString8 = string_new();
//
//		memcpy(otroString6,mapping_archivo_bloques + obtener_posicion_archivo_bloques(17) + 57,7);
//		memcpy(otroString7,mapping_archivo_bloques + obtener_posicion_archivo_bloques(18),9);
//
////		string_append(&otroString8,otroString6);
////		string_append(&otroString8,otroString7);
////
////		printf("Leido: %s\n",otroString8);

		char* leo_archivo_4 = leer_archivo("campeonesLibertadores",185,16);
		printf("Leido con fread: %s\n",leo_archivo_4);

		char* leo_archivo_5 = leer_archivo("Consoles",128,80);
		printf("Leido con fread: %s\n",leo_archivo_5);

		int rta = abrir_archivo("hey");

		printf("%i\n",rta);

		free(leo_archivo_4);

//		free(otroString8);
//		free(otroString7);
//		free(otroString6);
//		free(otroString5);
//		free(otroString4);
//		free(otroString3);
//		free(otroString2);
//		free(otroString);
//		free(unaDirecc4);
		free(unaDireccionCualq);
//		free(s1);
//		free(s2);
//		free(s3);
//		free(s4);
//		free(unaDirecc3);
//		free(unaDirecc2);
//		free(unaDirecc);
}
