################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ProtocoloDeMensajes.c \
../src/conexiones.c \
../src/configuracion.c \
../src/deadlock.c \
../src/entrenadores.c \
../src/main.c \
../src/mapa.c \
../src/planificador.c \
../src/pokemones.c \
../src/team.c 

OBJS += \
./src/ProtocoloDeMensajes.o \
./src/conexiones.o \
./src/configuracion.o \
./src/deadlock.o \
./src/entrenadores.o \
./src/main.o \
./src/mapa.o \
./src/planificador.o \
./src/pokemones.o \
./src/team.o 

C_DEPS += \
./src/ProtocoloDeMensajes.d \
./src/conexiones.d \
./src/configuracion.d \
./src/deadlock.d \
./src/entrenadores.d \
./src/main.d \
./src/mapa.d \
./src/planificador.d \
./src/pokemones.d \
./src/team.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


