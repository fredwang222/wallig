ODIR=obj
OBJ= $(ODIR)/usb_core.o $(ODIR)/usb_init.o $(ODIR)/usb_int.o $(ODIR)/usb_mem.o $(ODIR)/usb_regs.o $(ODIR)/usb_sil.o
OUTOBJ=$(ODIR)/DRV_Usb.a

all: $(OUTOBJ)

$(OUTOBJ):$(ODIR) $(OBJ)
	arm-none-eabi-ar -cr $@ $(OBJ)

$(ODIR)/%.o : Arch/Stm32/src/%.c
	$(CC) -o $@ -c $< $(CFLAGS) $(IDIR) -I$(CONFIG_PATH) -I$(ARCH_PATH) -IArch/Stm32/inc

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

