################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/libFat/diskio.c \
../lib/libFat/ff.c \
../lib/libFat/tff.c 

OBJS += \
./lib/libFat/diskio.o \
./lib/libFat/ff.o \
./lib/libFat/tff.o 

C_DEPS += \
./lib/libFat/diskio.d \
./lib/libFat/ff.d \
./lib/libFat/tff.d 


# Each subdirectory must supply rules for building sources it contributes
lib/libFat/%.o: ../lib/libFat/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-elf-gcc -DROM_RUN -DVECTORS_IN_RAM -UkUSE_IP_STACK -UkUSE_FAT -I"." -I"/home/luten/wallig/BaseArm7AT91" -O1 -g3 -Wall -c -mcpu=arm7tdmi  -gdwarf-2  -MD -MP -MF -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


