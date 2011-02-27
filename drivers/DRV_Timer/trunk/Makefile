ODIR=obj
OBJ=$(ODIR)/DRV_Timer.o $(ODIR)/DRV_Timer_Cli.o 
OUTOBJ=$(ODIR)/DRV_Timer.a

all: $(OUTOBJ)

$(OUTOBJ):$(ODIR) $(OBJ)
	arm-none-eabi-ar -cr $@ $(OBJ)
	
obj/%.o : Arch/Stm32/%_Stm32.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . $(IDIR) -I$(CONFIG_PATH) -I$(ARCH_PATH)

obj/%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . $(IDIR) -I$(CONFIG_PATH) -I$(ARCH_PATH)

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

