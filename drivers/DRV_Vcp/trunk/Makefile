ODIR=obj
OBJ= $(ODIR)/usb_desc.o $(ODIR)/usb_endp.o $(ODIR)/usb_istr.o $(ODIR)/usb_prop.o $(ODIR)/usb_pwr.o $(ODIR)/hw_config.o
OUTOBJ=$(ODIR)/DRV_Vcp.a

all: $(OUTOBJ)

$(OUTOBJ):$(ODIR) $(OBJ)
	arm-none-eabi-ar -cr $@ $(OBJ)

$(ODIR)/%.o : Arch/Stm32/src/%.c
	$(CC) -o $@ -c $< $(CFLAGS) $(IDIR) -I$(CONFIG_PATH) -I$(ARCH_PATH) -IArch/Stm32/inc -I$(DRV_USB_PATH)/Arch/Stm32/inc

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

