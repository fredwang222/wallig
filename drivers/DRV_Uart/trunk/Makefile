ODIR=obj
OBJ= $(ODIR)/DRV_Uart_Stm32.o 
OUTOBJ=$(ODIR)/DRV_Uart.a

all: $(OUTOBJ)

$(OUTOBJ):$(ODIR) $(OBJ)
	arm-none-eabi-ar -cr $@ $(OBJ)

$(ODIR)/%.o : Arch/Stm32/%.c
	$(CC) -o $@ -c $< $(CFLAGS) -I$(CONFIG_PATH) 

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*
