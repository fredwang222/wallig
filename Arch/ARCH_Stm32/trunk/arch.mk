export CC=arm-none-eabi-gcc 
export CFLAGS=-DARCH_STM32  -mthumb $(ARCH_DEFINE)  -mcpu=cortex-m3 -mfix-cortex-m3-ldrd -O0 -g3  -Wall -MMD -MP -MF$(patsubst %.o,%.d,$@) -MT$(patsubst %.o,%.d,$@) -Wa,-adhlns=$(patsubst %.o,%.lst,$@)
export LDFLAGS=-Wl,--gc-sections,-Map=$(patsubst %.elf,%.map,$@),-cref,-u,Reset_Handler -L$(ARCH_PATH)/lib -T $(ARCH_PATH)/stm32.ld -mthumb -march=armv7 -mfix-cortex-m3-ldrd $(ARCH_PATH)/lib/libstm32.a
