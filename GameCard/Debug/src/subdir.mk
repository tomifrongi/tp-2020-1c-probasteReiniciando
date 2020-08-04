################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Gamecard.c \
../src/funcionesEnvio.c \
../src/gm_filesystem.c \
../src/mensajes.c 

OBJS += \
./src/Gamecard.o \
./src/funcionesEnvio.o \
./src/gm_filesystem.o \
./src/mensajes.o 

C_DEPS += \
./src/Gamecard.d \
./src/funcionesEnvio.d \
./src/gm_filesystem.d \
./src/mensajes.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


