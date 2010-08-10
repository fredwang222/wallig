ODIR=obj
OBJ= $(ODIR)/DRV_SoftTimer_Stm32.o $(ODIR)/DRV_SoftTimer.o $(ODIR)/DRV_SoftTimer_Cli.o 
OUTOBJ=$(ODIR)/DRV_SoftTimer.a

all: $(OUTOBJ)

$(OUTOBJ):$(ODIR) $(OBJ)
	arm-none-eabi-ar -cr $@ $(OBJ)

$(ODIR)/%.o : Arch/Stm32/%.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(ARCH_INCLUDE_PATH) -I$(PROJECT_PATH)/$(LIB_FIFO_PATH)

$(ODIR)/%.o : Common/%.c 
	$(CC) -o $@ -c $< $(CFLAGS) -I . -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(ARCH_INCLUDE_PATH) -I$(PROJECT_PATH)/$(LIB_FIFO_PATH)

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*
