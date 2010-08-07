LIBOUT=libstm32.a
ODIR=obj
_OBJLIB = core_cm3.o misc.o stm32f10x_adc.o stm32f10x_bkp.o stm32f10x_can.o stm32f10x_cec.o stm32f10x_crc.o stm32f10x_dac.o stm32f10x_dbgmcu.o stm32f10x_dma.o stm32f10x_exti.o stm32f10x_flash.o stm32f10x_fsmc.o stm32f10x_gpio.o stm32f10x_i2c.o stm32f10x_iwdg.o stm32f10x_pwr.o stm32f10x_rcc.o stm32f10x_rtc.o stm32f10x_sdio.o stm32f10x_spi.o stm32f10x_tim.o stm32f10x_usart.o stm32f10x_wwdg.o system_stm32f10x.o 
OBJLIB = $(patsubst %,$(ODIR)/%,$(_OBJLIB))

all: $(ODIR) $(LIBOUT) 

$(LIBOUT): $(OBJLIB)
	arm-none-eabi-ar -cr $(LIBOUT) $(OBJLIB)

obj/%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS) -I inc

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*.o
	rm $(LIBOUT)
