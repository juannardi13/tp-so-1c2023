################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/archivos.c \
../src/conexion_kernel_cpu2.c \
../src/conexion_memoria.c \
../src/estados.c \
../src/hilos_kernel2.c \
../src/kernel2.c \
../src/peticiones_file_system.c \
../src/planificadores.c \
../src/recursos.c \
../src/serializacion_y_deserializacion.c \
../src/utils_kernel2.c 

C_DEPS += \
./src/archivos.d \
./src/conexion_kernel_cpu2.d \
./src/conexion_memoria.d \
./src/estados.d \
./src/hilos_kernel2.d \
./src/kernel2.d \
./src/peticiones_file_system.d \
./src/planificadores.d \
./src/recursos.d \
./src/serializacion_y_deserializacion.d \
./src/utils_kernel2.d 

OBJS += \
./src/archivos.o \
./src/conexion_kernel_cpu2.o \
./src/conexion_memoria.o \
./src/estados.o \
./src/hilos_kernel2.o \
./src/kernel2.o \
./src/peticiones_file_system.o \
./src/planificadores.o \
./src/recursos.o \
./src/serializacion_y_deserializacion.o \
./src/utils_kernel2.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2023-1c-DeLorean/shared-2/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/archivos.d ./src/archivos.o ./src/conexion_kernel_cpu2.d ./src/conexion_kernel_cpu2.o ./src/conexion_memoria.d ./src/conexion_memoria.o ./src/estados.d ./src/estados.o ./src/hilos_kernel2.d ./src/hilos_kernel2.o ./src/kernel2.d ./src/kernel2.o ./src/peticiones_file_system.d ./src/peticiones_file_system.o ./src/planificadores.d ./src/planificadores.o ./src/recursos.d ./src/recursos.o ./src/serializacion_y_deserializacion.d ./src/serializacion_y_deserializacion.o ./src/utils_kernel2.d ./src/utils_kernel2.o

.PHONY: clean-src

