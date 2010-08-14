ODIR=obj
OBJ=$(ODIR)/DRV_Eth.o $(ODIR)/DRV_DM9000.o
OUTOBJ=$(ODIR)/DRV_Eth.a

all: $(OUTOBJ)  $(OBJ)

$(OUTOBJ):$(ODIR) $(OBJ)
	arm-none-eabi-ar -cr $@ $(OBJ)
	
obj/%.o : Common/%.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(ARCH_INCLUDE_PATH) -I$(PROJECT_PATH)/$(DRV_GPIO_PATH) -I$(PROJECT_PATH)/$(LIB_FIFO_PATH) -I$(PROJECT_PATH)/$(LIB_UIP_PATH)

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

