ODIR=obj
OBJ= $(ODIR)/LIB_nRF24l01.o 

all: $(ODIR) $(OBJ)

$(ODIR)/%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS)  -I . -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(ARCH_INCLUDE_PATH) -I$(PROJECT_PATH)/$(DRV_SPI_PATH) -I$(PROJECT_PATH)/$(DRV_GPIO_PATH) -I$(PROJECT_PATH)/$(DRV_TIMER_PATH)

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*
