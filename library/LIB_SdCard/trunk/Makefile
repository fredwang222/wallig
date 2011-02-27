ODIR=obj
OBJ=$(ODIR)/LIB_SdCard.o

all: $(ODIR) $(OBJ)
	
obj/%.o : LIB_SdCard.c
	$(CC) -o $@ -c $< $(CFLAGS) -I .  -I$(DRV_SPI_PATH) -I$(DRV_GPIO_PATH) -I$(DRV_TIMER_PATH) -I$(CONFIG_PATH)

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

