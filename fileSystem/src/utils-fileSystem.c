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


		memcpy(unaDireccionCualq,string_substring(unString,0,4),5);

		memcpy(unaDireccionCualq + 6,string_substring(unString,4,4),5);

		memcpy(unaDireccionCualq + 14,string_substring(unString,8,7),8);

		char* s1 = string_new();
		char* s2 = string_new();
		char* s4 = string_new();

		memcpy(s1,unaDireccionCualq,5);

		memcpy(s2,unaDireccionCualq + 6,5);

		memcpy(s4,unaDireccionCualq + 14,8);

		char* s3 = string_new();

		string_append(&s3,s1);
		string_append(&s3,s2);
		string_append(&s3,s4);

		printf("%s\n",s1);
		printf("%s\n",s2);
		printf("%s\n",s4);
		printf("%s\n",s3);

		char* stringPrueba = "River Plate";
		char* unaDirecc = malloc(12);
		memcpy(unaDirecc,stringPrueba,12);

		char* mockDirecc = malloc(20);

		escribir_archivo("campeonesLibertadores",30,unaDirecc,12);

		// se que el primer bloque del archivo es el 14 (me fijé en fcb). Si me voy a este primer bloque y me desplazo 30 y leo tendría que obtener el string

		char* otroString = string_new();

		memcpy(otroString,mapping_archivo_bloques + obtener_posicion_archivo_bloques(14) + 30,12);

		printf("Leido: %s\n",otroString);

		printf("Leido con fread: %s\n:",leer_archivo("campeonesLibertadores",30,mockDirecc,12));

		// funca

		char* stringPrueba2 = "Boca Juniors";
		char* unaDirecc2 = malloc(13);
		memcpy(unaDirecc2,stringPrueba2,13);

		escribir_archivo("campeonesLibertadores",70,unaDirecc2,13);

		// se que el primer bloque del archivo es el 14 (me fijé en fcb). Si me voy a este primer bloque y me desplazo 30 y leo tendría que obtener el string

		char* otroString2 = string_new();

		memcpy(otroString2,mapping_archivo_bloques + obtener_posicion_archivo_bloques(16) + 6,13);

		printf("Leido: %s\n",otroString2);

		printf("Leido con fread: %s\n:",leer_archivo("campeonesLibertadores",70,mockDirecc,13));

		char* stringPrueba3 = "Racing";
		char* unaDirecc3 = malloc(10);
		memcpy(unaDirecc3,stringPrueba3,10);

		escribir_archivo("campeonesLibertadores",60,unaDirecc3,10);

		char* otroString3 = string_new();
		char* otroString4 = string_new();
		char* otroString5 = string_new();

		// necesito para leer offset dentro de bloque y cantidad de bytes a escribir en bloque (ver en fwrite)

		memcpy(otroString3,mapping_archivo_bloques + obtener_posicion_archivo_bloques(14) + 60,4);
		memcpy(otroString4,mapping_archivo_bloques + obtener_posicion_archivo_bloques(16),6);

		string_append(&otroString5,otroString3);
		string_append(&otroString5,otroString4);

		printf("Leido: %s\n",otroString5);

		printf("Leido con fread: %s\n:",leer_archivo("campeonesLibertadores",60,mockDirecc,10));

		char* stringPrueba4 = "Independiente";
		char* unaDirecc4 = malloc(16);
		memcpy(unaDirecc4,stringPrueba4,16);

		escribir_archivo("campeonesLibertadores",185,unaDirecc4,16);

		char* otroString6 = string_new();
		char* otroString7 = string_new();
		char* otroString8 = string_new();

		memcpy(otroString6,mapping_archivo_bloques + obtener_posicion_archivo_bloques(17) + 57,7);
		memcpy(otroString7,mapping_archivo_bloques + obtener_posicion_archivo_bloques(18),9);

		string_append(&otroString8,otroString6);
		string_append(&otroString8,otroString7);

		printf("Leido: %s\n",otroString8);

		printf("Leido con fread: %s\n:",leer_archivo("campeonesLibertadores",185,mockDirecc,16));
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


//----------------------------------------------------------------------------
// ----------------------- ABRIR ARCHIVO -------------------------------------
//----------------------------------------------------------------------------


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

//----------------------------------------------------------------------------
// ----------------------- ABRIR ARCHIVO -------------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ----------------------- CREAR ARCHIVO -------------------------------------
//----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------
// ----------------------- CREAR ARCHIVO -----------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ----------------------- TRUNCAR ARCHIVO -----------------------------------
//----------------------------------------------------------------------------

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

