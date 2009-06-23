################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/libIpStack/uip/psock.c \
../lib/libIpStack/uip/timer.c \
../lib/libIpStack/uip/uip-fw.c \
../lib/libIpStack/uip/uip-neighbor.c \
../lib/libIpStack/uip/uip-split.c \
../lib/libIpStack/uip/uip.c \
../lib/libIpStack/uip/uip_arp.c \
../lib/libIpStack/uip/uiplib.c 

OBJS += \
./lib/libIpStack/uip/psock.o \
./lib/libIpStack/uip/timer.o \
./lib/libIpStack/uip/uip-fw.o \
./lib/libIpStack/uip/uip-neighbor.o \
./lib/libIpStack/uip/uip-split.o \
./lib/libIpStack/uip/uip.o \
./lib/libIpStack/uip/uip_arp.o \
./lib/libIpStack/uip/uiplib.o 

C_DEPS += \
./lib/libIpStack/uip/psock.d \
./lib/libIpStack/uip/timer.d \
./lib/libIpStack/uip/uip-fw.d \
./lib/libIpStack/uip/uip-neighbor.d \
./lib/libIpStack/uip/uip-split.d \
./lib/libIpStack/uip/uip.d \
./lib/libIpStack/uip/uip_arp.d \
./lib/libIpStack/uip/uiplib.d 


# Each subdirectory must supply rules for building sources it contributes
lib/libIpStack/uip/%.o: ../lib/libIpStack/uip/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-elf-gcc -DROM_RUN -DkUSE_IP_STACK -DVECTORS_IN_RAM -UkUSE_FAT -I"." -I"/home/luten/wallig/BaseArm7AT91" -O1 -g3 -Wall -c -mcpu=arm7tdmi  -gdwarf-2  -MD -MP -MF -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


