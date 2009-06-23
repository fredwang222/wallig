################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/appTest/appTestserial.c 

OBJS += \
./app/appTest/appTestserial.o 

C_DEPS += \
./app/appTest/appTestserial.d 


# Each subdirectory must supply rules for building sources it contributes
app/appTest/%.o: ../app/appTest/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-elf-gcc -DROM_RUN -DVECTORS_IN_RAM -UkUSE_IP_STACK -UkUSE_FAT -I"." -I"/home/luten/wallig/BaseArm7AT91" -O1 -g3 -Wall -c -mcpu=arm7tdmi  -gdwarf-2  -MD -MP -MF -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


