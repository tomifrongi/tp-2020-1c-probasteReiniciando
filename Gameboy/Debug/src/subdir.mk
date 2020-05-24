################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Cliente.c \
../src/Gameboy.c \
../src/Impresora.c \
../src/MensajesBroker.c \
../src/MensajesGamecard.c \
../src/MensajesSuscripcion.c \
../src/Scanner.c \
../src/protocol.c 

OBJS += \
./src/Cliente.o \
./src/Gameboy.o \
./src/Impresora.o \
./src/MensajesBroker.o \
./src/MensajesGamecard.o \
./src/MensajesSuscripcion.o \
./src/Scanner.o \
./src/protocol.o 

C_DEPS += \
./src/Cliente.d \
./src/Gameboy.d \
./src/Impresora.d \
./src/MensajesBroker.d \
./src/MensajesGamecard.d \
./src/MensajesSuscripcion.d \
./src/Scanner.d \
./src/protocol.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


