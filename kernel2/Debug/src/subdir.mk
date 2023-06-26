################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/conexion_kernel_cpu2.c \
../src/estados.c \
../src/hilos_kernel2.c \
../src/kernel2.c \
../src/planificadores.c \
../src/serializacion_y_deserializacion.c \
../src/utils_kernel2.c 

C_DEPS += \
./src/conexion_kernel_cpu2.d \
./src/estados.d \
./src/hilos_kernel2.d \
./src/kernel2.d \
./src/planificadores.d \
./src/serializacion_y_deserializacion.d \
./src/utils_kernel2.d 

OBJS += \
./src/conexion_kernel_cpu2.o \
./src/estados.o \
./src/hilos_kernel2.o \
./src/kernel2.o \
./src/planificadores.o \
./src/serializacion_y_deserializacion.o \
./src/utils_kernel2.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/Documents/tp-2023-1c-DeLorean/shared-2/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/conexion_kernel_cpu2.d ./src/conexion_kernel_cpu2.o ./src/estados.d ./src/estados.o ./src/hilos_kernel2.d ./src/hilos_kernel2.o ./src/kernel2.d ./src/kernel2.o ./src/planificadores.d ./src/planificadores.o ./src/serializacion_y_deserializacion.d ./src/serializacion_y_deserializacion.o ./src/utils_kernel2.d ./src/utils_kernel2.o

.PHONY: clean-src

