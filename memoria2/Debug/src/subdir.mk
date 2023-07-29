################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/inicio.c \
../src/memoria2.c \
../src/operaciones.c \
../src/segmentacion.c 

C_DEPS += \
./src/inicio.d \
./src/memoria2.d \
./src/operaciones.d \
./src/segmentacion.d 

OBJS += \
./src/inicio.o \
./src/memoria2.o \
./src/operaciones.o \
./src/segmentacion.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2023-1c-DeLorean/shared-2/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/inicio.d ./src/inicio.o ./src/memoria2.d ./src/memoria2.o ./src/operaciones.d ./src/operaciones.o ./src/segmentacion.d ./src/segmentacion.o

.PHONY: clean-src

