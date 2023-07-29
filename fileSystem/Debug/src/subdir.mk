################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/archivoPruebas.c \
../src/fileSystem.c \
../src/funcionesArchivos.c \
../src/funcionesTruncar.c \
../src/inicializacion_estructuras.c \
../src/servidor_filesystem.c \
../src/utils-fileSystem.c 

C_DEPS += \
./src/archivoPruebas.d \
./src/fileSystem.d \
./src/funcionesArchivos.d \
./src/funcionesTruncar.d \
./src/inicializacion_estructuras.d \
./src/servidor_filesystem.d \
./src/utils-fileSystem.d 

OBJS += \
./src/archivoPruebas.o \
./src/fileSystem.o \
./src/funcionesArchivos.o \
./src/funcionesTruncar.o \
./src/inicializacion_estructuras.o \
./src/servidor_filesystem.o \
./src/utils-fileSystem.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2023-1c-DeLorean/shared-2/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/archivoPruebas.d ./src/archivoPruebas.o ./src/fileSystem.d ./src/fileSystem.o ./src/funcionesArchivos.d ./src/funcionesArchivos.o ./src/funcionesTruncar.d ./src/funcionesTruncar.o ./src/inicializacion_estructuras.d ./src/inicializacion_estructuras.o ./src/servidor_filesystem.d ./src/servidor_filesystem.o ./src/utils-fileSystem.d ./src/utils-fileSystem.o

.PHONY: clean-src

