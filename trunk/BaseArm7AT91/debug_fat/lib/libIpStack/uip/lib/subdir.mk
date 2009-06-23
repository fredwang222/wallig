################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lib/libIpStack/uip/lib/memb.c 

OBJS += \
./lib/libIpStack/uip/lib/memb.o 

C_DEPS += \
./lib/libIpStack/uip/lib/memb.d 


# Each subdirectory must supply rules for building sources it contributes
lib/libIpStack/uip/lib/%.o: ../lib/libIpStack/uip/lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-elf-gcc -DROM_RUN -DkUSE_FAT -DVECTORS_IN_RAM -UkUSE_IP_STACK -I"." -I"/home/luten/wallig/BaseArm7AT91" -O1 -g3 -Wall -c -mcpu=arm7tdmi  -gdwarf-2  -MD -MP -MF -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


