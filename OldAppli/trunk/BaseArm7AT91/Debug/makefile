################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include sys/subdir.mk
-include lib/libUart/subdir.mk
-include lib/libTime/subdir.mk
-include lib/libTelnet/subdir.mk
-include lib/libSlip/subdir.mk
-include lib/subdir.mk
-include lib/libIpStack/uip/subdir.mk
-include lib/libIpStack/uip/lib/subdir.mk
-include lib/libHttpd/subdir.mk
-include lib/libFat/subdir.mk
-include dv/dvUart/subdir.mk
-include dv/dvTimer/subdir.mk
-include dv/dvSpi/subdir.mk
-include dv/dvPWM/subdir.mk
-include dv/subdir.mk
-include dv/dvExtInt/subdir.mk
-include dv/dvDBGU/subdir.mk
-include cfg/cfgIpStack/subdir.mk
-include app/appTest/subdir.mk
-include app/subdir.mk
-include app/appIpStack/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: BaseArm7AT91

# Tool invocations
BaseArm7AT91: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	arm-elf-gcc -static -mcpu=arm7tdmi -T../sys/AT91SAM7S64-ROM.ld      -O1 -MD -MP -MF .dep/main.elf.d --cref -Wl,-Map=main.map -DROM_RUN -DVECTORS_IN_RAM -o"BaseArm7AT91" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) BaseArm7AT91
	-@echo ' '

post-build:
	-@echo 'Create lst file'
	-arm-elf-objdump -h -S -C BaseArm7AT91 > BaseArm7AT91.lss &&  arm-elf-objcopy -O ihex BaseArm7AT91 BaseArm7AT91.hex
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
