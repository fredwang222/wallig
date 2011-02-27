ODIR=obj
OBJ= $(ODIR)/DRV_Spi.o 

all: $(ODIR) $(OBJ)

$(ODIR)/%.o : Arch/Stm32/%_Stm32.c
	$(CC) -o $@ -c $< $(CFLAGS)  -I . $(IDIR) -I$(CONFIG_PATH) -I$(ARCH_PATH)

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*