//----------------------------------------------------------------------------
// ----------------------- TRUNCAR ARCHIVO -----------------------------------
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// ----------------------- ESCRIBIR ARCHIVO ----------------------------------
//----------------------------------------------------------------------------

int nro_bloque_escribir_cuando_escribo_en_unico_bloque(int nro_bloque_inicial,t_config* config_fcb_archivo)
{
	int ret = -1;
	if(nro_bloque_inicial == 0)
	{
		ret = config_get_int_value(config_fcb_archivo,"PUNTERO_DIRECTO");
	}
	else
	{
		int nro_entrada_bloque_punteros = nro_bloque_inicial - 1;

		int off_bloque_punteros = nro_entrada_bloque_punteros * sizeof(uint32_t);

		int nro_bloque_de_punteros_en_archivo_bloques = config_get_int_value(config_fcb_archivo,"PUNTERO_INDIRECTO");

		memcpy(&ret,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_de_punteros_en_archivo_bloques) + off_bloque_punteros,sizeof(uint32_t));
	}

	return ret;
}

void escribir_bytes_mismo_bloque(int nro_byte_archivo,int nro_bloque_inicial,char* direccion_fisica,int cantidad_bytes_escribir,t_config* config_fcb_archivo)
{
	int tamanio_bloque = config_super_bloque_valores->block_size;

	int off_bloque_datos = nro_byte_archivo - (nro_bloque_inicial * tamanio_bloque);

	printf("Offset dentro de bloque a escribir: %i\n",off_bloque_datos);

	int nro_bloque_inicial_en_archivo_bloques = nro_bloque_escribir_cuando_escribo_en_unico_bloque(nro_bloque_inicial,config_fcb_archivo);

	printf("Nro bloque inicial en archivo bloques: %i\n",nro_bloque_inicial_en_archivo_bloques);

	memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques) + off_bloque_datos,direccion_fisica,cantidad_bytes_escribir);

}

void escribir_bytes_en_bloque(int nro_byte_archivo,int nro_bloque_escritura,char* contenido_escribir,int start_escritura,int cantidad_bytes_escribir,t_config* config_fcb_archivo)
{
	printf("------------------------------------\n");

	printf("Logs función escribir_bytes_en_bloque\n");

	int tamanio_bloque = config_super_bloque_valores->block_size;

	printf("Nro bloque escritura %i\n",nro_bloque_escritura);

	int nro_bloque_inicial_en_archivo_bloques = nro_bloque_escribir_cuando_escribo_en_unico_bloque(nro_bloque_escritura,config_fcb_archivo);

	printf("Nro bloque inicial en archivo bloques %i\n",nro_bloque_inicial_en_archivo_bloques);

	int off_bloque_datos = nro_byte_archivo - (nro_bloque_escritura * tamanio_bloque);

	printf("%s\n",string_substring(contenido_escribir,start_escritura,cantidad_bytes_escribir - 1));

	memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques) + off_bloque_datos,string_substring(contenido_escribir,start_escritura,cantidad_bytes_escribir - 1),cantidad_bytes_escribir);

	printf("Fin Logs función escribir_bytes_en_bloque\n");

	printf("------------------------------------\n");
}

void escribir_bytes_en_bloque_de_principio_a_fin(int nro_bloque,char* contenido_escribir,int start_escritura,t_config* config_fcb_archivo)
{
	int tamanio_bloque = config_super_bloque_valores->block_size;

	int nro_bloque_inicial_en_archivo_bloques = nro_bloque_escribir_cuando_escribo_en_unico_bloque(nro_bloque,config_fcb_archivo);

	memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques),string_substring(contenido_escribir,start_escritura,tamanio_bloque -1),tamanio_bloque);
}

