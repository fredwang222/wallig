ODIR=obj
OBJ=$(ODIR)/DRV_Pwm.o

all: $(ODIR) $(OBJ)
	
obj/%.o : Arch/Stm32/%_Stm32.c
	$(CC) -o $@ -c $< $(CFLAGS) -I . -I$(PROJECT_PATH)/$(CONFIG_PATH) -I$(PROJECT_PATH)/$(ARCH_INCLUDE_PATH)

.PHONY: clean 

$(ODIR):
	mkdir $@

clean:
	rm -rf obj/*

