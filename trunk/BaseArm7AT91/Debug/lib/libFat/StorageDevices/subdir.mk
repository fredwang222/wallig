################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/libFat/StorageDevices/ata.c \
../lib/libFat/StorageDevices/mmc.c \
../lib/libFat/StorageDevices/usb.c 

OBJS += \
./lib/libFat/StorageDevices/ata.o \
./lib/libFat/StorageDevices/mmc.o \
./lib/libFat/StorageDevices/usb.o 

C_DEPS += \
./lib/libFat/StorageDevices/ata.d \
./lib/libFat/StorageDevices/mmc.d \
./lib/libFat/StorageDevices/usb.d 


# Each subdirectory must supply rules for building sources it contributes
lib/libFat/StorageDevices/%.o: ../lib/libFat/StorageDevices/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-elf-gcc -DROM_RUN -DVECTORS_IN_RAM -I"." -I"/home/luten/wallig/BaseArm7AT91" -O1 -g3 -Wall -c -mcpu=arm7tdmi  -gdwarf-2  -MD -MP -MF -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


