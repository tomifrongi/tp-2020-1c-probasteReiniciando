################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Cliente.c \
../src/Impresora.c \
../src/MensajesBroker.c \
../src/MensajesGamecard.c \
../src/MensajesSuscripcion.c \
../src/MensajesTeam.c \
../src/ModoBroker.c \
../src/ModoGamecard.c \
../src/ModoSuscriptor.c \
../src/ModoTeam.c \
../src/Scanner.c \
../src/gameboy.c \
../src/protocol.c 

OBJS += \
./src/Cliente.o \
./src/Impresora.o \
./src/MensajesBroker.o \
./src/MensajesGamecard.o \
./src/MensajesSuscripcion.o \
./src/MensajesTeam.o \
./src/ModoBroker.o \
./src/ModoGamecard.o \
./src/ModoSuscriptor.o \
./src/ModoTeam.o \
./src/Scanner.o \
./src/gameboy.o \
./src/protocol.o 

C_DEPS += \
./src/Cliente.d \
./src/Impresora.d \
./src/MensajesBroker.d \
./src/MensajesGamecard.d \
./src/MensajesSuscripcion.d \
./src/MensajesTeam.d \
./src/ModoBroker.d \
./src/ModoGamecard.d \
./src/ModoSuscriptor.d \
./src/ModoTeam.d \
./src/Scanner.d \
./src/gameboy.d \
./src/protocol.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


