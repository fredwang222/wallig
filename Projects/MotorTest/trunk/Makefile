export CC=arm-none-eabi-gcc 
export CFLAGS=-DARCH_STM32 -mthumb -mcpu=cortex-m3 -mfix-cortex-m3-ldrd -O0 -g3 -Wall -I../inc -MMD -MP -MF$$(patsubst %.o,%.d,$$@) -MT$$(patsubst %.o,%.d,$$@) -Wa,-adhlns=$$(patsubst %.o,%.lst,$$@)
export CFLAGSL=-DARCH_STM32 -mthumb -mcpu=cortex-m3 -mfix-cortex-m3-ldrd -O0 -g3 -Wall -I../inc -MMD -MP -MF$(patsubst %.o,%.d,$@) -MT$(patsubst %.o,%.d,$@) -Wa,-adhlns=$(patsubst %.o,%.lst,$@)
ODIR=obj
LDFLAGS=-Wl,--gc-sections,-Map=$(patsubst %.o,%.map,$@),-cref,-u,Reset_Handler -L$(ARCH_INCLUDE_PATH)/.. -T $(ARCH_INCLUDE_PATH)/../../stm32.ld -mthumb -march=armv7 -mfix-cortex-m3-ldrd 

export CONFIG_PATH=Config
export SYSTEM_INCLUDE_PATH=System
export ARCH_INCLUDE_PATH=System/Arch/Stm32/lib/inc
export PROJECT_PATH=$(CURDIR)

EXEC=MiniStm32.elf

SYSTEM_OBJ=$(SYSTEM_INCLUDE_PATH)/obj/SystemStm32.a $(SYSTEM_INCLUDE_PATH)/Arch/Stm32/lib/libstm32.a

#drivers
export DRV_PATH=Driver
#DRV_GPIO
DRV_GPIO_PATH=$(DRV_PATH)/DRV_Gpio
DRV_GPIO_OBJ=$(DRV_GPIO_PATH)/obj/DRV_Gpio.a
#DRV_Pwm
DRV_PWM_PATH=$(DRV_PATH)/DRV_Pwm
DRV_PWM_OBJ=$(DRV_PWM_PATH)/obj/DRV_Pwm.o
#DRV_Uart
DRV_UART_PATH=$(DRV_PATH)/DRV_Uart
DRV_UART_OBJ=$(DRV_UART_PATH)/obj/DRV_Uart.a
#DRV_Timer
DRV_TIMER_PATH=$(DRV_PATH)/DRV_Timer
DRV_TIMER_OBJ=$(DRV_TIMER_PATH)/obj/DRV_Timer.a
#Drivers obj
DRV_OBJ= $(DRV_GPIO_OBJ) $(DRV_PWM_OBJ) $(DRV_TIMER_OBJ) $(DRV_UART_OBJ)
IDRIVERDIR= -I $(DRV_GPIO_PATH) -I $(DRV_PWM_PATH) -I$(DRV_TIMER_PATH) -I$(DRV_UART_PATH)


#lib
export LIB_PATH=Lib
#lib fifo
export LIB_FIFO_PATH=$(LIB_PATH)/LIB_Fifo
LIB_FIFO_OBJ=$(LIB_FIFO_PATH)/obj/LIB_Fifo.o
#lib Cli
 LIB_CLI_PATH=$(LIB_PATH)/LIB_Cli
LIB_CLI_OBJ=$(LIB_CLI_PATH)/obj/LIB_Cli.o $(LIB_CLI_PATH)/obj/LIB_Cli_Help.o 
#lib obj
LIB_OBJ= $(LIB_FIFO_OBJ) $(LIB_CLI_OBJ)
#lib include dir
ILIBDIR = -I $(LIB_FIFO_PATH) -I $(LIB_CLI_PATH)

OBJ = $(ODIR)/Main.o $(ODIR)/MiniStm32.o $(ODIR)/MiniStm32_CLI.o

IDIR= -I . $(IDRIVERDIR) $(ILIBDIR) -I$(CONFIG_PATH) -I$(ARCH_INCLUDE_PATH) -ISystem

all:  DRV_Uart DRV_Gpio DRV_Pwm DRV_Timer LIB_Fifo LIB_Cli System_obj $(EXEC)

$(EXEC):$(OBJ) $(LIB_OBJ) $(DRV_OBJ)   $(SYSTEM_OBJ) 
	$(CC) -o $@ $(LDFLAGS) $^  
	@echo Creat binary file:
	arm-none-eabi-objcopy -O binary $@ $(ODIR)/$(patsubst %.elf,%.bin,$@) 
#@echo Creat disassembly file:
#arm-none-eabi-objdump -h -S -C -r $@  > $(patsubst %.elf,%.lss,$@)

DRV_Gpio: 
	@(cd $(DRV_GPIO_PATH) && $(MAKE))

DRV_Pwm:
	@(cd $(DRV_PWM_PATH) && $(MAKE))
	
DRV_Uart:
	@(cd $(DRV_UART_PATH) && $(MAKE))

DRV_Timer:
	@(cd $(DRV_TIMER_PATH) && $(MAKE))

LIB_Fifo:
	@(cd $(LIB_FIFO_PATH) && $(MAKE))

LIB_Cli:
	@(cd $(LIB_CLI_PATH) && $(MAKE))

System_obj:
	@(cd $(SYSTEM_INCLUDE_PATH) && $(MAKE))

$(ODIR)/%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGSL) $(IDIR)

clean_lib_stm32:
	@(cd $(SYSTEM_INCLUDE_PATH)/Arch/Stm32/lib && $(MAKE) clean)


clean:
	rm -rf obj/*
	@(cd $(DRV_PWM_PATH) && $(MAKE) $@)
	@(cd $(DRV_GPIO_PATH) && $(MAKE) $@)
	@(cd $(DRV_UART_PATH) && $(MAKE) $@)
	@(cd $(DRV_TIMER_PATH) && $(MAKE) $@)
	@(cd $(LIB_CLI_PATH) && $(MAKE) $@)
	@(cd $(LIB_FIFO_PATH) && $(MAKE) $@)
	@(cd $(SYSTEM_INCLUDE_PATH) && $(MAKE) $@)

mrproper: clean
	rm -rf $(EXEC)

