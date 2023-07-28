#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <commons/string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


int nro_bloque_escribir_cuando_escribo_en_unico_bloque(int nro_bloque_inicial,t_config* config_fcb_archivo,char* nombre_archivo)
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




//		memcpy(&ret,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_de_punteros_en_archivo_bloques) + off_bloque_punteros,sizeof(uint32_t));

		acceso_a_bloque(&ret,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_de_punteros_en_archivo_bloques) + off_bloque_punteros,sizeof(uint32_t),nombre_archivo,nro_bloque_inicial,nro_bloque_de_punteros_en_archivo_bloques);

	}

	return ret;
}

void escribir_bytes_mismo_bloque(int nro_byte_archivo,int nro_bloque_inicial,void* direccion_fisica,int cantidad_bytes_escribir,t_config* config_fcb_archivo,char* nombre_archivo)
{
	int tamanio_bloque = config_super_bloque_valores.block_size;

	int off_bloque_datos = nro_byte_archivo - (nro_bloque_inicial * tamanio_bloque);

//	printf("Offset dentro de bloque a escribir: %i\n",off_bloque_datos);

	int nro_bloque_inicial_en_archivo_bloques = nro_bloque_escribir_cuando_escribo_en_unico_bloque(nro_bloque_inicial,config_fcb_archivo,nombre_archivo);

//	printf("Nro bloque inicial en archivo bloques: %i\n",nro_bloque_inicial_en_archivo_bloques);


//	memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques) + off_bloque_datos,direccion_fisica,cantidad_bytes_escribir);
//
//	log_info(logger,"Acceso Bloque - Archivo: <%s> - Puntero: <%i> - Bloque Archivo: <%i> - Bloque FileSystem: <%i>",nombre_archivo,nro_byte_archivo,nro_bloque_inicial,nro_bloque_inicial_en_archivo_bloques);

	acceso_a_bloque(mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques) + off_bloque_datos,direccion_fisica,cantidad_bytes_escribir,nombre_archivo,nro_bloque_inicial,nro_bloque_inicial_en_archivo_bloques);
}



void escribir_bytes_en_bloque(int nro_byte_archivo,int nro_bloque_escritura,void* contenido_escribir,int start_escritura,int cantidad_bytes_escribir,int* cantidad_bytes_asignados,t_config* config_fcb_archivo,char* nombre_archivo)
{
//	printf("------------------------------------\n");
//
//	printf("Logs función escribir_bytes_en_bloque\n");

	int tamanio_bloque = config_super_bloque_valores.block_size;

//	printf("Nro bloque escritura %i\n",nro_bloque_escritura);

	int nro_bloque_inicial_en_archivo_bloques = nro_bloque_escribir_cuando_escribo_en_unico_bloque(nro_bloque_escritura,config_fcb_archivo,nombre_archivo);

//	printf("Nro bloque inicial en archivo bloques %i\n",nro_bloque_inicial_en_archivo_bloques);

	int off_bloque_datos = nro_byte_archivo - (nro_bloque_escritura * tamanio_bloque);

	// problema en valgrind
	// si inicializo char* aux = string_substring me dice que es invalid read xq el tamaño default del char* es 1 byte
	// si hago aux y malloc dps lo del malloc no lo puedo liberar

	int bytes_asignados_anterior = *cantidad_bytes_asignados;

	(*cantidad_bytes_asignados) +=  cantidad_bytes_escribir;

//	printf("%i\n",(*cantidad_bytes_asignados));

	char* aux = malloc(cantidad_bytes_escribir);

	memcpy(aux,contenido_escribir + bytes_asignados_anterior,cantidad_bytes_escribir);

	// SOLO PARA MOSTRAR

	char c = '\0';

	aux = realloc(aux,cantidad_bytes_escribir + 1);

	memcpy(aux + cantidad_bytes_escribir,&c,1);

//	printf("%s\n",aux);

	// solo para MOSTRAR



//	memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques) + off_bloque_datos,aux,cantidad_bytes_escribir);
//
//	log_info(logger,"Acceso Bloque - Archivo: <%s> - Puntero: <%i> - Bloque Archivo: <%i> - Bloque FileSystem: <%i>",nombre_archivo,nro_byte_archivo,nro_bloque_escritura,nro_bloque_inicial_en_archivo_bloques);

	acceso_a_bloque(mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques) + off_bloque_datos,aux,cantidad_bytes_escribir,nombre_archivo,nro_bloque_escritura,nro_bloque_inicial_en_archivo_bloques);

	free(aux);

//	printf("Fin Logs función escribir_bytes_en_bloque\n");
//
//	printf("------------------------------------\n");
}

