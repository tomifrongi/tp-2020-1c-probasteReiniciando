################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ParticionesDinamicas.c \
../src/broker.c \
../src/colasDeMensajes.c \
../src/funcionesEnvio.c 

OBJS += \
./src/ParticionesDinamicas.o \
./src/broker.o \
./src/colasDeMensajes.o \
./src/funcionesEnvio.o 

C_DEPS += \
./src/ParticionesDinamicas.d \
./src/broker.d \
./src/colasDeMensajes.d \
./src/funcionesEnvio.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


