################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/libTime/libTimer.c \
../lib/libTime/tc.c 

OBJS += \
./lib/libTime/libTimer.o \
./lib/libTime/tc.o 

C_DEPS += \
./lib/libTime/libTimer.d \
./lib/libTime/tc.d 


# Each subdirectory must supply rules for building sources it contributes
lib/libTime/%.o: ../lib/libTime/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-elf-gcc -DROM_RUN -DkUSE_IP_STACK -DVECTORS_IN_RAM -UkUSE_FAT -I"." -I"/home/luten/wallig/BaseArm7AT91" -O1 -g3 -Wall -c -mcpu=arm7tdmi  -gdwarf-2  -MD -MP -MF -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


