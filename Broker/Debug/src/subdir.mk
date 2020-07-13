################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BuddySystem.c \
../src/Configuracion.c \
../src/ParticionesDinamicas.c \
../src/broker.c \
../src/colasDeMensajes.c \
../src/funcionesEnvio.c \
../src/tiposMensajesEnMemoria.c 

OBJS += \
./src/BuddySystem.o \
./src/Configuracion.o \
./src/ParticionesDinamicas.o \
./src/broker.o \
./src/colasDeMensajes.o \
./src/funcionesEnvio.o \
./src/tiposMensajesEnMemoria.o 

C_DEPS += \
./src/BuddySystem.d \
./src/Configuracion.d \
./src/ParticionesDinamicas.d \
./src/broker.d \
./src/colasDeMensajes.d \
./src/funcionesEnvio.d \
./src/tiposMensajesEnMemoria.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


