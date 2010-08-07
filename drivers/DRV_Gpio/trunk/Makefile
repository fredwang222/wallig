ODIR=obj
OBJ= $(ODIR)/DRV_Gpio.o $(ODIR)/DRV_Gpio_Cli.o 
OUTOBJ=$(ODIR)/DRV_Gpio.a

all: $(OUTOBJ)

$(OUTOBJ):$(ODIR) $(OBJ)
	arm-none-eabi-ar -cr $@ $(OBJ)

$(ODIR)/%.o : Arch/Stm32/%_Stm32.c
	$(CC) -o $@ -c $< $(CFLAGS)  -I . -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(ARCH_INCLUDE_PATH)

$(ODIR)/%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(ARCH_INCLUDE_PATH)

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*