void escribir_bytes_en_bloque_de_principio_a_fin(int nro_bloque,void* contenido_escribir,int start_escritura,t_config* config_fcb_archivo,char* nombre_archivo)
{
	int tamanio_bloque = config_super_bloque_valores.block_size;

	int nro_bloque_inicial_en_archivo_bloques = nro_bloque_escribir_cuando_escribo_en_unico_bloque(nro_bloque,config_fcb_archivo,nombre_archivo);

	memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques),string_substring(contenido_escribir,start_escritura,tamanio_bloque -1),tamanio_bloque);
}

void escribir_bytes_diferentes_bloques(int nro_bloque_inicial,int nro_byte_archivo,void* direccion_fisica,int cantidad_bytes_escribir,t_config* config_fcb_archivo,char* nombre_archivo)
{
//	 dos casos principales -> pasar de bloque 0 (apuntado por puntero directo) a bloque 1 (apuntado por puntero indirecto)
//	 						 -> pasasr de bloque 1>= a otro bloque apuntado por puntero de bloque de punteros
//	printf("\n");
//
//	printf("------------------------------------\n");
//
//	printf("Logs función escribir_bytes_diferentes_bloques\n");

	void* contenido_escribir = malloc(cantidad_bytes_escribir);

	memset(contenido_escribir,0,cantidad_bytes_escribir);

	memcpy(contenido_escribir,direccion_fisica,cantidad_bytes_escribir);

	int tamanio_bloque = config_super_bloque_valores.block_size;

	int cantidad_bytes_escribir_bloque_inicial =  (tamanio_bloque * (nro_bloque_inicial + 1)) - nro_byte_archivo;

	int cantidad_bytes_restan_escribir = cantidad_bytes_escribir - cantidad_bytes_escribir_bloque_inicial;

	int cantidad_bloques_a_acceder = ceil((double) cantidad_bytes_restan_escribir/ (double) tamanio_bloque);

	int cantidad_bytes_asignados = 0;

	// caso racing nro byte 60 nro bloque 0, cantidad bytes escribir
	escribir_bytes_en_bloque(nro_byte_archivo,nro_bloque_inicial,contenido_escribir,0,cantidad_bytes_escribir_bloque_inicial,&cantidad_bytes_asignados,config_fcb_archivo,nombre_archivo);

	nro_byte_archivo += cantidad_bytes_escribir_bloque_inicial;


	// resto -1 porque uno de los bytes escritos fue el \n
	int start_escritura = cantidad_bytes_escribir_bloque_inicial - 1;

	int nro_bloque_archivo = nro_bloque_inicial + 1;

	for(int i = 0; i < cantidad_bloques_a_acceder;i++)
	{

		if(i == cantidad_bloques_a_acceder - 1)
		{
//			printf("Nro byte archivo: %i\n",nro_byte_archivo);

			escribir_bytes_en_bloque(nro_byte_archivo,nro_bloque_archivo,contenido_escribir,start_escritura,cantidad_bytes_restan_escribir,&cantidad_bytes_asignados,config_fcb_archivo,nombre_archivo);
		}
		else
		{

//			escribir_bytes_en_bloque_de_principio_a_fin(nro_bloque_archivo, contenido_escribir, start_escritura,config_fcb_archivo,nombre_archivo);
//
//			cantidad_bytes_restan_escribir -= tamanio_bloque;
//
//			start_escritura += tamanio_bloque;
//
//			nro_byte_archivo += tamanio_bloque - 1;

			escribir_bytes_en_bloque(nro_byte_archivo,nro_bloque_archivo,contenido_escribir,start_escritura,tamanio_bloque,&cantidad_bytes_asignados,config_fcb_archivo,nombre_archivo);

			nro_byte_archivo += tamanio_bloque;

			cantidad_bytes_restan_escribir -= tamanio_bloque;
		}


		nro_bloque_archivo++;
	}

	free(contenido_escribir);

//	printf("------------------------------------\n");
//
//	printf("Fin Logs función escribir_bytes_diferentes_bloques\n");
//
//	printf("\n");


}


void* leer_bytes_mismo_bloque(int nro_bloque_inicial,int nro_byte_archivo,int cantidad_bytes_leer,t_config* config_fcb_archivo,char* nombre_archivo)
{
	int tamanio_bloque = config_super_bloque_valores.block_size;

	int off_bloque_datos = nro_byte_archivo - (nro_bloque_inicial * tamanio_bloque);

	void* contenido_leido = malloc(cantidad_bytes_leer);

	int nro_bloque_inicial_en_archivo_bloques = nro_bloque_escribir_cuando_escribo_en_unico_bloque(nro_bloque_inicial,config_fcb_archivo,nombre_archivo);

//	memcpy(contenido_leido,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques) + off_bloque_datos,cantidad_bytes_leer);
//
//	log_info(logger,"Acceso Bloque - Archivo: <%s> - Puntero: <%i> - Bloque Archivo: <%i> - Bloque FileSystem: <%i>",nombre_archivo,nro_byte_archivo,nro_bloque_inicial,nro_bloque_inicial_en_archivo_bloques);

	acceso_a_bloque(contenido_leido,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques) + off_bloque_datos,cantidad_bytes_leer,nombre_archivo,nro_bloque_inicial,nro_bloque_inicial_en_archivo_bloques);

	return contenido_leido;
}