void escribir_bytes_diferentes_bloques(int nro_bloque_inicial,int nro_byte_archivo,char* direccion_fisica,int cantidad_bytes_escribir,t_config* config_fcb_archivo)
{
//	 dos casos principales -> pasar de bloque 0 (apuntado por puntero directo) a bloque 1 (apuntado por puntero indirecto)
//	 						 -> pasasr de bloque 1>= a otro bloque apuntado por puntero de bloque de punteros
	printf("\n");

	printf("------------------------------------\n");

	printf("Logs función escribir_bytes_diferentes_bloques\n");

	char* contenido_escribir = string_new();

	memcpy(contenido_escribir,direccion_fisica,cantidad_bytes_escribir);

	int tamanio_bloque = config_super_bloque_valores->block_size;

	int cantidad_bytes_escribir_bloque_inicial =  (tamanio_bloque * (nro_bloque_inicial + 1)) - nro_byte_archivo;

	int cantidad_bytes_restan_escribir = cantidad_bytes_escribir - cantidad_bytes_escribir_bloque_inicial;

	int cantidad_bloques_a_acceder = ceil((double) cantidad_bytes_restan_escribir/ (double) tamanio_bloque);


	// caso racing nro byte 60 nro bloque 0, cantidad bytes escribir
	escribir_bytes_en_bloque(nro_byte_archivo,nro_bloque_inicial,contenido_escribir,0,cantidad_bytes_escribir_bloque_inicial,config_fcb_archivo);

	nro_byte_archivo += cantidad_bytes_escribir_bloque_inicial;


	// resto -1 porque uno de los bytes escritos fue el \n
	int start_escritura = cantidad_bytes_escribir_bloque_inicial - 1;


	for(int i = 0; i < cantidad_bloques_a_acceder;i++)
	{
		int nro_bloque_archivo = i + 1;

		if(i == cantidad_bloques_a_acceder - 1)
		{
			printf("Nro byte archivo: %i\n",nro_byte_archivo);

			escribir_bytes_en_bloque(nro_byte_archivo,nro_bloque_archivo,contenido_escribir,start_escritura,cantidad_bytes_restan_escribir,config_fcb_archivo);
		}
		else
		{

			escribir_bytes_en_bloque_de_principio_a_fin(nro_bloque_archivo, contenido_escribir, start_escritura,config_fcb_archivo);

			cantidad_bytes_restan_escribir -= tamanio_bloque;

			start_escritura += tamanio_bloque;

			nro_byte_archivo += tamanio_bloque - 1;
		}
	}


	printf("------------------------------------\n");

	printf("Fin Logs función escribir_bytes_diferentes_bloques\n");

	printf("\n");


}

// SET RAX SonicTheHedgehog guarda en registro rax SonicTheHedgehog
// MOV_OUT 0 RAX escribo en la dirección de memoria física  obtenida a partir de la dirección lógica SonicTheHedgehog
// luego F_WRITE 0 12 bytes. SE busca en dir log 0 lo que hay (sonic hedgehog) y se escribe segun el puntero indicado por kernel

void escribir_archivo(char* nombre_archivo,int nro_byte_archivo,char* direccion_fisica,int cantidad_bytes_escribir)
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

		int tamanio_bloque = config_super_bloque_valores->block_size;

		int nro_byte_final = nro_byte_archivo + cantidad_bytes_escribir - 1;

		printf("Número de byte final de archivo a escribir: %i\n",nro_byte_final);


		int nro_bloque_inicial = floor((double) nro_byte_archivo / (double) tamanio_bloque);

		int nro_bloque_final = floor((double) nro_byte_final / (double) tamanio_bloque);

		printf("Número de bloque inicial donde se escribe: %i\n",nro_bloque_inicial);

		printf("Número de bloque final donde se escribe %i\n",nro_bloque_final);

		if(nro_bloque_inicial == nro_bloque_final)
		{
			printf("Llegué\n");
			escribir_bytes_mismo_bloque(nro_byte_archivo,nro_bloque_inicial,direccion_fisica,cantidad_bytes_escribir,config_fcb_archivo);
		}
		else
		{
			escribir_bytes_diferentes_bloques(nro_bloque_inicial,nro_byte_archivo,direccion_fisica,cantidad_bytes_escribir,config_fcb_archivo);
		}

		log_info(logger,"Escribir Archivo: <%s> - Puntero <%i> - Memoria <%i> - Tamaño <%i>",nombre_archivo,nro_byte_archivo,direccion_fisica,cantidad_bytes_escribir);
	}
}

//----------------------------------------------------------------------------
// ----------------------- ESCRIBIR ARCHIVO ----------------------------------
//----------------------------------------------------------------------------


// un ejemplo F_WRITE Consoles 32 80
// osea le pasa nombre archivo dir lógica y cantidad bytes
// esto recibe cpu imagino y le pasa a kernel. Después kernel pasa a filesystem el nombre del archivo, el puntero actual donde se encuentra según la tabla de archivos
// abiertos por proceso, y la cantidad de bytes a leer


//----------------------------------------------------------------------------
// ----------------------- LEER ARCHIVO --------------------------------------
//----------------------------------------------------------------------------

