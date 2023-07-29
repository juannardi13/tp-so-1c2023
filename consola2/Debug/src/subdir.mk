################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/consola2.c \
../src/utils_consola2.c 

C_DEPS += \
./src/consola2.d \
./src/utils_consola2.d 

OBJS += \
./src/consola2.o \
./src/utils_consola2.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2023-1c-DeLorean/shared-2/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/consola2.d ./src/consola2.o ./src/utils_consola2.d ./src/utils_consola2.o

.PHONY: clean-src

