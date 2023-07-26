#include "utils-fileSystem.h"
#include <commons/bitarray.h>
#include <commons/string.h>
#include <sys/mman.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

void reducir_tamanio_cuando_tam_actual_mayor_tam_bloque_y_nuevo_tam_menor_tam_bloque(int nro_ultima_entrada_bloque_pind,int nro_bloque_punteros_indirectos,char* nombre_archivo)
{
	for(int i = 0; i <= nro_ultima_entrada_bloque_pind; i++)
	{
		int off = sizeof(uint32_t) * i;

		int nro_bloque;

		memcpy(&nro_bloque,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_punteros_indirectos) + off,sizeof(uint32_t));

		acceso_a_bloque(&nro_bloque,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_punteros_indirectos) + off,sizeof(uint32_t),nombre_archivo,i+1,nro_bloque);

		acceso_escritura_bitmap(nro_bloque,0);
	}

	acceso_escritura_bitmap(nro_bloque_punteros_indirectos,0);
}

void reducir_tamanio_cuando_tam_actual_mayor_tam_bloque_y_nuevo_tam_mayor_tam_bloque(int nro_ultima_entrada_bloque_pind,int nro_bloque_punteros_indirectos,int cantidad_bloques_apuntados_necesarios,char* nombre_archivo)
{

	for(int i = nro_ultima_entrada_bloque_pind; i >= cantidad_bloques_apuntados_necesarios;i--)
	{
		int off = sizeof(uint32_t) * i;

		int nro_bloque;

		memcpy(&nro_bloque,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_punteros_indirectos) + off,sizeof(uint32_t));

		acceso_a_bloque(&nro_bloque,mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_punteros_indirectos) + off,sizeof(uint32_t),nombre_archivo,i+1,nro_bloque);

		acceso_escritura_bitmap(nro_bloque,0);
	}
}


void ampliar_con_tam_actual_cero_y_tam_nuevo_menor_igual_tam_bloque(t_config* config_fcb_archivo)
{
	int puntero_directo = primer_bloque_disponible();

//	bitarray_set_bit(estructura_bitmap,puntero_directo);

	acceso_escritura_bitmap(puntero_directo,1);

	char* puntero_directo_string = string_itoa(puntero_directo);

	config_set_value(config_fcb_archivo,"PUNTERO_DIRECTO",puntero_directo_string);

	free(puntero_directo_string);
}

void ampliar_con_tam_actual_cero_y_tam_nuevo_mayor_tam_bloque(int nuevo_tamanio_entero,t_config* config_fcb_archivo,char* nombre_archivo)
{
	int tamanio_bloque = config_super_bloque_valores.block_size;

	int cantidad_punteros_bloque = config_super_bloque_valores.block_size / sizeof(uint32_t);

	int cantidad_bloques_puntero_indirecto = ceil((((double)(nuevo_tamanio_entero - tamanio_bloque)) / ((double)tamanio_bloque)));

	int puntero_directo = primer_bloque_disponible();

//	bitarray_set_bit(estructura_bitmap,puntero_directo);

	acceso_escritura_bitmap(puntero_directo,1);

	char* puntero_directo_string = string_itoa(puntero_directo);

	config_set_value(config_fcb_archivo,"PUNTERO_DIRECTO",puntero_directo_string);

	free(puntero_directo_string);

	int puntero_indirecto = primer_bloque_disponible();

//	bitarray_set_bit(estructura_bitmap,puntero_indirecto);

	acceso_escritura_bitmap(puntero_indirecto,1);

	char* puntero_indirecto_string = string_itoa(puntero_indirecto);

	config_set_value(config_fcb_archivo,"PUNTERO_INDIRECTO",puntero_indirecto_string);

	free(puntero_indirecto_string);

	for(int i = 0; i <  cantidad_punteros_bloque;i++)
	{
		int off = sizeof(uint32_t) * i;

		if(i < cantidad_bloques_puntero_indirecto)
		{

			int bloque_datos = primer_bloque_disponible();

//			bitarray_set_bit(estructura_bitmap,bloque_datos);

			acceso_escritura_bitmap(bloque_datos,1);

//			memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto) + off,&bloque_datos,sizeof(uint32_t));

			acceso_a_bloque(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto) + off,&bloque_datos,sizeof(uint32_t),nombre_archivo,i+1,bloque_datos);

		}
		else
		{
			int menos_uno = -1;
			memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto)+ off,&menos_uno,sizeof(uint32_t));
		}
	}
}

void ampliar_con_tam_actual_menor_tam_bloque_tam_nuevo_mayor_tam_bloque(int nuevo_tamanio_entero,t_config* config_fcb_archivo,char* nombre_archivo)
{
	int cantidad_punteros_bloque = config_super_bloque_valores.block_size / sizeof(uint32_t);

	int tamanio_bloque = config_super_bloque_valores.block_size;

	int cantidad_bloques_puntero_indirecto = ceil((double)(nuevo_tamanio_entero - tamanio_bloque) / (double)tamanio_bloque);

	int puntero_indirecto = primer_bloque_disponible();

//	bitarray_set_bit(estructura_bitmap,puntero_indirecto);

	acceso_escritura_bitmap(puntero_indirecto,1);

	char* puntero_indirecto_string = string_itoa(puntero_indirecto);

	config_set_value(config_fcb_archivo,"PUNTERO_INDIRECTO",puntero_indirecto_string);

	free(puntero_indirecto_string);

	for(int i = 0; i <  cantidad_punteros_bloque;i++)
	{

		int off = sizeof(uint32_t) * i;

		if(i < cantidad_bloques_puntero_indirecto)
		{

			int bloque_datos = primer_bloque_disponible();

			acceso_escritura_bitmap(bloque_datos,1);

//			memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto) + off,&bloque_datos,sizeof(uint32_t));

			acceso_a_bloque(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto) + off,&bloque_datos,sizeof(uint32_t),nombre_archivo,i+1,bloque_datos);
		}
		else
		{
			int menos_uno = -1;
			memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(puntero_indirecto) + off,&menos_uno,sizeof(uint32_t));
		}
	}
}

void ampliar_con_tam_actual_mayor_tam_bloque(int tamanio_fcb, int nuevo_tamanio_entero,t_config* config_fcb_archivo,char* nombre_archivo)
{
	int tamanio_bloque = config_super_bloque_valores.block_size;

	int cantidad_indirecciones_usadas = ceil((double)(tamanio_fcb - tamanio_bloque)/(double)tamanio_bloque);

	int tamanio_necesario = nuevo_tamanio_entero - tamanio_fcb;

	int cantidad_punteros_indirectos = ceil((double)tamanio_necesario / (double)tamanio_bloque);

	int nro_bloque_punteros_indirectos = config_get_int_value(config_fcb_archivo,"PUNTERO_INDIRECTO");

	for(int i = cantidad_indirecciones_usadas; i < (cantidad_indirecciones_usadas + cantidad_punteros_indirectos) ;i++)
	{
		int off = sizeof(uint32_t) * i;

		int bloque_datos = primer_bloque_disponible();

//		bitarray_set_bit(estructura_bitmap,bloque_datos);

		acceso_escritura_bitmap(bloque_datos,1);

//		printf("%i\n",bloque_datos);

//		memcpy(mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_punteros_indirectos) + off,&bloque_datos,sizeof(uint32_t));

		acceso_a_bloque(mapping_archivo_bloques + obtener_posicion_archivo_bloques(nro_bloque_punteros_indirectos) + off,&bloque_datos,sizeof(uint32_t),nombre_archivo,i+1,bloque_datos);



	}

}
