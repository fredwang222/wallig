export SYSTEM_PATH = $(PROJECT_PATH)/System/$(ARCH)
export STM32LIB_PATH = $(SYSTEM_PATH)/lib/STM32F2xx_StdPeriph_Driver
export CMSIS_PATH = $(SYSTEM_PATH)/lib/CMSIS/CM3
export SYS_INCLUDE = -I $(SYSTEM_PATH) -I $(SYSTEM_PATH)/inc_f2 -I $(STM32LIB_PATH) -I $(STM32LIB_PATH)/inc -I $(CMSIS_PATH)/DeviceSupport/ST/STM32F2xx -I $(CMSIS_PATH)/CoreSupport/
export CC=arm-none-eabi-gcc 
export CFLAGS= -mthumb $(ARCH_DEFINE)  -mcpu=cortex-m3 -mfix-cortex-m3-ldrd -O0 -g3  -Wall -MMD -MP -MF$(patsubst %.o,%.d,$@) -MT$(patsubst %.o,%.d,$@) -Wa,-adhlns=$(patsubst %.o,%.lst,$@) $(SYS_INCLUDE)
#export LDFLAGS=-Wl,--gc-sections,-Map=$(patsubst %.elf,%.map,$@),-cref,-u,Reset_Handler -L$(ARCH_PATH)/lib -T $(ARCH_PATH)/stm32.ld -mthumb -march=armv7 -mfix-cortex-m3-ldrd
export LDFLAGS=-Wl,--gc-sections,-Map=$(patsubst %.elf,%.map,$@),-cref,-u,Reset_Handler  -T $(SYSTEM_PATH)/stm32f2xx.ld -mthumb -march=armv7 -mfix-cortex-m3-ldrd  
export SYSTEM_LIB_OBJ =$(SYSTEM_PATH)/libstm32.a
