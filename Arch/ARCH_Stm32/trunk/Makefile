ODIR=obj

ifeq ($(SUB_ARCH),SUB_STM32F2XX)
include arch_f2.mk
_OBJLIB = misc.o 
_OBJLIB += stm32f2xx_adc.o stm32f2xx_can.o stm32f2xx_crc.o stm32f2xx_cryp_aes.o stm32f2xx_cryp.o stm32f2xx_cryp_des.o 
_OBJLIB += stm32f2xx_cryp_tdes.o stm32f2xx_dac.o stm32f2xx_dbgmcu.o stm32f2xx_dcmi.o stm32f2xx_dma.o stm32f2xx_exti.o 
_OBJLIB += stm32f2xx_flash.o stm32f2xx_fsmc.o stm32f2xx_gpio.o stm32f2xx_hash.o stm32f2xx_hash_md5.o stm32f2xx_hash_sha1.o 
_OBJLIB += stm32f2xx_i2c.o stm32f2xx_iwdg.o stm32f2xx_pwr.o stm32f2xx_rcc.o stm32f2xx_rng.o stm32f2xx_rtc.o stm32f2xx_sdio.o 
_OBJLIB += stm32f2xx_spi.o stm32f2xx_syscfg.o stm32f2xx_tim.o stm32f2xx_usart.o stm32f2xx_wwdg.o 
_OBJLIB += stm32f2xx_it.o stm32f2xx_Vector.o
OBJLIB = $(patsubst %,$(ODIR)/%,$(_OBJLIB))
endif

all: .system  $(ODIR) $(SYSTEM_LIB_OBJ)


.system: lib stm32.ld

Archive/$(LIB_SOURCE_ARCHIVE_NAME):
	#@echo Archive/$(LIB_SOURCE_ARCHIVE_NAME)
	wget $(LIB_SOURCE_PATH)/$(LIB_SOURCE_ARCHIVE_NAME) -O $@
	

lib: Archive/$(LIB_SOURCE_ARCHIVE_NAME)
	mkdir temp
	mkdir lib
	@unzip Archive/$(LIB_SOURCE_ARCHIVE_NAME) -d temp
	cp -r temp/*/Libraries/* lib/
	rm -rf temp 
	
ifeq ($(SUB_ARCH),SUB_STM32F2XX)
include Makefile.f2
endif

$(ODIR):
	@echo $(STM32LIB_PATH)
	mkdir $@


