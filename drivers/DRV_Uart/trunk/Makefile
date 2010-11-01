ODIR=obj
OBJ= $(ODIR)/DRV_Uart_Stm32.o $(ODIR)/DRV_Uart_Fifo.o $(ODIR)/DRV_Uart_SLIP.o  $(ODIR)/DRV_Uart.o
OUTOBJ=$(ODIR)/DRV_Uart.a

all: $(OUTOBJ)

$(OUTOBJ):$(ODIR) $(OBJ)
	arm-none-eabi-ar -cr $@ $(OBJ)

$(ODIR)/%.o : Arch/Stm32/%.c
	$(CC) -o $@ -c $< $(CFLAGS) $(IDIR) -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(ARCH_INCLUDE_PATH) -I$(PROJECT_PATH)/$(LIB_Fifo_PATH)

$(ODIR)/%.o : Common/%.c 
	$(CC) -o $@ -c $< $(CFLAGS) -I . -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(ARCH_INCLUDE_PATH) -I$(PROJECT_PATH)/$(LIB_Fifo_PATH)

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*