void leer_bytes_en_bloque(int nro_byte_archivo,int nro_bloque_lectura,void* contenido_leido,int cantidad_bytes_leer,t_config* config_fcb_archivo,int* cantidad_bytes_asignados,char* nombre_archivo)
{
//	printf("------------------------------------\n");
//
//	printf("Logs función leer_bytes_en_bloque\n");

	int cantidad_bytes_asignados_anterior = (*cantidad_bytes_asignados);

//	printf("Cantidad bytes asignados anterior: %i \n",cantidad_bytes_asignados_anterior);

	(*cantidad_bytes_asignados)+=cantidad_bytes_leer;

	int tamanio_bloque = config_super_bloque_valores.block_size;

//	printf("Nro bloque lectura %i\n",nro_bloque_lectura);

	int nro_bloque_inicial_en_archivo_bloques = nro_bloque_escribir_cuando_escribo_en_unico_bloque(nro_bloque_lectura,config_fcb_archivo,nombre_archivo);

//	printf("Nro bloque inicial en archivo bloques %i\n",nro_bloque_inicial_en_archivo_bloques);

	int off_bloque_datos = nro_byte_archivo - (nro_bloque_lectura * tamanio_bloque);

	char* aux = malloc(cantidad_bytes_leer);


//	memcpy(aux,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques) + off_bloque_datos,cantidad_bytes_leer);
//
//	log_info(logger,"Acceso Bloque - Archivo: <%s> - Puntero: <%i> - Bloque Archivo: <%i> - Bloque FileSystem: <%i>",nombre_archivo,nro_byte_archivo,nro_bloque_lectura,nro_bloque_inicial_en_archivo_bloques);

	acceso_a_bloque(aux,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_inicial_en_archivo_bloques) + off_bloque_datos,cantidad_bytes_leer,nombre_archivo,nro_bloque_lectura,nro_bloque_inicial_en_archivo_bloques);

	memcpy(contenido_leido + cantidad_bytes_asignados_anterior,aux,cantidad_bytes_leer);

	free(aux);

//	printf("Fin Logs función leer_bytes_en_bloque\n");
//
//	printf("------------------------------------\n");


}

void* leer_bytes_en_bloques_distintos(int nro_bloque_inicial,int nro_byte_archivo,int cantidad_bytes_leer,t_config* config_fcb_archivo,char* nombre_archivo)
{
	int tamanio_bloque = config_super_bloque_valores.block_size;

	int cantidad_bytes_leer_bloque_inicial =  (tamanio_bloque * (nro_bloque_inicial + 1)) - nro_byte_archivo;

	void* contenido_leido = malloc(cantidad_bytes_leer);

	int cantidad_bytes_asignados = 0;

	int cantidad_bytes_restan_leer = cantidad_bytes_leer - cantidad_bytes_leer_bloque_inicial;

	int cantidad_bloques_a_acceder = ceil((double) cantidad_bytes_restan_leer/ (double) tamanio_bloque);

	leer_bytes_en_bloque(nro_byte_archivo,nro_bloque_inicial,contenido_leido,cantidad_bytes_leer_bloque_inicial,config_fcb_archivo,&cantidad_bytes_asignados,nombre_archivo);

	nro_byte_archivo += cantidad_bytes_leer_bloque_inicial;

	int nro_bloque_archivo = nro_bloque_inicial + 1;

	for(int i = 0; i < cantidad_bloques_a_acceder;i++)
	{
		if(i == cantidad_bloques_a_acceder -1 )
		{
			leer_bytes_en_bloque(nro_byte_archivo,nro_bloque_archivo,contenido_leido,cantidad_bytes_restan_leer,config_fcb_archivo,&cantidad_bytes_asignados,nombre_archivo);
		}
		else
		{
			leer_bytes_en_bloque(nro_byte_archivo,nro_bloque_archivo,contenido_leido,cantidad_bytes_restan_leer,config_fcb_archivo,&cantidad_bytes_asignados,nombre_archivo);

			nro_byte_archivo += tamanio_bloque;

			cantidad_bytes_restan_leer -= tamanio_bloque;
		}

		nro_bloque_archivo++;
	}

//	printf("MIRA LO QUE HICE LOCO: %s\n",contenido_leido);

	return contenido_leido;

}
