################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ciclo_instrucciones.c \
../src/cpu2.c \
../src/ejecucion_instrucciones.c \
../src/inicializacion_cpu.c \
../src/serializacion.c \
../src/utils_cpu2.c 

C_DEPS += \
./src/ciclo_instrucciones.d \
./src/cpu2.d \
./src/ejecucion_instrucciones.d \
./src/inicializacion_cpu.d \
./src/serializacion.d \
./src/utils_cpu2.d 

OBJS += \
./src/ciclo_instrucciones.o \
./src/cpu2.o \
./src/ejecucion_instrucciones.o \
./src/inicializacion_cpu.o \
./src/serializacion.o \
./src/utils_cpu2.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2023-1c-DeLorean/shared-2/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/ciclo_instrucciones.d ./src/ciclo_instrucciones.o ./src/cpu2.d ./src/cpu2.o ./src/ejecucion_instrucciones.d ./src/ejecucion_instrucciones.o ./src/inicializacion_cpu.d ./src/inicializacion_cpu.o ./src/serializacion.d ./src/serializacion.o ./src/utils_cpu2.d ./src/utils_cpu2.o

.PHONY: clean-src

