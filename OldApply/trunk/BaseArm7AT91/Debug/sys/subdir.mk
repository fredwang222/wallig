################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../sys/sysManager.c \
../sys/syscalls.c 

S_UPPER_SRCS += \
../sys/startup_SAM7S.S 

OBJS += \
./sys/startup_SAM7S.o \
./sys/sysManager.o \
./sys/syscalls.o 

C_DEPS += \
./sys/sysManager.d \
./sys/syscalls.d 


# Each subdirectory must supply rules for building sources it contributes
sys/%.o: ../sys/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: GCC Assembler'
	arm-elf-gcc -c -mcpu=arm7tdmi  -I. -x assembler-with-cpp -DROM_RUN -DVECTORS_IN_RAM -Wa,-adhlns=startup_SAM7S.lst,-gdwarf-2  -std=gnu99 -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

sys/%.o: ../sys/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-elf-gcc -DROM_RUN -DVECTORS_IN_RAM -UkUSE_IP_STACK -UkUSE_FAT -I"." -I"/home/luten/wallig/BaseArm7AT91" -O1 -g3 -Wall -c -mcpu=arm7tdmi  -gdwarf-2  -MD -MP -MF -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


