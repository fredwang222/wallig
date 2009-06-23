################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/appIpStack/appIpStack.c 

OBJS += \
./app/appIpStack/appIpStack.o 

C_DEPS += \
./app/appIpStack/appIpStack.d 


# Each subdirectory must supply rules for building sources it contributes
app/appIpStack/%.o: ../app/appIpStack/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-elf-gcc -DROM_RUN -DkUSE_IP_STACK -DVECTORS_IN_RAM -UkUSE_FAT -I"." -I"/home/luten/wallig/BaseArm7AT91" -O1 -g3 -Wall -c -mcpu=arm7tdmi  -gdwarf-2  -MD -MP -MF -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