char* leer_bytes_mismo_bloque(int nro_bloque_inicial,int nro_byte_archivo,int cantidad_bytes_leer,t_config* config_fcb_archivo)
{
	int tamanio_bloque = config_super_bloque_valores->block_size;

	int off_bloque_datos = nro_byte_archivo - (nro_bloque_inicial * tamanio_bloque);

	char* contenido_leido = malloc(cantidad_bytes_leer);

	int nro_bloque_inicial_en_archivo_bloques = nro_bloque_escribir_cuando_escribo_en_unico_bloque(nro_bloque_inicial,config_fcb_archivo);

	memcpy(contenido_leido,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques) + off_bloque_datos,cantidad_bytes_leer);

	return contenido_leido;
}

void leer_bytes_en_bloque(int nro_byte_archivo,int nro_bloque_lectura,char* contenido_leido,int cantidad_bytes_leer,t_config* config_fcb_archivo)
{
	printf("------------------------------------\n");

	printf("Logs función escribir_bytes_en_bloque\n");

	int tamanio_bloque = config_super_bloque_valores->block_size;

	printf("Nro bloque escritura %i\n",nro_bloque_lectura);

	int nro_bloque_inicial_en_archivo_bloques = nro_bloque_escribir_cuando_escribo_en_unico_bloque(nro_bloque_lectura,config_fcb_archivo);

	printf("Nro bloque inicial en archivo bloques %i\n",nro_bloque_inicial_en_archivo_bloques);

	int off_bloque_datos = nro_byte_archivo - (nro_bloque_lectura * tamanio_bloque);

	char* aux = string_new();

	memcpy(aux,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques) + off_bloque_datos,cantidad_bytes_leer);

	printf("%s\n",aux);

	string_append(&contenido_leido,aux);

	printf("Fin Logs función escribir_bytes_en_bloque\n");

	printf("------------------------------------\n");
}

char* leer_bytes_en_bloques_distintos(int nro_bloque_inicial,int nro_byte_archivo,int cantidad_bytes_leer,t_config* config_fcb_archivo)
{
	int tamanio_bloque = config_super_bloque_valores->block_size;

	int cantidad_bytes_leer_bloque_inicial =  (tamanio_bloque * (nro_bloque_inicial + 1)) - nro_byte_archivo;

	char* contenido_leido = string_new();

	int cantidad_bytes_restan_leer = cantidad_bytes_leer - cantidad_bytes_leer_bloque_inicial;

	int cantidad_bloques_a_acceder = ceil((double) cantidad_bytes_restan_leer/ (double) tamanio_bloque);

	leer_bytes_en_bloque(nro_byte_archivo,nro_bloque_inicial,contenido_leido,cantidad_bytes_leer_bloque_inicial,config_fcb_archivo);

	nro_byte_archivo += cantidad_bytes_leer_bloque_inicial;

	for(int i = 0; i < cantidad_bloques_a_acceder;i++)
	{
		int nro_bloque_archivo = i + 1;

		if(i == cantidad_bloques_a_acceder -1 )
		{
			leer_bytes_en_bloque(nro_byte_archivo,nro_bloque_archivo,contenido_leido,cantidad_bytes_restan_leer,config_fcb_archivo);
		}
	}

	printf("MIRA LO QUE HICE LOCO: %s\n",contenido_leido);

	return contenido_leido;

}

char* leer_archivo(char* nombre_archivo,int nro_byte_archivo,char* direccion_volcar_lectura,int cantidad_bytes_leer)
{
	char* leido = string_new();

	if(!abrir_archivo(nombre_archivo))
	{
		log_info(logger,"El archivo no existe");
	}
	else
	{

		int tamanio_bloque = config_super_bloque_valores->block_size;

		int nro_bloque_inicial = floor((double) nro_byte_archivo / (double) tamanio_bloque);

		int nro_byte_final = nro_byte_archivo + cantidad_bytes_leer - 1;

		int nro_bloque_final = floor((double) nro_byte_final / (double) tamanio_bloque);

		char* ruta = obtener_ruta_archivo(nombre_archivo);

		t_config* config_fcb_archivo = iniciar_config(ruta);

		if(nro_bloque_inicial == nro_bloque_final)
		{
			leido = leer_bytes_mismo_bloque(nro_bloque_inicial,nro_byte_archivo,cantidad_bytes_leer,config_fcb_archivo);
		}
		else
		{
			leido = leer_bytes_en_bloques_distintos( nro_bloque_inicial, nro_byte_archivo, cantidad_bytes_leer, config_fcb_archivo);
		}
	}

	log_info(logger,"Leer Archivo: <%s> - Puntero <%i> - Memoria <%i> - Tamaño <%i>",nombre_archivo,nro_byte_archivo,direccion_volcar_lectura,cantidad_bytes_leer);

	return leido;
}

//----------------------------------------------------------------------------
// ----------------------- LEER ARCHIVO --------------------------------------
//----------------------------------------------------------------------------

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
